#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"       // Requisito estricto de ESP-IDF para el Wi-Fi
#include "network.h"         // Nuestro propio módulo de red (que crearemos después)
#include "sensor.h"

// ── Credenciales y Destino ─────────────────────────────────────────────
#define WIFI_SSID      "Wifi_LF201"
#define WIFI_PASS      "wifi_lf201_deco"
#define TARGET_IP      "192.168.68.59"  // IP de tu computador con Parrot OS
#define TARGET_PORT    1234

static const char *TAG = "MAIN";

extern "C" void app_main() {
    // 1. Inicializar la memoria Flash no volátil (NVS)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Inicializando sistema XIAO ESP32S3...");

    // 2. Conectar a Wi-Fi usando la función de tu network.cpp
    wifi_init_sta(WIFI_SSID, WIFI_PASS); 
    
    // Esperamos un momento para asegurar que obtenga IP antes de enviar datos
    vTaskDelay(pdMS_TO_TICKS(3000)); 

    // Nota: Si tu network.cpp no tiene una función udp_init(), 
    // simplemente la omitimos, ya que los sockets se pueden abrir al enviar.

    // 3. Inicializar Sensores
    sensors_init();

    // 4. Imprimir cabecera CSV
    printf("Tiempo_ms,Temp_C,Hum_%%,Presion_hPa,MQ136_Raw\n");

    char payload[128];

    // 5. Ciclo Principal
    while (true) {
        
        // Obtener tiempo en milisegundos
        uint32_t tiempo = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        // Leer datos de los sensores
        int mq136_val = read_mq136_raw();
        BME280_Data bme_val = read_bme280();

        // Construir el Payload CSV
        snprintf(payload, sizeof(payload), "%lu,%.2f,%.2f,%.2f,%d\n", 
                 tiempo, bme_val.temperature, bme_val.humidity, bme_val.pressure, mq136_val);

        // Imprimir localmente
        printf("%s", payload);

        // Enviar por red pasando TODOS los argumentos que pide tu función
        udp_send_data(TARGET_IP, TARGET_PORT, payload);

        // Retardo estricto del sistema (100 ms = 10Hz)
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}