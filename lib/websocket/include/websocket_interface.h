#ifndef __WEBSOCKET_INTERFACE_H__
#define __WEBSOCKET_INTERFACE_H__ 1

/**
 * @brief  Start a websocket server and register required URIs 
 * 
 *
 * @param[in] ws        Array of httpd_uri_t structures to be registered.
 * @param[in] len       Size of array
 * 
 * 
 * @return
 *    - httpd_handle_t:     Handle of server
 *    - NULL:               Error starting server
 * 
 */
httpd_handle_t websocket_start_server(httpd_uri_t* ws, size_t len);

/**
 * @brief  Stop a websocket server
 * 
 *
 * @param[in] server       httpd_handle_t: Handle of the server to stop
 * 
 */
void websocket_stop_server(httpd_handle_t server);

#endif // __WEBSOCKET_INTERFACE_H__
