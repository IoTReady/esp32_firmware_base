#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "cJSON.h"

#include "common.h"
#include "phx_client.h"

#define TAG "TASKS"

#define WEBSOCKET_URI    CONFIG_PHOENIX_WEBSOCKET_URL

void phx_client_task()
{
    char* topic = malloc(20);
    sprintf(topic, "device:%s", get_device_id());

    esp_websocket_client_handle_t client = phx_connect(WEBSOCKETS, WEBSOCKET_URI);
    if (client == NULL)
    {
        ESP_LOGE("Failed to connect to %s", WEBSOCKET_URI);
        vTaskDelete(NULL);
    }

    ESP_ERROR_CHECK(phx_join(client, topic, NULL));

    int count = 0;
    while(1)
    {
        count++;

        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, "count", count);

        ESP_ERROR_CHECK(phx_publish(client, topic, root));
        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}

void init_tasks()
{
    xTaskCreate(&phx_client_task, "phx_client_task", LARGE_STACK, NULL, HIGH_PRIORITY, NULL);
}

