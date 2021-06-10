idf_component_register(SRCS ./src/wifi_prov.c
                       INCLUDE_DIRS include
                       REQUIRES "nvs_flash" "wifi_provisioning" "common"
                       PRIV_REQUIRES )

