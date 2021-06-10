// START #INCLUDES

#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_event.h"
#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_shadow_interface.h"
#include "aws_control.h"
#include "driver/gpio.h"
#include "common.h"
#include "cJSON.h"
#include <sys/time.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include <string.h>

// END #INCLUDES

// START #DEFINES

#define TAG "AWS_CONTROL"
#define MAX_LENGTH_OF_UPDATE_JSON_BUFFER 600

#define LIGHT_GPIO GPIO_NUM_5
#define SHADOW_UPDATE_INTERVAL 5 //seconds

// END #DEFINES

// START MODULE GLOBAL VARIABLES

static bool shadowUpdateInProgress;

char aws_root_ca_pem_start[2048] = "0";
char certificate_pem_crt_start[2048] = "0";
char private_pem_key_start[2048] = "0";

// END MODULE GLOBAL VARIABLES

// START FUNCTION DECLARATIONS

void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data);

void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
                                    IoT_Publish_Message_Params *params, void *pData);

void ShadowUpdateStatusCallback(const char *pThingName, ShadowActions_t action, Shadow_Ack_Status_t status,
                                const char *pReceivedJsonDocument, void *pContextData);
void light_state_callback(const char *pJsonString, uint32_t JsonStringDataLen, jsonStruct_t *pContext);
void aws_iot_task(void *param);
// END FUNCTION DECLARATIONS

// START FUNCTION DEFINITIONS

void set_light_state(int new_light_state)
{
    gpio_set_level(LIGHT_GPIO, new_light_state);
}

void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
                                    IoT_Publish_Message_Params *params, void *pData)
{
    ESP_LOGI(TAG, "%.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen, (char *)params->payload);
    cJSON *root = cJSON_Parse((char *)params->payload);
    if (root != NULL)
    {
        cJSON *device_state = cJSON_GetObjectItem(root,"device_state");
        if (device_state != NULL)
        {
            cJSON *light_state_json = cJSON_GetObjectItem(device_state,"light_state");
            if (light_state_json != NULL)
            {
                int new_light_state = light_state_json->valueint;
                ESP_LOGI(TAG, "Setting light mode in AWS topic");
                set_light_state(new_light_state);
            }
        }
    }
    // delete the json object to free up memory
    cJSON_Delete(root);
}

void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data)
{
    ESP_LOGW(TAG, "MQTT Disconnect");
    IoT_Error_t err = FAILURE;
    if(NULL == pClient) {
        return;
    }

    if(aws_iot_is_autoreconnect_enabled(pClient)) {
        ESP_LOGI(TAG, "Auto Reconnect is enabled, Reconnecting attempt will start now");
    } else {
        ESP_LOGW(TAG, "Auto Reconnect not enabled. Starting manual reconnect...");
        err = aws_iot_mqtt_attempt_reconnect(pClient);
        if(NETWORK_RECONNECTED == err) {
            ESP_LOGW(TAG, "Manual Reconnect Successful");
        } else {
            ESP_LOGW(TAG, "Manual Reconnect Failed - %d", err);
        }
    }
}

void ShadowUpdateStatusCallback(const char *pThingName, ShadowActions_t action, Shadow_Ack_Status_t status,
                                const char *pReceivedJsonDocument, void *pContextData) {
    IOT_UNUSED(pThingName);
    IOT_UNUSED(action);
    IOT_UNUSED(pReceivedJsonDocument);
    IOT_UNUSED(pContextData);

    shadowUpdateInProgress = false;

    if(SHADOW_ACK_TIMEOUT == status) {
        ESP_LOGE(TAG, "Update timed out");
    } else if(SHADOW_ACK_REJECTED == status) {
        ESP_LOGE(TAG, "Update rejected");
    } else if(SHADOW_ACK_ACCEPTED == status) {
        ESP_LOGI(TAG, "Update accepted");
    }
}

void light_state_callback(const char *pJsonString, uint32_t JsonStringDataLen, jsonStruct_t *pContext) {
    IOT_UNUSED(pJsonString);
    IOT_UNUSED(JsonStringDataLen);

    if(pContext != NULL) {
        // ESP_LOGI(TAG, "Delta - light mode changed to %d", *(int *) (pContext->pData));
        int new_light_state = *(int *) (pContext->pData);
        ESP_LOGI(TAG, "Setting light mode in AWS shadow");
        set_light_state(new_light_state);
        ESP_LOGI(TAG,"Changing light_state to %d", new_light_state);
    }
}

