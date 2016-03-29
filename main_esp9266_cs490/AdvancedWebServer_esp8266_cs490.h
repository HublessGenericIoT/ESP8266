#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include "mqtt_config.h"
#include "FS.h"

void hubless_webserver_setup();
void hubless_webserver_loop();
