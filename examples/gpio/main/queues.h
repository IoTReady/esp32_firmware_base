#ifndef __QUEUES_H__
#define __QUEUES_H__ 1

#include "freertos/queue.h"

void initialise_queues();

QueueHandle_t gpio_init_queue, gpio_input_queue, gpio_output_queue, ble_notify_queue;


#endif // __QUEUES_H__
