#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include <esp_http_server.h>


#define TAG "WEBSOCKET_INTERFACE"

/*
 * This handler echos back the received ws data
 * and triggers an async send if certain message received
 */
esp_err_t echo_handler(httpd_req_t *req)
{
    uint8_t buf[128] = { 0 };
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = buf;
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 128);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
        return ret;
    }
    ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
    ESP_LOGI(TAG, "Packet type: %d", ws_pkt.type);
    if (ws_pkt.type == HTTPD_WS_TYPE_TEXT &&
        strcmp((char*)ws_pkt.payload,"Trigger async") == 0) {
        return trigger_async_send(req->handle, req);
    }

    ret = httpd_ws_send_frame(req, &ws_pkt);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
    }
    return ret;
}

// static const httpd_uri_t ws[2] = {
//                                     {
//                                         .uri        = "/ws1",
//                                         .method     = HTTP_GET,
//                                         .handler    = echo_handler,
//                                         .user_ctx   = NULL,
//                                         .is_websocket = true
//                                     }
//                                     {
//                                         .uri        = "/ws2",
//                                         .method     = HTTP_GET,
//                                         .handler    = echo_handler,
//                                         .user_ctx   = NULL,
//                                         .is_websocket = true
//                                     }
//                                 };

httpd_handle_t websocket_start_server(httpd_uri_t* ws, size_t len)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Registering the ws handler
        ESP_LOGI(TAG, "Registering URI handlers");
        for (int i = 0; i < len; i++)
            httpd_register_uri_handler(server, &ws[i]);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void websocket_stop_server(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

void default_disconnect_handler(void* arg, esp_event_base_t event_base, 
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

void default_connect_handler(void* arg, esp_event_base_t event_base, 
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}