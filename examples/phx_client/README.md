# Phoenix Client implementation with ESP32

The major libraries used in this example are:
- [phx_client](../../lib/phx_client)
- [wifi_station](../../lib/wifi_station) 

## Running the example:

- Configure `Example Configuration` in the menuconfig. Set WiFi credentials and Phoenix Websocket URL
````
$ idf.py menuconfig
````
- Build and flash as normal:
````
$ idf.py flash
````
The device will connect to the URL provided, connect to the particular device channel 'device:<device_id>' and then start sending a count data in JSON format. The JSON is achieved with the [cJSON](https://github.com/DaveGamble/cJSON) library.

### Hardware Required

To run this example, you need:
- An ESP32 dev board (e.g. ESP32-WROVER Kit, ESP32-Ethernet-Kit) or ESP32 core board (e.g. ESP32-DevKitC).
- Bench DC power supply.
- Computer with ESP-IDF installed and configured. This example has been tested to work with ESP-IDF stable [v4.2.1](https://github.com/espressif/esp-idf/releases/tag/v4.2.1)


