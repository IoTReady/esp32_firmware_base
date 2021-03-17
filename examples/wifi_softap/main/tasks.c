#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "common.h"
// #include "gpio_interface.h"
// #include "queues.h"
#include "tasks.h"
#include "system_stats.h"
#include "wifi_softap_interface.h"
#include "nvs_flash.h"


#define TAG "TASKS"

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want. However, if you want to use a custom SSID, set CONFIG_USE_CUSTOM_SSID.

*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN       CONFIG_ESP_MAX_STA_CONN

#define REPORT_DELAY 20000
#define STATS_TICKS pdMS_TO_TICKS(1000)

TaskHandle_t *system_stats_task_handle, *softap_task_handle = NULL;

void softap_task(void *pvParameter) {

    char *softap_ssid[17];
#if (CONFIG_USE_CUSTOM_SSID)
    sprintf(softap_ssid, EXAMPLE_ESP_WIFI_SSID);
#else /* #if (CONFIG_USE_CUSTOM_SSID) */
    char *device_id = get_device_id();
    sprintf(softap_ssid, "IoT-%s", device_id);     
#endif /* #if (CONFIG_USE_CUSTOM_SSID) */

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }

    // Initialize softAP
    wifi_init_softap(softap_ssid, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL, EXAMPLE_MAX_STA_CONN);
    vTaskDelay(30000 / portTICK_PERIOD_MS);

    // De-Initialize softAP
    wifi_deinit_softap();

    // Delete task since it is a one-time operation
    vTaskDelete(NULL);

}


void system_stats_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting system stats monitor task");
    while (1) {
        ESP_LOGI(TAG, "Getting real time stats over %d ticks", STATS_TICKS);
        esp_err_t res = system_print_real_time_stats(STATS_TICKS);
        if (res == ESP_OK) {
            ESP_LOGI(TAG, "Real time stats obtained\n");
        } else {
            ESP_LOGE(TAG, "Error getting real time stats: %d\n", res);
        }
        vTaskDelay(REPORT_DELAY / portTICK_PERIOD_MS);
    }
}


void initialize_tasks()
{
    xTaskCreate(&system_stats_task, "system_stats_task", MEDIUM_STACK, NULL, LOW_PRIORITY, system_stats_task_handle);
    xTaskCreate(&softap_task, "softap_task", LARGE_STACK, NULL, LOW_PRIORITY, softap_task_handle);
}

