#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_http_server.h"
#include "nvs_flash.h"
#include "cJSON.h"
#include "string.h"
#include <sys/param.h>

#include "esp_log.h"
#include "common.h"
#include "utilities.h"
#include "wifi_config_rest.h"
#include "wifi_softap.h"


#define TAG "WIFI_CONFIG_REST"

// #define CONFIG_ESP_WIFI_SSID        "iotready"
#define CONFIG_ESP_WIFI_PASSWORD    "getiotready"
#define CONFIG_ESP_WIFI_CHANNEL     1
#define CONFIG_ESP_MAX_STA_CONN     4

#define STORAGE_NAMESPACE       "storage"
#define WIFI_CONFIG_STORE       "wifi_store"
#define WIFI_CONFIG_FLAG_STORE  "wifi_flag"

#define WIFI_CONFIG_URI     "/api/wifi/credential"

#define REPORT_DELAY    20000
#define STATS_TICKS     pdMS_TO_TICKS(1000)
#define MAX_BUFSIZE     10240


static esp_err_t wifi_config_write_nvs(const char* ssid, const char* password)
{
    nvs_handle nvsHandle;
    esp_err_t ret;
    
    wifi_sta_init_struct_t wifi_params;
    memset(wifi_params.network_ssid, 0, sizeof(wifi_params.network_ssid));
    for (int i=0; i < strlen(ssid); i++) 
    {
        wifi_params.network_ssid[i] = ssid[i];
    }
    memset(wifi_params.network_password, 0, sizeof(wifi_params.network_password));
    for (int i=0; i < strlen(password); i++) 
    {
        wifi_params.network_password[i] = password[i];
    }

    ret = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &nvsHandle);
    if (ret != ESP_OK) 
    {
        return error_print_and_return(TAG, ret);
    }

    size_t required_size = sizeof(wifi_sta_init_struct_t);
    nvs_set_blob(nvsHandle, WIFI_CONFIG_STORE, &wifi_params, required_size);
    ret = nvs_commit(nvsHandle);
    nvs_close(nvsHandle);
    
    return ret;
}

wifi_sta_init_struct_t wifi_config_read_nvs()
{
    
    static nvs_handle nvsHandle;
    static esp_err_t err;
    wifi_sta_init_struct_t wifi_params;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &nvsHandle);
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }
    else 
    {
        // Read the size of memory space required for struct
        size_t required_size = 0;  // value will default to 0, if not set yet in NVS
        err = nvs_get_blob(nvsHandle, WIFI_CONFIG_STORE, NULL, &required_size);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) 
        {
            ESP_LOGE(TAG, "Error (%s) while reading WiFi params struct size.", esp_err_to_name(err));
        }

        else if (required_size != sizeof(wifi_sta_init_struct_t))
        {
            ESP_LOGE(TAG, "WiFi params not found. Perhaps not stored yet?");
            err = ESP_FAIL;
        }
        else
        {
            // Read previously saved blob if available
            err = nvs_get_blob(nvsHandle, WIFI_CONFIG_STORE, &wifi_params, &required_size);
            if (err != ESP_OK) 
            {
                ESP_LOGE(TAG, "Error (%s) while reading WiFi params.", esp_err_to_name(err));
            }
            ESP_LOGI(TAG, "WiFi READ SUCCESS!\nSSID:%s, PASS:%s", wifi_params.network_ssid, wifi_params.network_password);
        }
    }
    nvs_close(nvsHandle);
    
    // return err;
    return wifi_params;
}

static esp_err_t wifi_config_write_flag_nvs()
{
    nvs_handle nvsHandle;
    esp_err_t ret;

    nvs_open(WIFI_CONFIG_FLAG_STORE, NVS_READWRITE, &nvsHandle);
    size_t required_size = sizeof(bool);
    nvs_set_blob(nvsHandle, WIFI_CONFIG_FLAG_STORE, &wifi_config_flag, required_size);
    ret = nvs_commit(nvsHandle);
    nvs_close(nvsHandle);
    
    return ret;
}

