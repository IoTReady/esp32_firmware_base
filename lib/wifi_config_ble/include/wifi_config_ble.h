#ifndef __WIFI_CONFIG_BLE_H__
#define __WIFI_CONFIG_BLE_H__ 1

/**
 * @brief  Initialize BLE
 * 
 * Initialize and enable bluetooth controller, then initialize and enable Bluedroid
 * 
 * 
 * @return
 *    - ESP_OK: Success
 *    - Other : Failed
 * 
 */
esp_err_t init_ble();

/**
 * @brief  Enable BLE
 * 
 * Register GATTS and GAP callbacks,setup advertising, register Service and set local MTU
 * 
 * 
 * @return
 *    - ESP_OK: Success
 *    - Other : Failed
 * 
 */
esp_err_t enable_ble();

/**
 * @brief  Disable BLE
 * 
 * Stop BLE advertisement and unregister application
 * 
 * 
 * @return
 *    - ESP_OK: Success
 *    - Other : Failed
 * 
 */
esp_err_t disable_ble();


#endif // __WIFI_CONFIG_BLE_H__
