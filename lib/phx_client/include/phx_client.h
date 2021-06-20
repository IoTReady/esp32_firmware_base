#ifndef __PHX_CLIENT__
#define __PHX_CLIENT__ 1

#include "esp_websocket_client.h"
#include "cJSON.h"

/**
 * Enumerator for holding types of transport. Only WEBSOCKETS currently supported.
 */
typedef enum {
	WEBSOCKETS = 0,
	LONGPOLLING = 1
} phx_transport_t;

/**
 * @brief   This function will open a connection based on the selected transport.
 *
 * @param[in]  phx_transport_t  transport type
 * @param[in]  url              URL
 *
 * @return
 *     - `esp_websocket_client_handle_t`
 *     - NULL if any errors 
 * 
 */
esp_websocket_client_handle_t phx_connect(phx_transport_t transport, const char *url);

/**
 * @brief   This function will join a channel/topic
 *
 * @param[in]  ws_client  Connection client handle returned by phx_connect
 * @param[in]  topic      Topic to join
 * @param[in]  token      Token for authentication 
 *
 * @return  void
 * 
 */
void phx_join(esp_websocket_client_handle_t ws_client, char *topic, char *token);

/**
 * @brief   This function will publish to a channel
 *
 * @param[in]  ws_client  Connection client handle returned by phx_connect()
 * @param[in]  topic      Topic to publish to
 * @param[in]  pub_json   cJSON object containing data to publish 
 *
 * @return  void 
 * 
 */
void phx_publish(esp_websocket_client_handle_t ws_client, char *topic, cJSON *pub_json);

#endif // __PHX_CLIENT__