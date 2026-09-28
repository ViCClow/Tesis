#include "sensors.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DFRobot_MultiGasSensor.h> 

// --- DEFINICIÓN DE HARDWARE ---
const int MQ136_PIN = A0; 
Adafruit_BME280 bme;      
DFRobot_GAS_I2C gas(&Wire, 0x74);  
bool dfrobot_conectado = false;

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

    Serial.println("Inicializando Sensor DFRobot H2S...");
    if(!gas.begin()) {
        Serial.println("¡Error! No se encontró el sensor DFRobot.");
        dfrobot_conectado = false;
    } else {
        Serial.println("Sensor DFRobot H2S inicializado con éxito.");
        // Activa la compensación de temperatura interna del módulo
        gas.changeAcquireMode(gas.PASSIVITY); 
        delay(1000);
        gas.changeAcquireMode(gas.INITIATIVE);
        dfrobot_conectado = true;
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

float read_dfrobot_h2s() {
    if (dfrobot_conectado) {
        return gas.readGasConcentrationPPM();
    }
    return 0.0; // Si el sensor falló al inicio, retorna 0 para no colapsar la matemática
}