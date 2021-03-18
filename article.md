# Easy EPS32 applications with building blocks - Network configuration.
With its lucrative price and power-packed performance, Espressif's ESP32 has taken large portions of the IoT space in today's world of "connected things".

One of the cheapest and most popular hybrid Bluetooth + WiFi chips available in the market even today, four and a half years after its initial release supercedeing its predecessor the ESP8266, the ESP32 has definitely made its mark in the world.

I will not dig too deep into the features of ESP32 since there are only already thousands of other articles including plenty by Espressif themselves that highlight its capabilities.

All I'm willing to say is, if you haven't already looked into the ESP32, YOU'RE MISSING OUT BIG TIME!

The ESP-IDF is a great framework with constant development and active forum discussions. These together with nice documentation mean that ESP-IDF framework draws in hobbyists and professionals bees to a beautiful flower.

### This brings me to why I'm writing this article:

While most hobbyists and professionals take complete advantage of the ESP32's Arduino IDE compatibility(as they should), some handful( maybe more :) )like myself will strongly argue that to squeeze out the best features of its capabilities, the best way has been and most likely will stay for quite some time to program it directly in C.

Despite the great framework and support on the IDF, one could argue that help and data are less easily available to the interested public on IDF when compared to ESP32 on the Arduino framework.

We have built enough ESP32 applications that we have identifed some common use-cases that are used more often than others. They are not unique to our applications but are use cases that almost any implementer will end up needing to consider. In order to make it easier for others to use the ESP32, and to make more rapid progress, we have decided to share some of these common use case building blocks with the community.

This article and the corresponding open source code made available on Github, show you the first of these blocks. More will come as we upgrade the repository going forward.


### IoTReady ESP32 Framework
- The framework consists of libraries that provide ready-to-use APIs that are built on top of the functions provided by the ESP-IDF and FreeRTOS.
- The maximum focus is on exposing easier-to-use and ready-to-deploy API wrappers over the ESP-IDF that allows programmers to write the least amount of code and care less about lower level implementations like building complex structures or handling error messages.
- The goal is not only to save the programmer's time in writing code, but also to encourage easier maintenance of homogeneity in code writing and structuring.
- Implement most frequently used IoT use-cases into ready-to-build examples using APIs from this library.

Whenever you start building am ESP32 product that uses WiFi for communications, you are faced with the problem of how to get the device to connect and link to your WiFi network. While developing code, its easy enough to set these over the serial port, but that is not something you can expect your customer to do in real life. You need a minimal overhead, easy to use process that doesn't require a custom application or tools in order to configure WiFi for the customers network.

The simplest workflow to achieve this goal is as follows:

1.   Have the node wake up and establish a WiFi hot spot that can easily be recognized by the user and connected to from their phone or computer. In our case, the hotspot is created with a common base "IoTReady" followed by the last bytes of the nodes MAC address and a standard password. 
2. The user connects to the hotspot.  Since the framework has initialized the network address space and assigned the address for the node, it is easy for the user to simply type a short http REST request into their browser to set the SSID and password.

3. On succesfull setting of the SSID and password, the node reboots and transitions to station mode and connects to the user's desired network. THere are a few other RESt API's that are supported in the hotspot mode so that a user can get get details of the node and check node status in case of problems with the process

The [wifi_config_rest](https://github.com/IoTReady/esp32_firmware_base/tree/master/examples/wifi_config_rest) example incorporates the [wifi_softap](https://github.com/IoTReady/esp32_firmware_base/tree/master/lib/wifi_softap) and [wifi_station](https://github.com/IoTReady/esp32_firmware_base/tree/master/lib/wifi_station) libraries to implements the necessary REST API server over HTTP that allows for easy WiFi station configuration without any need to access the console. 

Remote device configuration is a very specific but common use-case that IoT enablers should think about. For more information on this use-case, see the example specific [README](https://github.com/IoTReady/esp32_firmware_base/blob/master/examples/wifi_config_rest/README.md)