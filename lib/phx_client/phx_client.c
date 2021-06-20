#include "esp_websocket_client.h"
#include "phx_client.h"
#include "cJSON.h"
#include "esp_log.h"

#define TAG "PHX_CLIENT"

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
        break;
    case WEBSOCKET_EVENT_DATA:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
        ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
        ESP_LOGW(TAG, "Received=%.*s", data->data_len, (char *)data->data_ptr);
        ESP_LOGW(TAG, "Total payload length=%d, data_len=%d, current payload offset=%d\r\n", data->payload_len, data->data_len, data->payload_offset);
        break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
        break;
    }
}

esp_websocket_client_handle_t phx_connect(phx_transport_t transport, const char *url)
{
    if (transport == WEBSOCKETS)
    {
        esp_websocket_client_config_t websocket_cfg = {};
        websocket_cfg.uri = url;

        esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
        esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);

        esp_err_t ret = esp_websocket_client_start(client);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error starting websocket client!");
            esp_websocket_client_destroy(client);
            return NULL;
        }
        return client;
    }
    else
    {
        return NULL;
    }
}

void phx_join(esp_websocket_client_handle_t ws_client, char *topic, char *token)
{
    cJSON *root = cJSON_CreateObject();
    cJSON *payload = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "event", "phx_join");
    cJSON_AddStringToObject(root, "topic", topic);
    cJSON_AddNumberToObject(root, "ref", 0);
    cJSON_AddItemToObject(root, "payload", payload);
    char *data = cJSON_Print(root);

    ESP_LOGI(TAG, "Outgoing topic: %s, data:\n%s", topic, data);

    esp_websocket_client_send_text(ws_client, data, strlen(data), portMAX_DELAY);

    cJSON_Delete(root);
    cJSON_free(data);
}

void phx_publish(esp_websocket_client_handle_t ws_client, char *topic, cJSON *pub_json)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "event", "new_msg");
    cJSON_AddStringToObject(root, "topic", topic);
    cJSON_AddNumberToObject(root, "ref", 0);
    cJSON_AddItemToObject(root, "payload", pub_json);
    char *data = cJSON_Print(root);

    ESP_LOGI(TAG, "Outgoing topic: %s, data:\n%s", topic, data);

    esp_websocket_client_send_text(ws_client, data, strlen(data), portMAX_DELAY);

    cJSON_Delete(pub_json);
    cJSON_free(data);
}