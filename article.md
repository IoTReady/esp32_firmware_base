# IoT synonymous with the ESP32
- With its lucrative price and power-packed performance, Espressif's ESP32 has taken large portions of the IoT space in today's world of "connected things". 
- One of the cheapest and most popular hybrid Bluetooth + WiFi chips available in the market even today,four and a half years after its initial release successing its predecessor ESP8266 has definitely made its mark in the world.
- I will not dig too deep into the features of ESP32 since there are only already thousands of other articles including Espressif themselves who have done great to highlight its capabilities.
- All I'm willing to say is, if you haven't already looked into the ESP32, YOU'RE MISSING OUT BIG TIME!
- The ESP-IDF is a great framework with constant development and active forum discussions. These pros with nice documentation, the ESP-IDF framework draws in hobbyists and professionals towards it like bee to a beautiful flower.

### This brings me to talk about why I'm writing this article:
- While most hobbyists and professionals take complete advantage of the ESP32's Arduino IDE compatibility(as they should), some handful( maybe more :) )like myself will strongly argue that to squeeze out the best features in its prime capabilities, the best has been and most likely will stay for quite some time - C.
- Despite the great framework and support on the IDF, one could argue that help and data are less easily available to the interested public on IDF when compared to ESP32 on the Arduino framework.
- Some common use-cases are used more often than others. We feel the need and gap for these cases to be highlighted and made more easily available to learn and discuss. These use-cases can be better covered, and information on those can be brought easier to those interested or in need of help. This would provide manifold increase in speed in prototyping and development.
- Our initiative aims at sharing with the world the expertise and experience we have gained over the years building prototypes and products with the ESP32.


### IoTReady ESP32 Framework
- The framework consists of libraries that provide ready-to-use APIs that are built on top of the functions provided by the ESP-IDF and FreeRTOS.
- Maximum focus on exposing easier-to-use and ready-to-deploy API wrapper over the ESP-IDF that allows programmers to write the least amount of code and care less about lower level implementations like building complex structures or handling error messages.
- Goal not only to save the programmer's time in writing code, but also to encourage easier maintenance of homogeneity in code writing and structuring.
- Implement most frequently used IoT use-cases into ready-to-build examples using APIs from this library.

The [wifi_config_rest](https://github.com/IoTReady/esp32_firmware_base/tree/master/examples/wifi_config_rest) example incorporates the [wifi_softap](https://github.com/IoTReady/esp32_firmware_base/tree/master/lib/wifi_softap) and [wifi_station](https://github.com/IoTReady/esp32_firmware_base/tree/master/lib/wifi_station) libraries to implements a REST API server over HTTP that allows for easy WiFi station configuration without any need to access the console. Remote device configuration is a very specific but common use-case that IoT enablers should think about. For more information on this interesting use-case, see the example specific [README](https://github.com/IoTReady/esp32_firmware_base/blob/master/examples/wifi_config_rest/README.md)