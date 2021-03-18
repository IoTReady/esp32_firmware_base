#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "gpio_interface.h"
// #include "queues.h"

#define TAG "GPIO_INTERFACE"

// gpio sw debounce state

// static void IRAM_ATTR gpio_handle_isr(void* arg);

esp_err_t gpio_init_interface() {
    esp_err_t err;
    //install gpio isr service
    err = gpio_install_isr_service(0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not install GPIO ISR service");
    }
    return err;
}

esp_err_t gpio_enable_pin_isr(int gpio_num, gpio_isr_t isr_handler)
{
    ESP_LOGI(TAG, "Configuring GPIO NUM %d", gpio_num);
    esp_err_t err;
    gpio_config_t io_conf;

    //interrupt on falling edge
    io_conf.intr_type = GPIO_PIN_INTR_NEGEDGE;
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins
    io_conf.pin_bit_mask = 1ULL<<gpio_num;
    //enable pull-down mode
    io_conf.pull_down_en = 1;
    err = gpio_config(&io_conf);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not configure GPIO NUM %d", gpio_num);
        return err;
    }

    err = gpio_isr_handler_add(gpio_num, (gpio_isr_t*) isr_handler, (void*) gpio_num);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not add ISR handler for GPIO NUM %d", gpio_num);
        return err;
    }

    return err;
}


int gpio_get_state(int gpio_num)
{
    int state;
    esp_err_t err;
    gpio_pad_select_gpio(gpio_num);
    // If the pad is not configured for input (or input and output) the returned value is always 0.
    err = gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not set GPIO NUM %d as Input", gpio_num);
        return err;
    }
    state = gpio_get_level(gpio_num);
    ESP_LOGI(TAG, "GPIO PIN NUM %d state = %d", gpio_num , state);
    return state;
}


esp_err_t gpio_set_state(int gpio_num, int state)
{
    esp_err_t err;
    gpio_pad_select_gpio(gpio_num);
    err = gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not set GPIO NUM %d as Output", gpio_num);
        return err;
    }
    err = gpio_set_level(gpio_num, state);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Could not set level of GPIO NUM %d", gpio_num);
    }
    return err;
}


