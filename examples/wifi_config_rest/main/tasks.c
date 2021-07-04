#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "string.h"
#include <sys/param.h>

#include "esp_log.h"
#include "common.h"
// #include "gpio_interface.h"
// #include "queues.h"
#include "tasks.h"
#include "system_stats.h"
#include "wifi_softap.h"
#include "wifi_station.h"
#include "nvs_flash.h"


#define TAG "TASKS"



void initialize_tasks()
{
    // xTaskCreate(&softap_task, "softap_task", LARGE_STACK, NULL, LOW_PRIORITY, softap_task_handle);
}

