#include <Arduino.h>
#include "network.h"
#include "sensors.h"

// --- CONFIGURACIÓN DE RED ---
const char* WIFI_SSID     = "dpto1708";
const char* WIFI_PASS     = "victor2501";
const char* MQTT_SERVER   = "192.168.1.7"; // IP del dispositivo funcionando como servidor
const int   MQTT_PORT     = 1883;
const char* MQTT_TOPIC    = "tesis/sensores/ukf";

// --- CONFIGURACIÓN DE TIEMPO (10 Hz) ---
unsigned long tiempoAnterior = 0;
const long intervalo_ms = 100; // 100 ms = 10 Hz para el Filtro UKF

void setup() {
    Serial.begin(115200);
    delay(2000); // Pequeña pausa para que el monitor serial se estabilice al arrancar

    Serial.println("--- Iniciando Sistema DAQ Tesis ---");

    // 1. Iniciar subsistema de sensores
    if (!sensors_init()) {
        Serial.println("ADVERTENCIA: Falló el BME280. El sistema seguirá operando.");
    }

    // 2. Iniciar subsistema de red (Se quedará bloqueado aquí hasta conectar al Wi-Fi)
    network_init(WIFI_SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT);
}

void loop() {
    // 1. Mantener viva la red (reconexiones y tráfico MQTT)
    network_loop();

    // 2. Control de tiempo no bloqueante para el muestreo
    unsigned long tiempoActual = millis();

    if (tiempoActual - tiempoAnterior >= intervalo_ms) {
        tiempoAnterior = tiempoActual;

        // --- Adquisición de Datos ---
        int   mq136_adc = read_mq136_voltage();
        float temp      = read_bme_temperature();
        float hum       = read_bme_humidity();

        // --- Empaquetado de Datos (Formato CSV) ---
        // Buffer para guardar el mensaje (Ej: "4095,25.40,60.50")
        char payload[64]; 
        
        // snprintf ensambla las variables en texto plano de forma segura
        // %d (entero), %.2f (flotante con 2 decimales)
        snprintf(payload, sizeof(payload), "%3f,%.2f,%.2f", mq136_adc, temp, hum);

        // --- Transmisión ---
        mqtt_publish(MQTT_TOPIC, payload);

        // (Opcional) Imprimir en pantalla para depuración visual
        Serial.println(payload); 
    }
}