void aws_iot_task(void *param) {
    IoT_Error_t rc = FAILURE;

    char JsonDocumentBuffer[MAX_LENGTH_OF_UPDATE_JSON_BUFFER];
    size_t sizeOfJsonDocumentBuffer = sizeof(JsonDocumentBuffer) / sizeof(JsonDocumentBuffer[0]);

    int light_state = 0, software_version = 1, uptime = 0;
    const char *device_id = get_device_id();
    float system_temperature = 32.25;

    jsonStruct_t light_state_json;
    light_state_json.cb = light_state_callback;
    light_state_json.pKey = "light_state";
    light_state_json.pData = &light_state;
    light_state_json.type = SHADOW_JSON_INT8;

    jsonStruct_t uptime_json;
    uptime_json.cb = NULL;
    uptime_json.pKey = "uptime";
    uptime_json.pData = &uptime;
    uptime_json.type = SHADOW_JSON_INT32;

    jsonStruct_t system_temperature_json;
    system_temperature_json.cb = NULL;
    system_temperature_json.pKey = "system_temperature";
    system_temperature_json.pData = &system_temperature;
    system_temperature_json.type = SHADOW_JSON_FLOAT;

    jsonStruct_t software_version_json;
    software_version_json.cb = NULL;
    software_version_json.pKey = "software_version";
    software_version_json.pData = &software_version;
    software_version_json.type = SHADOW_JSON_INT8;

    jsonStruct_t device_id_json;
    device_id_json.cb = NULL;
    device_id_json.pKey = "device_id";
    device_id_json.pData = device_id;
    device_id_json.type = SHADOW_JSON_STRING;

    ESP_LOGI(TAG, "AWS IoT SDK Version %d.%d.%d-%s", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

    // initialize the mqtt client
    AWS_IoT_Client mqttClient;

    ShadowInitParameters_t sp = ShadowInitParametersDefault;
    sp.pHost = AWS_IOT_HOST;
    sp.port = AWS_IOT_PORT;
    sp.pClientCRT = (const char *)certificate_pem_crt_start;
    sp.pClientKey = (const char *)private_pem_key_start;
    sp.pRootCA = (const char *)aws_root_ca_pem_start;
    sp.enableAutoReconnect = false;
    sp.disconnectHandler = disconnect_callback_handler;
    
    ESP_LOGI(TAG, "Shadow Init");
    do {
        rc = aws_iot_shadow_init(&mqttClient, &sp);
        if(SUCCESS != rc) {
            ESP_LOGE(TAG, "aws_iot_shadow_init returned error %d", rc);
            vTaskDelay(2000 / portTICK_RATE_MS);
        }
    } while(SUCCESS != rc);
    

    ShadowConnectParameters_t scp = ShadowConnectParametersDefault;
    scp.pMyThingName = device_id;
    scp.pMqttClientId = device_id;
    scp.mqttClientIdLen = (uint16_t) strlen(device_id);

    ESP_LOGI(TAG, "Shadow Connect thing: %s", device_id);
    do {
        rc = aws_iot_shadow_connect(&mqttClient, &scp);
        if(SUCCESS != rc) {
            ESP_LOGE(TAG, "aws_iot_shadow_connect returned error %d", rc);
            vTaskDelay(2000 / portTICK_RATE_MS);
        }
    } while(SUCCESS != rc);
    

    rc = aws_iot_shadow_set_autoreconnect_status(&mqttClient, true);
    if(SUCCESS != rc) {
        ESP_LOGE(TAG, "Unable to set Auto Reconnect to true - %d", rc);
    }

    do {
        rc = aws_iot_shadow_register_delta(&mqttClient, &light_state_json);
        if(SUCCESS != rc) {
            ESP_LOGE(TAG, "Shadow Register Delta Error");
            vTaskDelay(2000 / portTICK_RATE_MS);
        }
    } while(SUCCESS != rc);

    // loop and publish a heartbeat
    while(1)
    {
        while(NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc) {
            rc = aws_iot_shadow_yield(&mqttClient, 200);
            if(NETWORK_ATTEMPTING_RECONNECT == rc || shadowUpdateInProgress) {
                rc = aws_iot_shadow_yield(&mqttClient, 1000);
                // If the client is attempting to reconnect, or already waiting on a shadow update,
                // we will skip the rest of the loop.
                continue;
            }
            uptime = esp_timer_get_time()/1000000;

            rc = aws_iot_shadow_init_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
            if(SUCCESS == rc) {
                
                rc = aws_iot_shadow_add_reported(JsonDocumentBuffer, sizeOfJsonDocumentBuffer, 5, &light_state_json, &device_id_json, &system_temperature_json, &uptime_json, &software_version_json);
                if(SUCCESS == rc) {
                    rc = aws_iot_finalize_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
                    if(SUCCESS == rc) {
                        ESP_LOGI(TAG, "Update Shadow: %s", JsonDocumentBuffer);
                        rc = aws_iot_shadow_update(&mqttClient, device_id, JsonDocumentBuffer,
                                                ShadowUpdateStatusCallback, NULL, 4, true);
                        shadowUpdateInProgress = true;
                    }
                }
            }
            vTaskDelay(SHADOW_UPDATE_INTERVAL / portTICK_RATE_MS);
        }
    }

    if(SUCCESS != rc) {
        ESP_LOGE(TAG, "An error occurred in the loop %d", rc);
    }
}

int init_spiffs(void)
{
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = false
    };
    
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%d)", ret);
        }
        return -1;
    }

    vTaskDelay(1000 / portTICK_RATE_MS);

    return 0;
}

void readFileToString(char *fname, char buf[2048])
{
    printf("  file: \"%s\"\n", fname);

    int res;
    FILE *fd = fopen(fname, "rb");
    if (fd == NULL) {
        printf("  Error opening file (%d) %s\n", errno, strerror(errno));
        free(buf);
        printf("\n");
        return;
    }
    res = 999;
    res = fread(buf, 1, 2047, fd);
    if (res <= 0) {
        printf("  Error reading from file\n");
    }
    else {
        printf("  %d bytes read \n", res);
        buf[res] = '\0';
        // printf("%s\n]\n", buf);
    }
    // free(buf);

    res = fclose(fd);
    if (res) {
        printf("  Error closing file\n");
    }
    printf("\n");
}


// END FUNCTION DEFINITIONS