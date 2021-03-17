# WiFi Config Rest Example

Starts a WiFi softap for 30 seconds and ends it.

See the corresponding [README.md](../../lib/wifi_softap/README.md) file in the corresponding 'lib' directory for more information on this module.
## Overview

This example introduces how to implement a RESTful API server and HTTP server on ESP32 using our ESP32 framework built on top of the ESP-IDF. While doing so, it also exhibits handling of the common use-case of configuring the ESP32 to connect to the desired WiFi network remotely through a simple HTTP call. 

The libraries used in this example are:
- [wifi_softap](../../lib/wifi_softap)
- [wifi_station](../../lib/wifi_station) 

## API Documentation
| API                      | Method | Resource Example                                       	   | Description                                                                          |
| ------------------------ | ------ | ------------------------------------------------------------ | ------------------------------------------------------------------------------------ |
| `/api/system/info`       | `GET`  | {<br />version:"v4.0-dev",<br />cores:2<br />}         	   | Used for clients to get system information like IDF version, ESP32 cores, etc        |
| `/api/wifi/credential`   | `POST` | { <br />"ssid":"helloSSID",<br />"password":"helloPW"<br />} | Used for clients to upload WiFi configuration to ESP32 in order to connect to an AP  |

## How to use example
- Build and flash the example normally as any other ESP-IDF project. No special configuration is required during this process.
- The ESP32 will create a softAP(hotspot) with the credentials:
  
|   Parameter      |    Value                               |
| -----------------|----------------------------------------|
| SSID             | IoT-\<12-digit-mac-ID-of-the-device>   |
| Password         | getiotready                            |

- Connect to the hotspot from your computer/phone to use as a client and make the API call as mentioned [above](#api-documentation). One easy-to-use client is [Postman REST Client](https://www.postman.com/product/rest-client/).

### Hardware Required

To run this example, you need:
- An ESP32 dev board (e.g. ESP32-WROVER Kit, ESP32-Ethernet-Kit) or ESP32 core board (e.g. ESP32-DevKitC).
