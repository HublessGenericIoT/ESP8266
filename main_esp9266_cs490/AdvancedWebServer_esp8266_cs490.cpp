#include "AdvancedWebServer_esp8266_cs490.h"
#include "mqtt_esp8266_cs490.h"

const char *ssid = MQTT_CLIENT_ID;
const char *password = "";

const char *st = "<ol>";
String content;
const int led = 2;

ESP8266WebServer server ( 80 );



void handleRoot() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& jsonMsg = jsonBuffer.createObject();
  String msg = "";
  
  jsonMsg["msg"] = "You are connected!";
  jsonMsg.prettyPrintTo(msg);
  
  server.send(200, "application/json", msg);
}

void handleNotFound() {
   StaticJsonBuffer<200> jsonBuffer;
  JsonObject& failureMsg = jsonBuffer.createObject();
	String msg = "";
  failureMsg["msg"] = "File Not Found";
  failureMsg.prettyPrintTo(msg);

	server.send ( 400, "application/json", msg );
}

void handleSetup() {
  StaticJsonBuffer<200> failureBuffer;
  JsonObject& failureMsg = failureBuffer.createObject();
  String msg = "";
  
  //open a json file for writing
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.print("Failed to open config file for writing\n");
    
    failureMsg["msg"] = "Failed to open config file.";
    failureMsg.prettyPrintTo(msg);

    server.send(400, "application/json", msg);
    return;
  }

  //get the json data sent from the client
  StaticJsonBuffer<512> jsonBuffer;
  String jsonData = server.arg(0);
  JsonObject& root = jsonBuffer.parseObject(jsonData);

  if(!root.success()) {
    Serial.println("parseObject() failed");
    Serial.println(server.arg(0));
    failureMsg["msg"] = "Failed to parse config file.";
    failureMsg.prettyPrintTo(msg);

    server.send(400, "application/json", msg);
    
    return;
  }

  //print the data to the config file
  root.printTo(configFile);
  digitalWrite ( led, 1 );

  //send a success message
  JsonObject& msgRoot = jsonBuffer.createObject();
  
  msgRoot["msg"] = "Configuration saved";
  Serial.print("Configuration saved.\n");
  msgRoot.prettyPrintTo(msg);
  server.send(200, "application/json", msg);

  //set up mqtt connection
  hubless_mqtt_setup();
}

void hubless_webserver_dc() {
  WiFi.disconnect();
}

void hubless_webserver_setup () {
  //initialize the LEDs and serial outputs
	pinMode ( led, OUTPUT );
	digitalWrite ( led, 1 );
	Serial.begin ( 115200 );

  //begin the access point
	Serial.println ( "Configuring access point..." );
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  //mount the flash file system
  Serial.println("Mounting FS...");
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  //server callback functions
	server.on ( "/", handleRoot );
  server.on ("/setup", handleSetup);
	server.onNotFound ( handleNotFound );
	server.begin();
	Serial.println ( "HTTP server started" );
}

void hubless_webserver_loop () {
	server.handleClient();
}
