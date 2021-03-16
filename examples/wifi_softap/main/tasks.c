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
#include "wifi_softap_interface.h"
#include "wifi_station_interface.h"
#include "nvs_flash.h"


#define TAG "TASKS"

// #define CONFIG_ESP_WIFI_SSID        "iotready"
#define CONFIG_ESP_WIFI_PASSWORD    "getiotready"
#define CONFIG_ESP_WIFI_CHANNEL     1
#define CONFIG_ESP_MAX_STA_CONN     4

#define REPORT_DELAY 20000
#define STATS_TICKS pdMS_TO_TICKS(1000)

TaskHandle_t *softap_task_handle = NULL;

/* Simple handler for getting system handler */
static esp_err_t system_info_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    cJSON_AddStringToObject(root, "version", IDF_VER);
    cJSON_AddNumberToObject(root, "cores", chip_info.cores);
    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    return ESP_OK;
}

/* An HTTP POST handler */
static esp_err_t wifi_credential_post_handler(httpd_req_t *req)
{
    char buf[100];
    // char *buf = (char*) malloc(100 * sizeof(char));
    int ret, total_len = req->content_len, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    const char *ssid = cJSON_GetObjectItem(root, "ssid")->valuestring;
    const char *password = cJSON_GetObjectItem(root, "password")->valuestring;

    ESP_LOGI(TAG, "WiFi credentials received: SSID = %s, password = %s", ssid, password);

    // End response
    httpd_resp_sendstr(req, "Post WiFi credentials successfully");

    // Delay necessary for proper despatch of response
    vTaskDelay(100 / portTICK_PERIOD_MS);

    wifi_deinit_softap();
    wifi_init_station(ssid, password);

    cJSON_Delete(root);
    return ESP_OK;
}

static esp_err_t start_rest_server()
{
    esp_err_t err;

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI(TAG, "Starting HTTP Server on port: '%d'", config.server_port);
    err = httpd_start(&server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP SERVER");
        return err;
    }

    ESP_LOGI(TAG, "Registering URI handlers");
    /* URI handler for fetching system info */
    httpd_uri_t system_info_get_uri = {
        .uri = "/api/v1/system/info",
        .method = HTTP_GET,
        .handler = system_info_get_handler,
        .user_ctx = NULL
    };
    err = httpd_register_uri_handler(server, &system_info_get_uri);

        /* URI handler for light brightness control */
    httpd_uri_t wifi_credential_post_uri = {
        .uri = "/api/wifi/credential",
        .method = HTTP_POST,
        .handler = wifi_credential_post_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &wifi_credential_post_uri);

    return err;
}

void softap_task(void *pvParameter) {

    char *device_id = get_device_id();
    char *softap_ssid[17];
    sprintf(softap_ssid, "IoT-%s", device_id);

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    
    // Initialize softAP
    wifi_init_softap(softap_ssid, CONFIG_ESP_WIFI_PASSWORD, CONFIG_ESP_WIFI_CHANNEL, CONFIG_ESP_MAX_STA_CONN);

    ret = start_rest_server();

    // Delete task since it is a one-time operation
    vTaskDelete(NULL);

}


void initialize_tasks()
{
    xTaskCreate(&softap_task, "softap_task", LARGE_STACK, NULL, LOW_PRIORITY, softap_task_handle);
}

