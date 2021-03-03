#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "common.h"
#include "tasks.h"
#include "system_stats.h"


#define TAG "TASKS"
#define REPORT_DELAY 4000
#define STATS_TICKS pdMS_TO_TICKS(1000)

TaskHandle_t *task1_handle, *task2_handle, *task3_handle, *system_stats_task_handle, *free_heap_monitor_task_handle = NULL;

void task1(void *pvParameter);
void task2(void *pvParameter);
void task3(void *pvParameter);
void system_stats_task(void *pvParameter);

void task1(void *pvParameter)
{
    int task1_count = 0;
    while (1) {
        task1_count++;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task2(void *pvParameter)
{
    int task2_count = 0;
    while (1) {
        task2_count++;
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

void task3(void *pvParameter)
{
    int task3_count = 0;
    while (1) {
        task3_count++;
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
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

void free_heap_monitor_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting free heap monitor task");
    while (1)
    {
        ESP_LOGI(TAG, "Free heap left  = %d", system_get_free_heap());
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

void initialize_tasks()
{
    xTaskCreate(&task1, "task1", MEDIUM_STACK, NULL, LOW_PRIORITY, task1_handle);
    xTaskCreate(&task2, "task2", MEDIUM_STACK, NULL, LOW_PRIORITY, task2_handle);
    xTaskCreate(&task3, "task3", MEDIUM_STACK, NULL, LOW_PRIORITY, task3_handle);

    // xTaskCreate(&free_heap_monitor_task, "free_heap_monitor_task", MEDIUM_STACK, NULL, LOW_PRIORITY, free_heap_monitor_task_handle);
	xTaskCreatePinnedToCore(&system_stats_task, "system_stats_task", MEDIUM_STACK, NULL, HIGH_PRIORITY, NULL, tskNO_AFFINITY);
    
}