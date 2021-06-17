idf_component_register(SRCS "main.c"
                    "tasks.c"
                    INCLUDE_DIRS "."
                    REQUIRES "common" "phx_client" "wifi_station" "esp_websocket_client" "json")