#ifndef __COMMON_H__
#define __COMMON_H__ 1

#define LOW_PRIORITY 5
#define HIGH_PRIORITY 10

#define LARGEST_STACK 15360
#define LARGER_STACK 8192
#define LARGE_STACK 4096
#define MEDIUM_STACK 2048
#define SMALL_STACK 1024

#define TOTAL_GPIO 50

// wifi_sta_init_struct_t wifi_params_sta;

/**
 * @brief  Get MAC address of the device
 * 
 * 
 * @return  char* : MAC address
 * 
 */
const char* get_device_id();

#endif // __COMMON_H__