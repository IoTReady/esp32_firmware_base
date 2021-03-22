#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "utilities.h"

#include "lwip/err.h"
#include "lwip/sys.h"


#define TAG "WIFI_STATION_INTERFACE"

#define CONFIG_ESP_MAXIMUM_RETRY    5

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static int s_retry_num = 0;

esp_netif_t *ap_netif;
esp_netif_ip_info_t ip;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < CONFIG_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

EventGroupHandle_t wifi_sta_get_event_group()
{
    return s_wifi_event_group;
}

esp_err_t wifi_init_station(const char *wifi_ssid, const char *wifi_password)
{
    esp_err_t err;

    s_wifi_event_group = xEventGroupCreate();

    // Initialize the underlying TCP/IP stack
    err = esp_netif_init();
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }
    // Create default event loop
    err = esp_event_loop_create_default();
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }

    // Create default WIFI AP. In case of any init error this API aborts.
    ap_netif = esp_netif_create_default_wifi_sta();

    // Store default WiFi Init configurations
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    // Init WiFi Alloc resource
    err = esp_wifi_init(&cfg);
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }

    // Register the WiFi event handler to the system event loop
    err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL);
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }

    // Register the IP event handler to the system event loop
    err = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL);
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }

    // Station configuration settings
    wifi_config_t wifi_config = {
        .sta = {
            // .ssid = wifi_ssid,
            // .password = wifi_password,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	        .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            /** Configuration structure for Protected Management Frame */
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    strcpy((char*)wifi_config.ap.ssid, wifi_ssid);
    strcpy((char*)wifi_config.ap.password, wifi_password);

    // Set the WiFi operating mode to Station.
    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }

    // Set the configuration of the ESP32 Station.
    err = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }

    // Start WiFi according to current configuration
    err = esp_wifi_start();
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }

    ESP_LOGI(TAG, "wifi_init_station finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to AP SSID:%s password:%s",
                 wifi_ssid, wifi_password);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(TAG, "Failed to connect to SSID:%s, password:%s",
                 wifi_ssid, wifi_password);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    err = esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler);
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }

    err = esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler);
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }

    vEventGroupDelete(s_wifi_event_group);

    esp_netif_get_ip_info(ap_netif, &ip);
    ESP_LOGI(TAG, "- IPv4 address: " IPSTR, IP2STR(&ip.ip));
    
    return err;
}

esp_err_t wifi_deinit_station()
{
    esp_err_t err;

    err = esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler);
    if (err == ESP_ERR_INVALID_ARG)
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }

    err = esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler);
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }

    // Stop WiFi
    err = esp_wifi_stop();
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }

    // Deinit WiFi. Free all resource allocated in esp_wifi_init and stop WiFi task.
    err = esp_wifi_deinit();
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }

    // Clear default wifi event handlers for supplied network interface.
    err = esp_wifi_clear_default_wifi_driver_and_handlers(ap_netif);
    if (err  != ESP_OK){
        return error_print_and_return(TAG, err);
    }
   
    // Destroys the esp_netif object. 
    esp_netif_destroy(ap_netif);
    ap_netif = NULL;

    // return error_print_and_return(TAG, err);
    return err;
}