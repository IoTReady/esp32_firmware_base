#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"


#define TAG "SOFTAP_INTERFACE"

esp_netif_t *ap_netif;

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

// Setup and initialize WiFi in softAP mode with the parameters provided as arguments.
esp_err_t wifi_init_softap(const char *wifi_ssid, const char *wifi_password, int channel, int max_connections)
{
    esp_err_t err;

    // Initialize the underlying TCP/IP stack
    err = esp_netif_init();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not initialize NETIF!");
        return err;
    }

    // Create default event loop
    err = esp_event_loop_create_default();
    if (err == ESP_ERR_NO_MEM)
    {
        ESP_LOGE(TAG, "Not enough memory to create default event loop!");
        return err;   
    }
    else if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not create default event loop!");
        return err;
    }

    // Create default WIFI AP. In case of any init error this API aborts.
    ap_netif = esp_netif_create_default_wifi_ap();

    // Store default WiFi Init configurations
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    // Init WiFi Alloc resource
    err = esp_wifi_init(&cfg);
    if (err == ESP_ERR_NO_MEM)
    {
        ESP_LOGE(TAG, "Not enough memory to initialize WiFi");
        return err;
    }
    else if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not initialize WiFi!");
        return err;
    }

    // Register the event handler to the system event loop
    err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    if (err == ESP_ERR_NO_MEM)
    {
        ESP_LOGE(TAG, "Not enough memory to allocate memory for the handler!");
        return err;
    }
    else if (err == ESP_ERR_INVALID_ARG)
    {
        ESP_LOGE(TAG, "Could not register the event handler to the system event loop! Invalid combination of event base and event id");
        return err;
    }
    else if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not register the event handler to the system event loop!");
        return err;
    }

    // Soft-AP configuration settings
    wifi_config_t wifi_config = {
        .ap = {
            .channel = channel,
            .max_connection = max_connections,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    strcpy((char*)wifi_config.ap.ssid, wifi_ssid);
    strcpy((char*)wifi_config.ap.password, wifi_password);
    if (strlen(wifi_password) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    // Set the WiFi operating mode.
    err = esp_wifi_set_mode(WIFI_MODE_AP);
    if (err == ESP_ERR_WIFI_NOT_INIT)
    {
        ESP_LOGE(TAG, "WiFi is not initialized by esp_wifi_init! Could not set WiFi operating mode!");
        return err;
    }
    else if(err == ESP_ERR_INVALID_ARG)
    {
        ESP_LOGE(TAG, "Invalid argument! Could not set WiFi operating mode!");
        return err;
    }
    else if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not set WiFi operating mode!");
        return err;
    }

    // Set the configuration of the ESP32 AP.
    err = esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    if (err == ESP_ERR_WIFI_NOT_INIT)
    {
        ESP_LOGE(TAG, "WiFi is not initialized by esp_wifi_init! Could not set AP configuration!");
        return err;
    }
    else if(err == ESP_ERR_INVALID_ARG)
    {
        ESP_LOGE(TAG, "Invalid argument! Could not set AP configuration!");
        return err;
    }
    else if(err == ESP_ERR_WIFI_IF)
    {
        ESP_LOGE(TAG, "Invalid interface! Could not set AP configuration!");
        return err;
    }
    else if(err == ESP_ERR_WIFI_MODE)
    {
        ESP_LOGE(TAG, "Invalid mode! Could not set AP configuration!");
        return err;
    }
    else if(err == ESP_ERR_WIFI_PASSWORD)
    {
        ESP_LOGE(TAG, "Invalid password! Could not set AP configuration!");
        return err;
    }
    else if(err == ESP_ERR_WIFI_NVS)
    {
        ESP_LOGE(TAG, "WiFi internal NVS error! Could not set AP configuration!");
        return err;
    }
    else if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not set WiFi operating mode!");
        return err;
    }

    // Start WiFi according to current configuration
    err = esp_wifi_start();
    if (err == ESP_ERR_WIFI_NOT_INIT)
    {
        ESP_LOGE(TAG, "WiFi is not initialized by esp_wifi_init! Could not start WiFi!");
        return err;
    }
    else if(err == ESP_ERR_INVALID_ARG)
    {
        ESP_LOGE(TAG, "Invalid argument! Could not start WiFi!");
        return err;
    }
    else if(err == ESP_ERR_NO_MEM)
    {
        ESP_LOGE(TAG, "Out of memory! Could not start WiFi!");
        return err;
    }
    else if(err == ESP_ERR_WIFI_CONN)
    {
        ESP_LOGE(TAG, "WiFi internal error, station or soft-AP control block wrong! Could not start WiFi!");
        return err;
    }
    else if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not set WiFi operating mode!");
        return err;
    }

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             wifi_ssid, wifi_password, channel);
    
    return err;
}

esp_err_t wifi_deinit_softap()
{
    esp_err_t err;

    // Unregister the event handler to the system event loop
    err = esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler);
    if (err == ESP_ERR_INVALID_ARG)
    {
        ESP_LOGE(TAG, "Could not unregister the event handler! Invalid combination of event base and event id!");
        return err;
    }
    else if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not register the event handler to the system event loop!");
        return err;
    }

    // Stop WiFi
    err = esp_wifi_stop();
    if (err == ESP_ERR_WIFI_NOT_INIT) {
        ESP_LOGW(TAG, "WiFi is not initialized by esp_wifi_init!");
        return err;
    }

    // Deinit WiFi. Free all resource allocated in esp_wifi_init and stop WiFi task.
    err = esp_wifi_deinit();
    if (err == ESP_ERR_WIFI_NOT_INIT) {
        ESP_LOGW(TAG, "WiFi is not initialized by esp_wifi_init!");
        return err;
    }

    // Clear default wifi event handlers for supplied network interface.
    err = esp_wifi_clear_default_wifi_driver_and_handlers(ap_netif);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Could not clear default wifi event handlers for supplied network interface!");
        return err;
    }
    
    // Destroys the esp_netif object. 
    esp_netif_destroy(ap_netif);
    ap_netif = NULL;

    return err;
}