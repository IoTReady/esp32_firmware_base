#ifndef __SOFTAP_INTERFACE_H__
#define __SOFTAP_INTERFACE_H__ 1

#include <esp_wifi.h>

// #define DEFAULT_ESP_WIFI_CHANNEL 1
// #define DEFAULT_ESP_MAX_STA_CONN 4


// // Soft-AP configuration settings
// wifi_config_t wifi_config = {
//     .ap = {
//         .channel = DEFAULT_ESP_WIFI_CHANNEL,
//         .max_connection = DEFAULT_ESP_MAX_STA_CONN,
//         .authmode = WIFI_AUTH_WPA_WPA2_PSK
//     },
// };

void wifi_restart();

void wifi_set_credentials(char* ssid, char* password);

/**
 * @brief  Initialize softAP
 * 
 * Create event loop and register event handler. Configures and starts a softAP with provided configuration.
 *
 * @param wifi_ssid         SSID of ESP32 soft-AP
 * @param wifi_password     Password of ESP32 soft-AP
 * @param channel           Channel of ESP32 soft-AP 
 * @param max_connections   Max number of stations allowed to connect in (max 10 allowed)
 * 
 * 
 * @return
 *    - ESP_OK:             Success
 *    - ESP_ERR_NO_MEM:     Not enough memory to initialize softAP
 *    - ESP_ERR_WIFI_NVS:   WiFi internal NVS error
 *    - ESP_ERR_WIFI_CONN:  WiFi internal error, station or soft-AP control block wrong
 * 
 */
esp_err_t wifi_init_softap(char* wifi_ssid, char* wifi_pasword, int channel, int max_connections);

/**
 * @brief  De-initialize softAP
 * 
 * Unregister the event handler to the system event loop. Free all resources allocated and stop WiFi task.
 *
 * 
 * @return
 *    - ESP_OK: Success
 *    - ESP_ERR_WIFI_NOT_INIT: WiFi has not been initialized.
 *    - ESP_ERR_INVALID_ARG: Parameter error
 * 
 */
esp_err_t wifi_deinit_softap();

#endif // __SOFTAP_INTERFACE_H__
