#include "tasks.h"
#include "wifi_prov.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define TAG "main"

/**
 * Main entry point of the program.
*/
void app_main(void)
{
    // wifi_provision();
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ret = wifi_init_station(EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not initialize as Station. Rebooting in 3 seconds");
        vTaskDelay(3000 / portTICK_RATE_MS);
        esp_restart();
    }
    init_tasks();
}
