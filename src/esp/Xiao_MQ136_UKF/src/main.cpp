#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Necesario en C++ para que el sistema operativo encuentre el punto de inicio
extern "C" void app_main(void) {
    
    // ---------------------------------------------------------
    // AQUÍ IRÁ LO QUE ANTES TENÍAS EN TU setup()
    // (Inicialización de pines, I2C y ADC para los sensores)
    // ---------------------------------------------------------
    printf("Inicializando sensores en el XIAO ESP32S3...\n");


    // ---------------------------------------------------------
    // AQUÍ IRÁ LO QUE ANTES TENÍAS EN TU loop()
    // ---------------------------------------------------------
    while (true) {
        
        // Aquí irán las lecturas del BME280 y el MQ-136
        // Y posteriormente el cálculo del Filtro de Kalman Unscented (UKF)

        vTaskDelay(100 / portTICK_PERIOD_MS); // Delay para no colapsar el FreeRTOS
    }
}




// #include <Wire.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BME280.h>
// #include <WiFi.h>
// #include <WiFiUdp.h>

// #define SEALEVELPRESSURE_HPA (1013.25)
// #define MQ136_PIN A0
// #define RL_OHM     6282.5

// // ── Configuración de Red Wi-Fi y UDP ───────────────────────────────────
// const char* ssid = "TU_NOMBRE_DE_RED";       // <-- Cambia esto
// const char* password = "TU_CONTRASENA";      // <-- Cambia esto
// const char* targetIP = "192.168.X.X";        // <-- IP de tu computador (donde escucharás los datos)
// const int targetPort = 1234;                 // Puerto UDP de destino

// Adafruit_BME280 bme;
// unsigned long delayTime;
// WiFiUDP udp;

// void setup() {
//     Serial.begin(9600);
//     delay(2000);

//     // 1. Inicialización de Sensores
//     unsigned status = bme.begin(0x76);
//     if (!status) {
//         Serial.println("ERROR CRITICO: No se detecta el BME280. Error en soldadura I2C.");
//         while (1) delay(10);
//     }

//     // 2. Conexión Wi-Fi
//     Serial.print("Conectando a Wi-Fi: ");
//     Serial.println(ssid);
//     WiFi.begin(ssid, password);

//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");
//     }
//     Serial.println("\n¡Wi-Fi conectado!");
//     Serial.print("Dirección IP del ESP32: ");
//     Serial.println(WiFi.localIP());

//     delayTime = 1000;

//     // Imprimir cabecera CSV en Serial para validación visual
//     Serial.println("Tiempo_ms,Temp_C,Hum_%,Presion_hPa,Rs_Ohmios");
// }

// void loop() {
//     printAndSendValues();
//     delay(delayTime);
// }

// void printAndSendValues() {
//     // ── MQ136: lectura ADC → voltaje → Rs ──────────────────────────────────
//     float v_pin  = (analogRead(MQ136_PIN) / 4095.0) * 3.3;   
//     float v_aout = v_pin * 1.69;                             
//     float Rs     = (v_aout > 0.05) ? RL_OHM * ((5.0 / v_aout) - 1.0) : 0;

//     // ── Construcción del paquete (Payload) CSV ─────────────────────────────
//     // Agrupamos todo en un solo String para garantizar que el paquete UDP 
//     // y la salida Serial sean idénticos en cada ciclo.
//     String payload = String(millis()) + "," +
//                      String(bme.readTemperature(), 2) + "," +
//                      String(bme.readHumidity(), 2) + "," +
//                      String(bme.readPressure() / 100.0F, 2) + "," +
//                      String(Rs, 2);

//     // ── 1. Salida Local (Serial) ───────────────────────────────────────────
//     Serial.println(payload);

//     // ── 2. Salida Inalámbrica (UDP) ────────────────────────────────────────
//     udp.beginPacket(targetIP, targetPort);
//     udp.print(payload);
//     udp.endPacket();
// }