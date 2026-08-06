#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>

// Estructura para agrupar los datos del clima
struct BME280_Data {
    float temperature;
    float humidity;
    float pressure;
};

// Inicializa los periféricos (ADC e I2C)
void sensors_init();

// Lee el valor analógico crudo del MQ-136 (0-4095)
int read_mq136_raw();

// Lee los datos del BME280
BME280_Data read_bme280();

#endif