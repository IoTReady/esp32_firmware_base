# WiFi SoftAP Example
## Overview

This example introduces how to use our [wifi_softap_interface](../../lib/wifi_softap_interface) library to start a WiFi softap for 30 seconds and stop it.

The library used in this example are:
- [wifi_softap_interface](../../lib/wifi_softap_interface)

## How to use example
- Build and flash the example normally as any other ESP-IDF project. No special configuration is required during this process.
- The device will create a softap(hotspot) with the following credentials:
  
|   Parameter      |    Value                               |
| -----------------|----------------------------------------|
| SSID             | IoT-\<12-digit-mac-ID-of-the-device>   |
| Password         | getiotready                            |

- The password can be changed by editing the `CONFIG_ESP_WIFI_PASSWORD` macro in [tasks.c](./main/tasks.c)

### Hardware Required

To run this example, you need:
- An ESP32 dev board (e.g. ESP32-WROVER Kit, ESP32-Ethernet-Kit) or ESP32 core board (e.g. ESP32-DevKitC).

> See the corresponding [README.md](../../lib/wifi_softap/README.md) file in the corresponding 'lib' directory for more information on this module.