#define MQTT_MAX_PACKET_SIZE 512
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "FS.h"
#include <ArduinoJson.h>

void hubless_mqtt_setup();
void hubless_mqtt_loop();
