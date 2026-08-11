#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

// Inicializa el BME280 y configura el ADC
bool sensors_init();

// Funciones de lectura pura
int read_mq136();
float read_bme_temperature();
float read_bme_humidity();
float read_bme_pressure();

#endif // SENSORS_H