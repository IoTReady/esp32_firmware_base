# C Phoenix Client Library

## Overview

This C library allows using the ESP32 to communicate directly as a client with Phoenix Websocket Servers. 


## Features

- Connects directly to a Phoenix Websocket URL.
- Joins a unique Phoenix channel `device:<device_id>` where `<device_id>` is the 12 digit MAC address of the ESP32.
- Takes cJSON object to send as payload
For more insights, view API specific code documentation [here](./include/phx_client.h).


## Phoenix Channels

- [Phoenix Channels](https://hexdocs.pm/phoenix/channels.html) are similar to MQTT topics - except, they work over websockets or HTTP long-polling. (Currently this library only supports long-polling)
- Devices can connect to multiple channels/topics.
- Channels are multiplexed over a single socket connection
- Authentication is handled at the socket level
- Authorisation is handled at the channel level
- Connecting to a channel requires sending a specific message over the socket.
- 

### Phoenix Events

- `phx_join`: Event for connecting to a channel 
- `new_msg`: Event for connecting to a channel
- `phx_reply`: Some message types get a response from Phoenix server. This is the event of a reply received. 


### Message Structure

A valid Phoenix socket message contains the following keys:
- `topic` - The string topic or "topic:subtopic" pair namespace, such as "messages" or "messages:123"
- `event` - The string event name, for example "phx_join"
- `payload` - The message payload
- `ref` - The unique string ref


#### Example message for connecting to a channel

````
{
	"event":	"phx_join",
	"topic":	"device:<device_id>",
	"ref":	0,
	"payload":	{
	}
}
````

#### Example message to send a message to a channel

````
{
	"event":	"new_msg",
	"topic":	"device:<device_id>",
	"ref":	0,
	"payload":	{
		"count":	1
	}
}
````


## Work in Progress:

- Support for token authorisation.
- SSL authentication using CA signed certificates
- HTTP long-polling support.
- Support for device presence.

> Note: This library has been tested to work with ESP-IDF [v4.2.1](https://github.com/espressif/esp-idf/releases/tag/v4.2.1)


## Feedback

* [create issue on github](https://github.com/IoTReady/esp32_firmware_base/issues/new)

* Click ★ <kbd>Star</kbd> on the top right corner of this page if this repo helps you.

* For any queries, please write to us at hello@iotready.co


## Participate in the development

Fork -> Modify -> Submit PR


## References'
- [Phoenix Channels](https://hexdocs.pm/phoenix/channels.html)
- [Phoenix Channels JavaScript Client](https://github.com/phoenixframework/phoenix/blob/v1.4/assets/js/phoenix.js): [docs](https://hexdocs.pm/phoenix/js/)
- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)
- [FreeRTOS](https://freertos.org/)
