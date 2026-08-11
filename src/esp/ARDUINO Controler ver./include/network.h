#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>

// 1. Inicializa la conexión Wi-Fi y configura el servidor MQTT
void network_init(const char* ssid, const char* pass, const char* mqtt_server, int mqtt_port);

// 2. Mantiene vivas las conexiones y maneja reconexiones automáticas
void network_loop();

// 3. Envía (Publica) los datos de los sensores al broker para ser graficados
void mqtt_publish(const char* topic, const char* payload);

#endif // NETWORK_H