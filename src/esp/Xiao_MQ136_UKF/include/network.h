#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// ── Declaración de Funciones de Red ────────────────────────────────────

/**
 * @brief Inicializa el hardware Wi-Fi y conecta a la red especificada.
 * Bloquea la ejecución hasta que se obtiene una dirección IP.
 * 
 * @param ssid Nombre de la red Wi-Fi
 * @param pass Contraseña de la red Wi-Fi
 */
void wifi_init_sta(const char* ssid, const char* pass);

/**
 * @brief Envía una cadena de texto (payload) a través de UDP a un destino.
 * 
 * @param target_ip Dirección IP del destinatario (ej. "192.168.1.15")
 * @param target_port Puerto de destino
 * @param payload Datos a enviar (cadena de caracteres terminada en null)
 */
void udp_send_data(const char* target_ip, int target_port, const char* payload);

#ifdef __cplusplus
}
#endif