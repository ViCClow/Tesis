#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>  // Para la pantalla
#include <Wire.h> // Para el I2C del sensor DFRobot
#include <DFRobot_MultiGasSensor.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ==========================================
// 1. VARIABLES GLOBALES
// ==========================================
TFT_eSPI tft = TFT_eSPI(); 

// Configuración de Red 
const char* WIFI_SSID     = /*"dpto1708";*/ "Wifi_LF201";
const char* WIFI_PASS     = /*"victor2501";*/ "wifi_lf201_deco";
const char* MQTT_SERVER   = "192.168.68.65"; // IP del dispositivo funcionando como servidor
const int   MQTT_PORT     = 1883;
const char* MQTT_TOPIC    = "tesis/sensores/reference";

// --- Instancias ---
WiFiClient espClient;
PubSubClient client(espClient);

// --- Funciones de Conexión ---
void setup_wifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

// Instanciamos el objeto del sensor en el bus I2C
// La dirección I2C por defecto de estos sensores suele ser 0x74
DFRobot_GAS_I2C gas(&Wire, 0x74); 

unsigned long tiempoAnterior = 0;
unsigned long ultimoIntentoMQTT = 0;
const long intervalo_ms = 1000;

// Variables de interfaz gráfica
const uint16_t btn_x = 10;
const uint16_t btn_y = 240;
const uint16_t btn_w = 140;
const uint16_t btn_h = 50;
bool modo_alerta = false;

bool sensor_conectado = false;



// ==========================================
// 2. HARDWARE: SENSOR DFROBOT H2S (I2C)
// ==========================================

void init_sensor_h2s() {
    Serial.println("Inicializando Bus I2C y Sensor DFRobot H2S...");
    
    // Inicia el hardware de comunicación I2C (usa los pines SDA/SCL por defecto del ESP32)
    Wire.begin(32, 25); 
    
    // Intenta contactar al sensor DFRobot
    if(!gas.begin()) {
        Serial.println("¡Error! No se encontró el sensor DFRobot. Revisa las conexiones I2C.");
        sensor_conectado = false;
    } else {
        Serial.println("Sensor H2S inicializado con éxito.");
    
        // Activa la compensación de temperatura interna si el módulo lo soporta
        gas.changeAcquireMode(gas.PASSIVITY); 
        delay(1000);
        gas.changeAcquireMode(gas.INITIATIVE);
        sensor_conectado = true;
    }
  
}

float read_sensor_h2s() {
    // La librería lee la concentración calculada internamente por el microchip de DFRobot
    float concentracion = gas.readGasConcentrationPPM();
    return concentracion; 
}

// ==========================================
// 3. HARDWARE: PANTALLA Y TÁCTIL
// ==========================================

void dibujar_boton_interactivo(bool estado_alerta) {
    uint16_t color_fondo = estado_alerta ? TFT_RED : TFT_BLUE;
    tft.fillRect(btn_x, btn_y, btn_w, btn_h, color_fondo);
    tft.drawRect(btn_x, btn_y, btn_w, btn_h, TFT_WHITE); 
    
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(btn_x + 20, btn_y + 15);
    
    if (estado_alerta) {
        tft.print("ALERTA ON");
    } else {
        tft.print("ALERTA OFF");
    }
}

void init_display() {
    Serial.println("Inicializando pantalla TFT...");
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    uint16_t calData[5] = { 275, 3620, 264, 3532, 1 }; 
    tft.setTouch(calData);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(3);
    tft.setCursor(10, 20);
    tft.print("MONITOR DE H2S");
    tft.drawLine(10, 55, 470, 55, TFT_DARKGREY); 
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 70);
    tft.print("Lectura DFRobot:");
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(3);
    tft.setCursor(240, 145);
    tft.print("PPM");

    dibujar_boton_interactivo(modo_alerta);
}

void update_display_data(float concentracion) {
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%.2f", concentracion);

    tft.fillRect(10, 120, 200, 70, TFT_BLACK); 
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(7); 
    tft.setCursor(10, 125);
    tft.print(buffer);
}

// ==========================================
// 4. LÓGICA TÁCTIL
// ==========================================

void procesar_tactil() {
    uint16_t x, y;
    if (tft.getTouch(&x, &y)) {
        if (x > btn_x && x < (btn_x + btn_w) && y > btn_y && y < (btn_y + btn_h)) {
            modo_alerta = !modo_alerta; 
            dibujar_boton_interactivo(modo_alerta);
            Serial.println("Estado de Alerta cambiado.");
            delay(300); 
        }
    }
}

// ==========================================
// 5. PIPELINE PRINCIPAL
// ==========================================

void setup() {
    // 1. Inicializar depuración y pantalla primero
    Serial.begin(115200);
    delay(1000); 

    init_display();
    
    // (Opcional) Puedes mostrar un mensaje en la TFT aquí
    // tft.fillScreen(TFT_BLACK);
    // tft.drawString("Iniciando Sistema...", 10, 10, 2);

    // 2. Inicializar Comunicaciones (WiFi y MQTT)
    // tft.drawString("Conectando WiFi...", 10, 30, 2);
    setup_wifi(); 
    client.setServer(MQTT_SERVER, MQTT_PORT);
    
    // 3. Inicializar Hardware Externo al final
    // Al colocar esto al final, si el sensor falla, 
    // la pantalla igual se dibujará y podrás ver los mensajes.
    // tft.drawString("Iniciando Sensor DFRobot...", 10, 50, 2);
    init_sensor_h2s(); 
    
    // tft.fillScreen(TFT_BLACK); // Limpiar pantalla antes de entrar al loop()
}

void loop() {
    unsigned long tiempoActual = millis();

    // 1. Gestión de MQTT No Bloqueante
    if (!client.connected()) {
        // Intentar reconectar solo cada 5 segundos para no congelar la pantalla
        if (tiempoActual - ultimoIntentoMQTT > 5000) {
            ultimoIntentoMQTT = tiempoActual;
            Serial.println("Intentando conexión MQTT...");
            
            // IMPORTANTE: Usa un ID único para este ESP32
            if (client.connect("ESP32_Referencia_DFRobot")) {
                Serial.println("Conectado al servidor MQTT");
            }
        }
    } else {
        // Si está conectado, mantener vivo el proceso MQTT
        client.loop();
    }

    // 2. Procesar panel táctil (se ejecuta en cada ciclo para máxima respuesta)
    procesar_tactil();

    // 3. Temporizador de lectura y publicación (1000 ms)
    if (tiempoActual - tiempoAnterior >= intervalo_ms) {
        tiempoAnterior = tiempoActual;

        // Leer sensor
        float lectura_h2s = 0.0;
        if (sensor_conectado) {
            lectura_h2s = read_sensor_h2s();
        }
        
        // Actualizar pantalla
        update_display_data(lectura_h2s);
        
        // Imprimir en monitor Serial
        Serial.print("Concentracion H2S (Ref): ");
        Serial.print(lectura_h2s);
        Serial.println(" PPM");

        // Publicar por MQTT solo si estamos conectados
        if (client.connected()) {
            // 1. Determinar el estado del botón en texto
            String estado_boton = modo_alerta ? "ON" : "OFF";
            
            // 2. Construir el payload en formato JSON
            // Resultado esperado: {"ppm": 12.34, "alerta": "ON"}
            String payload = "{\"ppm\":" + String(lectura_h2s, 2) + ", \"alerta\":\"" + estado_boton + "\"}";
            
            // 3. Enviar al broker
            client.publish(MQTT_TOPIC, payload.c_str());
        }
    }
}