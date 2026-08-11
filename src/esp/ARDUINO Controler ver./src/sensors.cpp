#include "sensors.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h> // Se cargarán desde tu carpeta 'lib'

// 1. Definir hardware
const int MQ136_PIN = A0; // El pin analógico donde está conectado el MQ-136
Adafruit_BME280 bme;      // Instancia del sensor climático por I2C

bool sensors_init() {
    Serial.println("Inicializando sensores...");
    
    // Fijar explícitamente la resolución del ADC a 12 bits (0 a 4095)
    // Esto asegura que la matemática de tu regresión polinomial futura no se rompa
    analogReadResolution(12);
    
    // Iniciar BME280 (La dirección estándar de los módulos suele ser 0x76, a veces 0x77)
    if (!bme.begin(0x76)) {
        Serial.println("Error: No se detecta el sensor BME280. Revisa el cableado I2C.");
        return false;
    }
    
    Serial.println("Sensores listos.");
    return true;
}

int read_mq136() {
    // Retorna el valor ADC crudo (0 - 4095)
    return analogRead(MQ136_PIN);
}

float read_bme_temperature() {
    // Retorna temperatura en grados Celsius
    return bme.readTemperature();
}

float read_bme_humidity() {
    // Retorna humedad relativa en %
    return bme.readHumidity();
}