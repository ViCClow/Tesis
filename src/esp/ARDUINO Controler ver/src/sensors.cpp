#include "sensors.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// --- DEFINICIÓN DE HARDWARE ---
const int MQ136_PIN = A0; 
Adafruit_BME280 bme;      

// --- CALIBRACIÓN DEL DIVISOR DE VOLTAJE ---
// R1 = 6.9k (4.7k + 2.2k) y R2 = 10k
const float VOLTAGE_DIVIDER_RATIO = 1.69; 
const float V_REF = 3.3; // Voltaje de referencia de la placa

bool sensors_init() {
    Serial.println("Inicializando sensores...");
    
    // Fijar resolución del ADC a 12 bits (0 a 4095)
    analogReadResolution(12);
    
    // Iniciar BME280 (Dirección I2C: 0x76)
    if (!bme.begin(0x76)) {
        Serial.println("Error: No se detecta el sensor BME280. Revisa el cableado I2C.");
        return false;
    }
    
    Serial.println("Sensores listos.");
    return true;
}

float read_mq136_voltage() {
    int adc_raw = analogRead(MQ136_PIN);
    
    // 1. Mapear el ADC (0-4095) al voltaje que realmente está leyendo el pin (0-3.3V)
    float v_pin = (adc_raw / 4095.0) * V_REF;
    
    // 2. Mapear de vuelta al voltaje original del sensor antes del divisor
    float v_sensor = v_pin * VOLTAGE_DIVIDER_RATIO;
    
    return v_sensor;
}

float read_bme_temperature() {
    return bme.readTemperature();
}

float read_bme_humidity() {
    return bme.readHumidity();
}