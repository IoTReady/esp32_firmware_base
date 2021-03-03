# IoTReady Demo Firmware for ESP32

### Project Structure
- [lib](./lib) folder contains all libraries/modules
- [examples](./examples) contains examples using modules contained in the above mentioned lib folder.

### Goal Framework Modules
#### v1.0
- System Status API: get [free heap sizes](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system.html#_CPPv422esp_get_free_heap_size), [task status](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html?highlight=uxtaskgetsystemstate#_CPPv420uxTaskGetSystemStatePC12TaskStatus_tK11UBaseType_tPC8uint32_t), [run-time stats](https://www.freertos.org/rtos-run-time-stats.html)
  - Example: Run multiple sample tasks and periodically print system stats.
- GPIO Interface: Initialize, enable ISR, get/set state uxTaskGetSystemState
  - Example: Atleast one button, one led and one interrupt service routine using queues to share state.
- WiFi Hotspot/Softap API: Initialize, create hotspot
  - Example: Create a hotspot and create/render a QR code that contains info about the wifi credentials to be used to connect to it easily.
  - REST API for WiFi SSID and password configuration to then connect to another AP.
- WiFi Station: Initialize, connect to an external AP.
  - Ability to connect to external WPA2-PSK Access Point
  - Example code that gets the Access Point credentials from:
    - REST API calls
    - Bluetooth (?)
#### Post v1.0
- REST API for control signals similar to [Nerves example](https://elixirschool.com/en/)
- Websockets API for state communication
- ESP32 will send updated state via websockets to every connected client
  - Websockets is TCP based so does not have broadcast. 
  - ESP32 will maintain a list of connected clients
- Clean, abstracted library for GPIO control (already in place)
- Clean, abstracted library for WiFi control (TODO)
- Clean, abstracted library for Websocket control (TODO)
- Central dispatcher for client and task coordination
- State expressed as a bit sequence (int8)

### Stretch Goals
- mDNS support

### Non Goals
- BLE/Bluetooth support
- OTA support




## Goals (in order of priority)
- Clean, decoupled code with obvious modules for new code to sit in
- No/minimal global state
- Abstracted base utility functions
- Event driven flow based using states and dependencies
- Abstraction of flow into custom domain specific language (DLS) e.g. JSON

## References
- [Living document](https://docs.google.com/document/d/105yF2BfyxGQoTqmHD_4SXcBsB_iKOMyMGL3y_QxcSfE)
- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)
- [FreeRTOS](https://freertos.org/)

## Inspirations
- [Node-RED](https://nodered.org/)
- [Nerves]()
- [Juniper for Arduino](http://www.juniper-lang.org/index.html)
- [Well-typed-lightbulbs](https://github.com/well-typed-lightbulbs)
- [Microflo](https://github.com/microflo/microflo)
- [Mongoose](https://mongoose-os.com/)
- [uLisp](http://www.ulisp.com/)
- [micro:bit](https://spivey.oriel.ox.ac.uk/baremetal/The_micro:bit_page)
- [ChibiOS](http://www.chibios.org/dokuwiki/doku.php?id=chibios:documentation:books:rt:intro)
- [Hailstorm](https://abhiroop.github.io/pubs/hailstorm.pdf)
