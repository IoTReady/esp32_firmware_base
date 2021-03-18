#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "common.h"
#include "gpio_interface.h"
#include "queues.h"
#include "tasks.h"
#include "system_stats.h"


#define TAG "TASKS"

#define EXAMPLE_BUTTON_GPIO     CONFIG_BUTTON_GPIO
#define EXAMPLE_LED_GPIO        CONFIG_LED_GPIO


TaskHandle_t *system_stats_task_handle, *led_task_handle, *button_task_handle = NULL;
volatile unsigned long int current,last = 0;

void button_task(void *pvParameter);
void led_task(void *pvParameter);

static void IRAM_ATTR gpio_handle_isr(void* arg)
{
    current = xTaskGetTickCountFromISR();
    if(abs(current - last) > 20)
    {
        uint32_t gpio_num = (uint32_t) arg;

        // ESP_LOGI(TAG, "Received event for GPIO NUM %d", gpio_num);

        // This is an ISR. We should not run any blocking code so we send the GPIO number to the input queue and get out of the way.

        xQueueSendFromISR(gpio_input_queue, &gpio_num, NULL);
    }
    last = current;
}

void button_task(void *pvParameter) 
{
    esp_err_t err;

    int gpio_num = EXAMPLE_BUTTON_GPIO;

    struct GPIO_Output_Event data = {gpio_num, 0};

    err = gpio_enable_pin_isr(gpio_num, gpio_handle_isr);

    data.gpio_num = EXAMPLE_LED_GPIO;
    while (1) {
         //Wait for gpio input event
        if(xQueueReceive(gpio_input_queue, &gpio_num, (portTickType)portMAX_DELAY)) 
        {
            data.state = !data.state;
            xQueueSend(gpio_output_queue, &data, 0);
        }
    }
}

void led_task(void *pvParameter)
{
    struct GPIO_Output_Event data;
    while (1) {
         //Wait for gpio output event
        if(xQueueReceive(gpio_output_queue, &data, (portTickType)portMAX_DELAY)) 
        {
            ESP_LOGI(TAG, "Interrupt triggered! Changing LED GPIO %d state to: %d", data.gpio_num, data.state);
            gpio_set_state(data.gpio_num, data.state);
        }
    }
}

void init_tasks()
{
    // xTaskCreate(&system_stats_task, "system_stats_task", MEDIUM_STACK, NULL, LOW_PRIORITY, system_stats_task_handle);
    xTaskCreate(&led_task, "led_task", MEDIUM_STACK, NULL, LOW_PRIORITY, led_task_handle);
    xTaskCreate(&button_task, "button_task", MEDIUM_STACK, NULL, LOW_PRIORITY, button_task_handle);
}

