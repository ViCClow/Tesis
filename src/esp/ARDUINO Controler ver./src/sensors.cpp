#include <Arduino.h>

#include "sensors.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// 1. Crea el objeto del BME280 aquí
// Adafruit_BME280 bme; 

// 2. Define el pin de tu MQ-136 (El XIAO ESP32S3 usa el pin A0)
const int MQ136_PIN = A0; 

void sensors_init() {
    // 3. Inicia el BME280 (Tip: bme.begin(0x76))
    // 4. (Opcional) Configura la resolución del ADC si quieres asegurar los 12 bits
}

int read_mq136() {
    // 5. Lee y retorna el valor analógico
}

// ... función para leer el clima ...