idf_component_register(SRCS ./src/aws_control.c
                       INCLUDE_DIRS include
                       REQUIRES common  aws_iot json spiffs
                       PRIV_REQUIRES )

