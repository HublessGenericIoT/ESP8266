<!-- # ESP8266
## Setup
The source code (that you should modify) is:
- /src/user/user_main.c (here is the program, but you may want to break it up into separate files)
- /src/include/mqtt_config.h (network setup, such as SSID, password, host, port, etc.)
- /src/include/user_config.h (I don't know what this is, but it looks important)
- /src/Makefile (if you need to modify this, good luck!)

## Compiling
1. Navigate to /src/
2. Execute ````$ ./init.sh````

## Flashing
Execute ````$ ./flash.sh```` -->

# ESP8266 Arduino

## Source Code

The source code for this Arduino project is located in main_esp9266_cs490

- AdvancedWebServer_esp8266_cs490.(cpp/h) are the files that make up the Access Point and server funcitonality
- mqtt_esp8266_cs490.(cpp/h) are the files that make up the MQTT functionality
- main_esp9266_cs490.ino is the Arduino file that pulls in the functions from the two previously named files.
- mqtt_config.h contains default values for the MQTT server credentials

In the ESP8266 parent directory are two scripts (get.sh and post.sh) that are used to communicate with the ESP8266 device when connected to its access point.

## Requirements

In order to compile and upload the code to the ESP8266 device you must have the following:

- Setup the Arduino IDE to recognize the ESP8266. Follow the instructions here:  http://learn.acrobotic.com/tutorials/post/esp8266-getting-started#step-7-lnk

- Install the following Arduino libraries using the Arduino IDE library manager:
  - Adafruit MQTT Library
  - ArduinoJson
  - PubSubClient