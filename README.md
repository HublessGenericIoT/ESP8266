# ESP8266
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
Execute ````$ ./flash.sh````
