#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

// Inicializa el BME280 y configura la resolución del ADC
bool sensors_init();

// Devuelve el voltaje real emitido por el sensor MQ-136
float read_mq136_voltage();

// Funciones de lectura climática
float read_bme_temperature();
float read_bme_humidity();

#endif // SENSORS_H