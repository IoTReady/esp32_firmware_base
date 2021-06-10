#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "common.h"
#include "stdio.h"
#include "aws_control.h"
#include "wifi_station.h"

#define TAG "TASKS"

TaskHandle_t *system_stats_task_handle, *station_task_handle = NULL;

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD

void station_task(void *pvParameter) {

    // Initialize softAP
    wifi_init_station(EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    vTaskDelay(30000 / portTICK_PERIOD_MS);

    // De-Initialize softAP
    wifi_deinit_station();

    // Delete task since it is a one-time operation
    vTaskDelete(NULL);
}

void init_tasks()
{
    int spiffs_ok = init_spiffs();
    if (spiffs_ok == 0)
    {
        // read aws certificates into memory
        readFileToString("/spiffs/certificate.pem.crt", certificate_pem_crt_start);
        readFileToString("/spiffs/private.pem.key", private_pem_key_start);
        readFileToString("/spiffs/aws-root-ca.pem", aws_root_ca_pem_start);
        xTaskCreate(&aws_iot_task, "aws_iot_task", LARGER_STACK, NULL, LOW_PRIORITY, NULL);
    }
}

