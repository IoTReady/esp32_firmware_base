#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tasks.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "wifi_station.h"

#define TAG "main"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD

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
