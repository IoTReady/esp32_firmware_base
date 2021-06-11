#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "common.h"
#include "stdio.h"
#include "aws_control.h"

#define TAG "TASKS"


void init_tasks()
{
    int spiffs_ok = init_spiffs();
    if (spiffs_ok == 0)
    {
        // read aws certificates into memory
        readFileToString("/spiffs/certificate.pem.crt", certificate_pem_crt_start);
        readFileToString("/spiffs/private.pem.key", private_pem_key_start);
        readFileToString("/spiffs/aws-root-ca.pem", aws_root_ca_pem_start);
        xTaskCreate(&aws_iot_task, "aws_iot_task", LARGER_STACK, NULL, LOW_PRIORITY, NULL);
    }
}

