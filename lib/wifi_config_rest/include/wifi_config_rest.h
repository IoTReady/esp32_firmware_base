#ifndef __WIFI_CONFIG_REST_H__
#define __WIFI_CONFIG_REST_H__ 1

#include "wifi_station.h"

bool wifi_config_flag;

bool wifi_config_is_provisioned();
wifi_sta_init_struct_t wifi_config_read_nvs();
bool wifi_config_read_flag_nvs();
esp_err_t wifi_config_start();

#endif // __WIFI_CONFIG_REST_H__