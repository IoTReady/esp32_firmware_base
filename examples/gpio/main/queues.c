#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "common.h"
#include "queues.h"
#include "gpio_interface.h"


#define TAG "QUEUES"


void initialise_queues()
{
    gpio_init_queue = xQueueCreate(10, sizeof(int));

    if (gpio_init_queue == NULL)
    {
        ESP_LOGE(TAG, "Could not create GPIO init queue.");
    }

    gpio_input_queue = xQueueCreate(10, sizeof(int));

    if (gpio_input_queue == NULL)
    {
        ESP_LOGE(TAG, "Could not create GPIO input queue.");
    }

    gpio_output_queue = xQueueCreate(TOTAL_GPIO, sizeof(struct GPIO_Output_Event));

    if (gpio_output_queue == NULL)
    {
        ESP_LOGE(TAG, "Could not create GPIO output queue.");
    }

    ble_notify_queue = xQueueCreate(TOTAL_GPIO, sizeof(struct GPIO_Output_Event));

    if (ble_notify_queue == NULL)
    {
        ESP_LOGE(TAG, "Could not create BLE Notify queue.");
    }

}

