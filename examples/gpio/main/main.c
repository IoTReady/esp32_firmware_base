#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include "gpio_interface.h"
#include "queues.h"
#include "tasks.h"

#define TAG "main"

/**
 * Main entry point of the program.
*/
void app_main(void)
{
    initialise_queues();
    
    gpio_init_interface();

    init_tasks();

}
