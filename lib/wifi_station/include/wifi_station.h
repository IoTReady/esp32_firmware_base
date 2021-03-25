
#ifndef __WIFI_STATION_H__
#define __WIFI_STATION_H__ 1

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"


/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define DEFAULT_WIFI_SSID       "IoTReady"
#define DEFAULT_WIFI_PASSWORD   "getiotready"

EventGroupHandle_t wifi_sta_get_event_group();


typedef struct wifi_sta_init_struct_ {
    
    // Network SSID to connect to.
    char network_ssid[32];
    
    // Network password.
    char network_password[32];
    
} wifi_sta_init_struct_t;

wifi_sta_init_struct_t wifi_params_sta;

/**
 * @brief  Initialize station
 * 
 * Create event loop and register event handlers. Configures and starts a station. Connects to the target AP provided as parameters.
 *
 * @param wifi_ssid         SSID of target AP. Null terminated string.
 * @param wifi_password     Password of target AP. Null terminated string.
 * 
 * 
 * @return
 *    - ESP_OK:             Success
 *    - ESP_ERR_NO_MEM:     Not enough memory to initialize softAP
 *    - ESP_ERR_WIFI_NVS:   WiFi internal NVS error
 *    - ESP_ERR_WIFI_CONN:  WiFi internal error, station or soft-AP control block wrong
 * 
 */
esp_err_t wifi_init_station(const char *wifi_ssid, const char *wifi_password);

/**
 * @brief  De-initialize station
 * 
 * Unregister the event handlers to the system event loop. Free all resources allocated and stop WiFi task.
 *
 * 
 * @return
 *    - ESP_OK: Success
 *    - ESP_ERR_WIFI_NOT_INIT: WiFi has not been initialized.
 *    - ESP_ERR_INVALID_ARG: Parameter error
 * 
 */
esp_err_t wifi_deinit_station();

#endif // __WIFI_STATION_H__
