#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define MQ136_PIN A0
#define RL_OHM     6282.5

Adafruit_BME280 bme;
unsigned long delayTime;

void setup() {
    Serial.begin(9600);
    delay(2000);

    unsigned status = bme.begin(0x76);
    if (!status) {
        Serial.println("ERROR CRITICO: No se detecta el BME280. Error en soldadura I2C.");
        while (1) delay(10);
    }

    delayTime = 1000;

    Serial.println("Tiempo_ms,Temp_C,Hum_%,Presion_hPa,Rs_Ohmios");
}

void loop() {
    printValues();
    delay(delayTime);
}

void printValues() {
    // ── MQ136: lectura ADC → voltaje → Rs ──────────────────────────────────
    float v_pin  = (analogRead(MQ136_PIN) / 4095.0) * 3.3;   // voltaje en el pin [V]
    float v_aout = v_pin * 1.69;                             // voltaje real de salida [V]
    float Rs     = (v_aout > 0.05) ? RL_OHM * ((5.0 / v_aout) - 1.0) : 0;

    // ── Salida CSV estricta: Tiempo(ms),Temp(C),Hum(%),Pres(hPa),Rs(Ohm) ───
    Serial.print(millis());
    Serial.print(",");
    Serial.print(bme.readTemperature());
    Serial.print(",");
    Serial.print(bme.readHumidity());
    Serial.print(",");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.print(",");
    Serial.println(Rs, 2); // println cierra la fila, '2' acota a dos decimales
}
