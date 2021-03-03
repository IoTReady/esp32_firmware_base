#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "system_stats.h"
#include "tasks.h"

#define TAG "main"

/**
 * Main entry point of the program.
*/
void app_main(void)
{
    initialize_tasks();
}
