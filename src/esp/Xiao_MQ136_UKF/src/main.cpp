#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"       // Requisito estricto de ESP-IDF para el Wi-Fi
#include "network.h"         // Nuestro propio módulo de red (que crearemos después)

// ── Credenciales y Destino ─────────────────────────────────────────────
#define WIFI_SSID      "TU_NOMBRE_DE_RED"
#define WIFI_PASS      "TU_CONTRASENA"
#define TARGET_IP      "192.168.X.X"  // IP de tu computador con Parrot OS
#define TARGET_PORT    1234

extern "C" void app_main(void) {
    // 1. Inicializar la memoria Flash no volátil (NVS)
    // En ESP-IDF, el driver de Wi-Fi se niega a iniciar si no tiene acceso 
    // a esta memoria para guardar datos de calibración.
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    printf("Inicializando sistema XIAO ESP32S3...\n");

    // 2. Conectar a Wi-Fi (Llamamos a la función que haremos en network.cpp)
    wifi_init_sta(WIFI_SSID, WIFI_PASS);

    // 3. Imprimir cabecera CSV (Equivalente a tu Serial.println del setup)
    printf("Tiempo_ms,Temp_C,Hum_%%,Presion_hPa,Rs_Ohmios\n");

    // 4. Ciclo Principal (Equivalente a tu loop() en Arduino)
    while (true) {
        
        // Obtener tiempo en milisegundos (Equivalente a millis())
        uint32_t tiempo = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        // --- Variables Simuladas (Hasta que configuremos el BME280 y el ADC) ---
        float temp = 25.0;
        float hum = 50.0;
        float pres = 1013.25;
        float rs = 1500.0;

        // Construir el Payload CSV en un arreglo de caracteres.
        // En ESP-IDF no existe la clase "String", usamos snprintf de C estándar.
        char payload[128];
        snprintf(payload, sizeof(payload), "%lu,%.2f,%.2f,%.2f,%.2f\n", tiempo, temp, hum, pres, rs);

        // Imprimir localmente (Equivalente a Serial.print)
        printf("%s", payload);

        // Enviar por red (Llamamos a la función que haremos en network.cpp)
        udp_send_data(TARGET_IP, TARGET_PORT, payload);

        // Retardo estricto del sistema operativo (Equivalente a delay(1000))
        // Liberamos el procesador para que el ESP32 no colapse.
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}