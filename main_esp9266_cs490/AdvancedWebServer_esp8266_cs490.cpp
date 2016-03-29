#include "AdvancedWebServer_esp8266_cs490.h"
#include "mqtt_esp8266_cs490.h"

const char *ssid = MQTT_CLIENT_ID;
const char *password = "";

const char *st = "<ol>";
String content;

ESP8266WebServer server ( 80 );

const int led = 13;

void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

void handleNotFound() {
	digitalWrite ( led, 1 );
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";

	for ( uint8_t i = 0; i < server.args(); i++ ) {
		message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
	}

	server.send ( 404, "text/plain", message );
	digitalWrite ( led, 0 );
}

void handleSetup() {
  //open a json file for writing
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    server.send(200, "text/plain", "failed1");
    return;
  }

  //get the json data sent from the client
  String jsonData = server.arg(0);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonData);

  if(!root.success()) {
    Serial.println("parseObject() failed");
    server.send(200, "text/plain", "failed2");
  }

  //print the data to the config file
  root.printTo(configFile);

  //send a success message
  content = "<!DOCTYPE HTML>\r\n<html>";
  content += "<p>Configuration saved.</p></html>";
  
  server.send(200, "text/html", content);

  //set up mqtt connection
  hubless_mqtt_setup();
}

void hubless_webserver_setup () {
  //initialize the LEDs and serial outputs
	pinMode ( led, OUTPUT );
	digitalWrite ( led, 0 );
	Serial.begin ( 115200 );

  //begin the access point
	Serial.println ( "Configuring access point..." );
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
