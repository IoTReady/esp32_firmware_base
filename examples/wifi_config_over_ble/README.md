# WiFi configuration on the ESP32 over Bluetooth

## Overview

IoT devices dont always have an accessible local user interface, resulting in almost zero capabilitiy for interaction with them after deployment if they are not connected to the cloud infrastructure that supports them. Unfortunately there are more than a few chances of an unforeseen circumstance occuring where the device needs to be reconfigured. 

Let's talk about a simple example. Your ESP32 devices are functioning in station mode and are connected to your company sub-network. Your company changes your subnet SSID and as a result credentials have to be updated across your set of nodes. If you hadn't foreseen this use-case, you have to go through a laborious task of bringing these devices down one-by-one and re-configuring them before they are deployed again. Wouldn't it be helpful if you had some kind of local access that didn't depend on the main infrastructure being up and running ?

While we are talking in this simple example about WiFi credentials, this capability can also be useful when some features of the system need configuring once in a while. In these type of problems, even a simple pre-thought out plan can save a lot of time and effort. Since our devices support BLE, it makes obvious that a solution to this is to expose critical parameters behind BLE accessible points (called characteristics in BLE jargon).

This example implements a BLE server and exposes a service that allows users to configure the WiFi configuration for the device to become a station and connect to that network. Obviously, you can add other variables / commands to the example to handle your specific use cases.


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
- Bench DC power supply.
- Computer with ESP-IDF installed and configured.
