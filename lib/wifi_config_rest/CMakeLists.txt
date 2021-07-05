idf_component_register(SRCS "wifi_config_rest.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_http_server json common wifi_softap wifi_station utilities)