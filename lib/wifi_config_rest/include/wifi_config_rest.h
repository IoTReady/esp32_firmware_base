#ifndef __WIFI_CONFIG_REST_H__
#define __WIFI_CONFIG_REST_H__ 1

#include "wifi_station.h"

bool wifi_config_flag;

/**
 * @brief  Read previously configured WiFi parameter from NVS
 * 
 * @return
 *    - wifi_sta_init_struct_t containing SSID and password of the network configured
 * 
 */
wifi_sta_init_struct_t wifi_config_read_nvs();

/**
 * @brief  Check if wifi has already been provisioned.
 * 
 * @return
 *    - true
 *    - false
 * 
 */
bool wifi_config_read_flag_nvs();

/**
 * @brief  Initialize/Reinitialize WiFi configuration for the device.
 * 
 * @note This will re-write the WiFi configuration on the device if it was previously already configured.
 * 
 * @return
 *    - ESP_OK:             Successful
 *    - Others:             Unsuccessful
 * 
 */
esp_err_t wifi_config_start();

#endif // __WIFI_CONFIG_REST_H__