#include "network.h"
#include <WiFi.h>
#include <MQTT.h>

// Instancias globales
WiFiClient espClient;
MQTTClient mqtt(256); // Buffer de 256 bytes para los mensajes

// Variable para controlar la reconexión sin usar delay()
unsigned long ultimoIntentoReconexion = 0;

void network_init(const char* ssid, const char* pass, const char* mqtt_server, int mqtt_port) {
    Serial.println("Conectando a Wi-Fi...");
    
    // 1. Iniciar conexión Wi-Fi
    WiFi.begin(ssid, pass);
    
    // Bucle bloqueante (Solo ocurre una vez al encender el ESP32)
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\n¡Wi-Fi conectado!");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());

    // 2. Configurar el Broker MQTT
    // Vinculamos la IP, el puerto y el cliente Wi-Fi a nuestra instancia MQTT
    mqtt.begin(mqtt_server, mqtt_port, espClient);
}

void network_loop() {
    // 1. Mantener vivo el tráfico MQTT entrante y los pings
    mqtt.loop();
    
    // 2. Comprobar si el Wi-Fi sigue conectado
    if (WiFi.status() != WL_CONNECTED) {
        // En ESP32, WiFi.reconnect() es una función nativa asíncrona (no bloquea)
        WiFi.reconnect(); 
    }

    // 3. Comprobar si MQTT se desconectó
    if (!mqtt.connected()) {
        unsigned long tiempoActual = millis();
        
        // Intentar reconectar solo cada 5000 milisegundos (5 segundos)
        // Esto evita usar delay(5000) que congelaría la lectura de tus sensores
        if (tiempoActual - ultimoIntentoReconexion > 5000) {
            ultimoIntentoReconexion = tiempoActual;
            
            Serial.println("Intentando conectar a MQTT...");
            
            // Si logra conectar, reseteamos el contador de tiempo
            // "ESP32_Tesis" es el nombre único (Client ID) con el que se presenta al broker
            if (mqtt.connect("ESP32_Tesis")) {
                Serial.println("¡Reconectado al Broker MQTT!");
                ultimoIntentoReconexion = 0;
            } else {
                Serial.println("Fallo al conectar MQTT. Reintentando en 5s...");
            }
        }
    }
}

void mqtt_publish(const char* topic, const char* payload) {
    // Solo intentar publicar si la conexión está activa
    if (mqtt.connected()) {
        mqtt.publish(topic, payload);
    }
}