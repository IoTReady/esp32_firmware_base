# WiFi Station Example
## Overview

This example introduces how to use our [wifi_station](../../lib/wifi_station) library to connect to the configured WiFi network for 5 tries.

The library used in this example are:
- [wifi_station](../../lib/wifi_station)

## How to use example
- Run `idf.py menuconfig` to open the project configuration menu. Navigate project-specific "Example Configuration" and set the WiFi SSID and password for the AP you want the station to connect to.
- Build and flash the example normally as any other ESP-IDF project. No special configuration is required during this process.

### Hardware Required

To run this example, you need:
- An ESP32 dev board (e.g. ESP32-WROVER Kit, ESP32-Ethernet-Kit) or ESP32 core board (e.g. ESP32-DevKitC).

> See the corresponding [README.md](../../lib/wifi_station/README.md) file in the corresponding 'lib' directory for more information on this module.