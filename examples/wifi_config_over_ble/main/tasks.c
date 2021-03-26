#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "common.h"
#include "tasks.h"
#include "system_stats.h"
#include "wifi_station.h"
#include "wifi_config_ble.h"

#define TAG "TASKS"

#define REPORT_DELAY 20000
#define STATS_TICKS pdMS_TO_TICKS(1000)

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID       CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS       CONFIG_ESP_WIFI_PASSWORD

TaskHandle_t *system_stats_task_handle = NULL;

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
        ESP_LOGI(TAG, "Free heap left  = %d", system_get_free_heap());

        vTaskDelay(REPORT_DELAY / portTICK_PERIOD_MS);
    }
}

void initialize_tasks()
{
    xTaskCreate(&system_stats_task, "system_stats_task", MEDIUM_STACK, NULL, LOW_PRIORITY, system_stats_task_handle);
    init_ble();
    enable_ble();
}