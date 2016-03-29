#include "AdvancedWebServer_esp8266_cs490.h"
#include "mqtt_esp8266_cs490.h"

extern int connectedToWifi;

void setup() {
  //set up Web Server to obtain configuration info
  hubless_webserver_setup();
}

void loop() {
  switch(connectedToWifi) {
    case 0:
      //keep running Web Server to receive info from a client
      hubless_webserver_loop();
      break;
    case 1:
      //this should publish messages once device is connect to the internet
      hubless_mqtt_loop();
      break;
  }
}
