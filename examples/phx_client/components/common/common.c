#include "esp_system.h"

const char* get_device_id()
{
    static char *device_id[13];

    uint8_t chipid[6];
    esp_efuse_mac_get_default(chipid);
    sprintf(device_id,"%02x%02x%02x%02x%02x%02x",chipid[0], chipid[1], chipid[2], chipid[3], chipid[4], chipid[5]);

    return device_id;
}