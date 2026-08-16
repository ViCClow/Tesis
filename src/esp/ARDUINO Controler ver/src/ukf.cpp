#include "ukf.h"

// Variables de estado del Filtro (Ejemplo para 1 dimensión)
static float x_est = 0.0; // Estado estimado (Concentración de gas)
static float p_est = 1.0; // Covarianza del error (Incertidumbre)

// Ruido del sistema (Ajustar en la sintonización)
const float Q = 0.01; // Ruido del proceso (Modelo dinámico)
const float R = 0.5;  // Ruido de la medida (Varianza del MQ-136)

void ukf_init() {
    Serial.println("Inicializando Filtro Kalman Inodoro (UKF)...");
    // Aquí inicializaremos los pesos de los Puntos Sigma (Wc, Wm)
    x_est = 0.0; // Asumimos concentración inicial cero
    p_est = 1.0; 
}

float ukf_step(float mq136_volts, float temp, float hum) {
    // ==========================================
    // 1. FASE DE PREDICCIÓN (Modelo Dinámico f(x))
    // ==========================================
    // Proyección del estado hacia adelante
    float x_pred = x_est; // Modelo estático simple por defecto
    float p_pred = p_est + Q;
    
    // ==========================================
    // 2. FASE DE ACTUALIZACIÓN (Modelo de Observación h(x))
    // ==========================================
    // Aquí es donde entra tu marco teórico: 
    // Transformar mq136_volts a Resistencia (Rs) y aplicar 
    // la regresión polinomial para predecir la medida esperada (z_pred)
    
    // -- ESPACIO PARA TU REGRESIÓN POLINOMIAL --
    // float Rs = ... (Cálculo con la ley de Ohm usando mq136_volts)
    // float z_pred = a*pow(Rs, 2) + b*Rs + c; (Ejemplo polinomial)
    float z_pred = mq136_volts; // Placeholder temporal
    
    // Medida real del sensor (innovación)
    float z = mq136_volts; 
    float y = z - z_pred; // Residual
    
    // Ganancia de Kalman (Simplificada para 1D temporalmente)
    float S = p_pred + R;
    float K = p_pred / S;
    
    // Actualización del estado y covarianza
    x_est = x_pred + K * y;
    p_est = (1.0 - K) * p_pred;
    
    return x_est;
}