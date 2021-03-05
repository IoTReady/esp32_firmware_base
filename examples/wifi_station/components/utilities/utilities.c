#include "esp_log.h"
#include "esp_err.h"

esp_err_t error_print_and_return(char* TAG, esp_err_t error_code) {
  ESP_LOGE(TAG, "Error %x: %s", error_code, esp_err_to_name(error_code)); 
  return error_code;
}