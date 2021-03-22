idf_component_register(SRCS "local_ota.c"
                    INCLUDE_DIRS "include"
                    REQUIRES app_update common wifi_station)