bool wifi_config_read_flag_nvs()
{
    
    static nvs_handle nvsHandle;
    static esp_err_t err;
    bool flag = false;

    err = nvs_open(WIFI_CONFIG_FLAG_STORE, NVS_READONLY, &nvsHandle);
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }
    else 
    {
        // Read the size of memory space required for struct
        size_t required_size = 0;  // value will default to 0, if not set yet in NVS
        err = nvs_get_blob(nvsHandle, WIFI_CONFIG_FLAG_STORE, NULL, &required_size);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) 
        {
            ESP_LOGE(TAG, "Error (%s) while reading WiFi params flag.", esp_err_to_name(err));
        }

        else if (required_size != sizeof(bool))
        {
            ESP_LOGE(TAG, "Flag not found. Perhaps not stored yet?");
            err = ESP_FAIL;
        }
        else
        {
            // Read previously saved blob if available
            err = nvs_get_blob(nvsHandle, WIFI_CONFIG_FLAG_STORE, &flag, &required_size);
            if (err != ESP_OK) 
            {
                ESP_LOGE(TAG, "Error (%s) while reading WiFi params flag.", esp_err_to_name(err));
            }
        }
    }
    nvs_close(nvsHandle);
    
    // return err;
    return flag;
}

/* An HTTP POST handler */
static esp_err_t wifi_credential_post_handler(httpd_req_t *req)
{
    char buf[100];
    // char *buf = (char*) malloc(100 * sizeof(char));
    int ret, total_len = req->content_len, remaining = req->content_len;
    if (total_len >= MAX_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Content too long");
        return ESP_FAIL;
    }
    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
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

    ESP_LOGI(TAG, "Setting WiFi credentials: SSID = %s, password = %s", ssid, password);
    
    esp_err_t err = wifi_config_write_nvs(ssid, password);
    if (err != ESP_OK)
    {
        return error_print_and_return(TAG, err);
    }

    wifi_config_flag = true;
    err = wifi_config_write_flag_nvs();
    if (err != ESP_OK)
    {
        return error_print_and_return(TAG, err);
    }

    // Send response 200 OK by default
    httpd_resp_sendstr(req, "Post WiFi credentials successfully");

    // Delay necessary for proper despatch of response
    vTaskDelay(100 / portTICK_PERIOD_MS);

    httpd_unregister_uri(req->handle, WIFI_CONFIG_URI);

    wifi_deinit_softap();
    wifi_init_station(ssid, password);

    cJSON_Delete(root);
    return ESP_OK;
}

static esp_err_t start_rest_server()
{
    esp_err_t ret;

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI(TAG, "Starting HTTP Server on port: '%d'", config.server_port);
    ret = httpd_start(&server, &config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP SERVER");
        return ret;
    }

    ESP_LOGI(TAG, "Registering URI handlers");

    /* URI handler for wifi configuration */
    httpd_uri_t wifi_credential_post_uri = {
        .uri = WIFI_CONFIG_URI,
        .method = HTTP_POST,
        .handler = wifi_credential_post_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &wifi_credential_post_uri);

    return ret;
}

bool wifi_config_is_provisioned()
{
    return wifi_config_flag;
}

esp_err_t wifi_config_start() 
{
    const char *device_id = get_device_id();
    char softap_ssid[22];
    sprintf(softap_ssid, "IoTReady %s", device_id);
    
    // Initialize softAP
    esp_err_t ret = wifi_init_softap(softap_ssid, CONFIG_ESP_WIFI_PASSWORD, CONFIG_ESP_WIFI_CHANNEL, CONFIG_ESP_MAX_STA_CONN);
    if (ret != ESP_OK){
        return error_print_and_return(TAG, ret);
    }

    ret = start_rest_server();
    if (ret != ESP_OK){
        return error_print_and_return(TAG, ret);
    }

    return ret;
}

