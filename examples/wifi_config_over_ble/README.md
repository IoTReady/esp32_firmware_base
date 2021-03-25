# WiFi configuration on the ESP32 over Bluetooth

## Overview

IoT devices dont always have an accessible user interface allowing almost zero interaction with them after deployment. There are always chances of an unforeseen circumstance occuring where the device needs to be reconfigured. 

Let's talk ESP32 as a station. Your company-wide WiFi infrastructure needs a change, and the credentials cant be the same. If you hadn't foreseen this, you have to go through a massive task of bringing these devices down one-by-one and re-configuring them before they are deployed again.

It can be a problem when some features of the system need configuring once in a while. We recognize this use-case as a common problem that could need solving. In these type of problems, even a simple pre-thought out plan can save a lot of time and effort.

This example implements a BLE server and exposes a service that allows users to configure the WiFi configuration for the device to become a station and connect to that network.


The libraries used in this example are:
- [wifi_config_ble](../../lib/wifi_config_ble)
- [wifi_station](../../lib/wifi_station)

## Bluetooth Specification Documentation
| Service UUID | Characteristic UUID | Description | 
| ------------ | ------------------- | ----------------- |
| `0x00EE`     | `0xEE00`            |   WiFi SSID       |
| `0x00EE`     | `0xEE01`            |   WiFi Password   |
| `0x00EE`     | `0xEE02`            |   WiFi Connect    |
 

## How to use example
- Build and flash the example normally as any other ESP-IDF project. No special configuration is required during this process.
- The ESP32 will create a BLE server with the advertisement name IoT-<12-digit-mac-address>
- Connect to the device and send data to the device according to the specification table above. You can use any BLE app to do so.

### Hardware Required

To run this example, you need:
- An ESP32 dev board (e.g. ESP32-WROVER Kit, ESP32-Ethernet-Kit) or ESP32 core board (e.g. ESP32-DevKitC).
