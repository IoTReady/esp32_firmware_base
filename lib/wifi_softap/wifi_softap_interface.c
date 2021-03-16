#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"


#define TAG "SOFTAP_INTERFACE"
#define DEFAULT_ESP_WIFI_CHANNEL 1
#define DEFAULT_ESP_MAX_STA_CONN 4

esp_netif_t *ap_netif;
esp_netif_ip_info_t ip;

// Soft-AP configuration settings
wifi_config_t wifi_config = {
    .ap = {
        .channel = DEFAULT_ESP_WIFI_CHANNEL,
        .max_connection = DEFAULT_ESP_MAX_STA_CONN,
        .authmode = WIFI_AUTH_WPA_WPA2_PSK
    },
};


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

esp_err_t wifi_set_credentials(char* ssid, char* password)
{
    strcpy((char*)wifi_config.ap.ssid, ssid);
    strcpy((char*)wifi_config.ap.password, password);
    if (strlen(password) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    // Set the configuration of the ESP32 AP.
    esp_err_t err = esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
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
    return err;
}

void wifi_restart()
{
    esp_wifi_stop();
    vTaskDelay(200 / portTICK_PERIOD_MS);
    esp_wifi_start();
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

    esp_netif_dhcp_status_t dhcp_status;
    if(esp_netif_dhcps_get_status(ap_netif, &dhcp_status) != ESP_NETIF_DHCP_STOPPED)
        esp_netif_dhcps_stop(ap_netif);

    const esp_netif_ip_info_t new_ip_info = {
        .ip = { .addr = ESP_IP4TOADDR( 192, 168, 0, 1) },
        .gw = { .addr = ESP_IP4TOADDR( 192, 168, 0, 1) },
        .netmask = { .addr = ESP_IP4TOADDR( 255, 255, 255, 0) },
    };
    err = esp_netif_set_ip_info(ap_netif, &new_ip_info);
    esp_netif_dhcps_start(ap_netif);

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
    
    // Soft-AP configuration settings
    wifi_config.ap.channel = channel;
    wifi_config.ap.max_connection = max_connections;

    wifi_set_credentials(wifi_ssid, wifi_password);

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

    esp_netif_get_ip_info(ap_netif, &ip);
    ESP_LOGI(TAG, "- IPv4 address: " IPSTR, IP2STR(&ip.ip));

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

    err = esp_event_loop_delete_default();
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