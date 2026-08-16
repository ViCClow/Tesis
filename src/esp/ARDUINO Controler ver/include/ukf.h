#ifndef UKF_H
#define UKF_H

#include <Arduino.h>

// Inicializa las matrices de covarianza, pesos y estado inicial del UKF
void ukf_init();

// Ejecuta un ciclo completo de Predicción y Actualización
// Entradas: voltaje crudo del MQ-136, temperatura y humedad
// Salida: Concentración de gas estimada y filtrada
float ukf_step(float mq136_volts, float temp, float hum);

#endif // UKF_H