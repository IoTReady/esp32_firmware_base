# Clean, abstracted library for WiFi softAP control

### Overview
This module provides wrapper APIs over the ESP-IDF APIs that provide WiFi softap functionalities.

### Features
- Zero/minimum global states ensures high code quality
- In-built error handling
- Handles complex structures and configurations for you.

For example, the wifi_init_softap() handles the following:
- Initializes the underlying TCP/IP stack
- Creates a default event loop
- Registers an event handler
- Sets operating mode and configuration
- Starts WiFi with the above configuration

For more insights, view API specific code documentation [here!](./include/wifi_softap.h)