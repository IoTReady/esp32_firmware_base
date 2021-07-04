#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "wifi_config_rest.h"
#include "tasks.h"

#define TAG "main"

/**
 * Main entry point of the program.
*/
void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    
    wifi_config_flag = wifi_config_read_flag_nvs();
    if ( !wifi_config_flag )
    {
        ESP_LOGI(TAG, "WiFi not configured yet. Starting config process.");
        wifi_config_start();
    }
    else
    {
        ESP_LOGI(TAG, "WiFi already configured. Reading NVS for params.");
        wifi_sta_init_struct_t wifi_params = wifi_config_read_nvs();
        wifi_init_station(wifi_params.network_ssid, wifi_params.network_password);
    }

    initialize_tasks();
}
