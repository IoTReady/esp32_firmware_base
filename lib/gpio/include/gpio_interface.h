#ifndef __GPIO_INTERFACE_H__
#define __GPIO_INTERFACE_H__ 1

#include "driver/gpio.h"

/**
 * Structure which holds data necessary for passing events to the output queue. 
 */
struct GPIO_Output_Event {
    int gpio_num, state; 
};

/**
 * @brief  Install the driver’s GPIO ISR handler service, which allows per-pin GPIO interrupt handlers.
 *
 * @return
 *    - ESP_OK: Success
 *    - ESP_ERR_NO_MEM: No memory to install this service
 *    - ESP_ERR_INVALID_STATE: ISR service already installed
 *    - ESP_ERR_NOT_FOUND: No free interrupt found with the specified flags
 *    - ESP_ERR_INVALID_ARG: GPIO error
 * 
 */
esp_err_t gpio_init_interface();

/**
 * @brief  Add ISR handler for the corresponding GPIO pin.
 * 
 * Call this function after using gpio_install_isr_service() to install the driver’s GPIO ISR handler service. This function also sets up a handler that fills the queue "gpio_input_queue".
 *
 * @param gpio_num GPIO number
 * 
 * @return
 *    - ESP_OK: Success
 *    - ESP_ERR_INVALID_STATE: Wrong state, the ISR service has not been initialized
 *    - ESP_ERR_INVALID_ARG: Parameter error
 * 
 */
// esp_err_t gpio_enable_pin_isr(int gpio_num);
esp_err_t gpio_enable_pin_isr(int gpio_num, gpio_isr_t isr_handler);


/**
 * @brief  Select pad as a gpio function from IOMUX, set the gpio to input mode and get the current level.
 *
 * @param gpio_num GPIO number. If you want to get the logic level of e.g. pin GPIO16, gpio_num should be GPIO_NUM_16 (16).
 * 
 * @return
 *     - 0 the GPIO input level is 0
 *     - 1 the GPIO input level is 1
 * 
 */
int gpio_get_state(int gpio_num);

/**
 * @brief  Select pad as a gpio function from IOMUX, set the gpio to output mode and set the output level.
 *
 * @param  gpio_num GPIO number. If you want to set the output level of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16).
 * @param  state Output level. 0: low ; 1: high
 * 
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO number error
 * 
 */
esp_err_t gpio_set_state(int gpio_num, int state);

#endif // __GPIO_INTERFACE_H__
