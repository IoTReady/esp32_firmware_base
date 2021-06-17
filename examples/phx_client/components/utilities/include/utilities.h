#ifndef __UTILITIES_H__
#define __UTILITIES_H__ 1

/**
 * @brief  Helper function to print and return error code.
 * 
 *
 * @param TAG               Identifier for the module.
 * @param error_code        Error code returned by the function call.
 * 
 * 
 * @return                  Error code returned by the function call. Please refer to the IDF libraries for more information on specific error codes.
 * 
 */
esp_err_t error_print_and_return(char* TAG, esp_err_t error_code);


#endif // __UTILITIES_H__
