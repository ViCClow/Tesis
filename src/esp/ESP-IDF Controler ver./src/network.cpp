#include "network.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include <string.h>

// "TAG" se usa en ESP-IDF para identificar en la consola quién imprimió el mensaje
static const char *TAG = "NETWORK";

// Un "EventGroup" es como una bandera que el FreeRTOS usa para avisar a otras tareas 
// que algo pasó. Aquí la usaremos para avisarle al main que ya tenemos IP.
static EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

// ── 1. Manejador de Eventos (El cerebro del Wi-Fi) ──────────────────────
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // Si el Wi-Fi arrancó, dale la orden de conectarse
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // Si se cae la red (o el router se apaga), reintenta infinitamente
        ESP_LOGW(TAG, "Desconectado del Wi-Fi. Reintentando...");
        esp_wifi_connect();
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        // ¡Éxito! El router nos dio una IP
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "IP asignada: " IPSTR, IP2STR(&event->ip_info.ip));
        
        // Levantamos la bandera para que el main.cpp sepa que puede continuar
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// ── 2. Inicialización del Wi-Fi ─────────────────────────────────────────
void wifi_init_sta(const char* ssid, const char* pass) {
    wifi_event_group = xEventGroupCreate();

    // Inicia las capas de red internas del ESP-IDF
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // Configuración base de la antena Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Suscribimos nuestro event_handler a los eventos de Wi-Fi e IP
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    // Configuramos las credenciales (SSID y Password)
    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password) - 1);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Esperando a que el router nos de acceso...");
    
    // Aquí es donde "pausamos" esta función. Espera a que el event_handler 
    // levante la bandera (WIFI_CONNECTED_BIT) de que ya tenemos IP.
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    
    ESP_LOGI(TAG, "¡Wi-Fi conectado y listo!");
}

// ── 3. Transmisión UDP (Socket crudo) ───────────────────────────────────
void udp_send_data(const char* target_ip, int target_port, const char* payload) {
    // 1. Abrimos un socket estándar de Linux/UNIX para UDP (SOCK_DGRAM)
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Error creando el socket UDP");
        return;
    }

    // 2. Preparamos un "sobre" con la dirección de destino y el puerto
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(target_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(target_port);

    // 3. Enviamos el mensaje (nuestro CSV)
    int err = sendto(sock, payload, strlen(payload), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0) {
        ESP_LOGE(TAG, "Fallo al enviar el paquete");
    }

    // 4. Cerramos el socket para no comer memoria RAM (muy importante en C++)
    close(sock);
}