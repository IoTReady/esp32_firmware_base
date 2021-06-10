#ifndef __AWS_CONTROL_H__
#define __AWS_CONTROL_H__ 1

#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

#define AWS_IOT_HOST CONFIG_AWS_IOT_MQTT_HOST
#define AWS_IOT_PORT CONFIG_AWS_IOT_MQTT_PORT

extern char aws_root_ca_pem_start[2048];
extern char certificate_pem_crt_start[2048];
extern char private_pem_key_start[2048];

void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
                                    IoT_Publish_Message_Params *params, void *pData);
void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data);
IoT_Error_t init_aws_iot(AWS_IoT_Client mqtt_client, IoT_Client_Init_Params mqttInitParams);
void aws_iot_task(void *param);
void readFileToString(char *fname, char *buf);
int init_spiffs(void);

#endif // __AWS_CONTROL_H__

