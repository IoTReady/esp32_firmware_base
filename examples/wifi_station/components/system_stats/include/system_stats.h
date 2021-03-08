#ifndef __SYSTEM_STATS_H__
#define __SYSTEM_STATS_H__ 1


/**
 * @brief   Function to get free heap size.
 *
 * @return
 *  - uint32_t              Free heap size
 */
uint32_t system_get_free_heap();


/**
 * @brief   Function to print the current CPU usage of tasks.
 *
 * This example demonstrates how a human readable table of run time stats information is generated from raw data provided by uxTaskGetSystemState(). The human readable table is written to pcWriteBuffer.
 *
 * @warning configUSE_TRACE_FACILITY must be defined as 1 in FreeRTOSConfig.h for
 *          uxTaskGetSystemState() to be available.
 * @warning configGENERATE_RUN_TIME_STATS must be defined as 1 in FreeRTOSConfig.h for
 *          uxTaskGetSystemState() to be available.
 * 
 * @note    This function should be called from a high priority task to minimize
 *          inaccuracies with delays.
 * @note    When running in dual core mode, each core will correspond to 50% of
 *          the run time.
 *
 */
void system_print_runtime_stats( char *pcWriteBuffer );

/**
 * @brief   Function to print the CPU usage of tasks over a given duration.
 *
 * This function will measure and print the CPU usage of tasks over a specified
 * number of ticks (i.e. real time stats). This is implemented by simply calling
 * uxTaskGetSystemState() twice separated by a delay, then calculating the
 * differences of task run times before and after the delay.
 *
 * @warning configUSE_TRACE_FACILITY must be defined as 1 in FreeRTOSConfig.h for
 *          uxTaskGetSystemState() to be available.
 * @warning configGENERATE_RUN_TIME_STATS must be defined as 1 in FreeRTOSConfig.h for
 *          uxTaskGetSystemState() to be available.
 * 
 * @note    If any tasks are added or removed during the delay, the stats of
 *          those tasks will not be printed.
 * @note    This function should be called from a high priority task to minimize
 *          inaccuracies with delays.
 * @note    When running in dual core mode, each core will correspond to 50% of
 *          the run time.
 *
 * @param   xTicksToWait    Period of stats measurement
 *
 * @return
 *  - ESP_OK                Success
 *  - ESP_ERR_NO_MEM        Insufficient memory to allocated internal arrays
 *  - ESP_ERR_INVALID_SIZE  Insufficient array size for uxTaskGetSystemState. Trying increasing ARRAY_SIZE_OFFSET
 *  - ESP_ERR_INVALID_STATE Delay duration too short
 */
esp_err_t system_print_real_time_stats(TickType_t xTicksToWait);

#endif // __SYSTEM_STATS_H__