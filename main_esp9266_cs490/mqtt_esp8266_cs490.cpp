#include "mqtt_esp8266_cs490.h"
#include <string.h>

// Update these with values suitable for your network.
WiFiClient espClient;
PubSubClient client(espClient);

//used to repeatedly send and recieve mqtt messages
long lastMsg;
long value;
int now;
char msg[256];
char outTopic[256];
char inTopic[256];
char shadow[256];
char shadowUpdate[256];

//information about the device, wifi, and mqtt connections.
char dev_id[50];
char sta_ssid[50];
char sta_pass[50];
char sta_auth[50];
char mqtt_user[50];
char mqtt_pass[50];
char mqtt_endpoint[50];
int mqtt_port = 1883;

//wifi connection status
int connectedToWifi;

//led pin value
const int led = 2;
const int switchPin = 13;

//desired/current state of the device
char desiredStateStr[50];
char currentStateStr[50];
int currentState;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(sta_ssid);

  //connect to wifi
  WiFi.begin(sta_ssid, sta_pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  connectedToWifi = 1;
  
  //WiFi.mode(WIFI_STA); this causes a crash to the device
}

void callback(char* topic, byte* payload, unsigned int length) {
  digitalWrite ( led, 0 );
  delay(10);

  //print message to serial monitor (testing and demo)
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  delay(10);

  //did we get sent a shadow? if so, update the device status.
  if((char)*topic == '$' ) {
    //parse the incoming message (assume device shadow/json format)
    StaticJsonBuffer<512> jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject((char*)payload);
  
    //check if parsing was successful
    if (!json.success()) {
      Serial.println("Failed to parse config file");
      return;
    }

    //retrieve the desired state
    strcpy(desiredStateStr, json["state"]["desired"]["state"]);

    Serial.print("Desired State is ");
    Serial.println(desiredStateStr);

    if(String(desiredStateStr) != String(currentStateStr)) {
      Serial.print("State changed from ");
      Serial.print(currentStateStr);
      Serial.print(" to ");
      Serial.print(desiredStateStr);
      Serial.println(".");

      //update the current state
      strcpy(currentStateStr, desiredStateStr);
      
      currentState = !currentState;
      digitalWrite(switchPin, currentState);
    }
    
    delay(10);
  }

  digitalWrite ( led, 1 );
}

void reconnect() {
  // Loop until we're reconnected  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect to provided mqtt server
    if (client.connect(dev_id, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      
      // Once connected, publish an announcement...
      client.publish(outTopic, "connected.");

      //Initialize shadow
      char updateMsg[50];
      strcpy(updateMsg, "{\"state\":{\"desired\":{\"state\":\"");
      strcat(updateMsg, currentStateStr);
      strcat(updateMsg, "\"}}}");
      
      Serial.print("Initialized shadow to: ");
      Serial.println(updateMsg);
      client.publish(shadowUpdate, updateMsg);
      
      // ... and resubscribe
      client.subscribe(inTopic);
      client.subscribe(shadow, 1);
    } else {
      //try to reconnect to the AP
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

bool loadConfig() {
  //open the file
  File configFile = SPIFFS.open("/config.json", "r");

  //check if the file opened successfully
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  //check the file size
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  //read the file into the buffer
  configFile.readBytes(buf.get(), size);

  //parse the config file into a JSON object
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  //check if parsing was successful
  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  //parse the config items
  //device id
  strcpy(dev_id, json["id"]);
  
  //wifi info
  strcpy(sta_ssid, json["wifiConfig"]["ssid"]);
  strcpy(sta_pass, json["wifiConfig"]["password"]);
  strcpy(sta_auth, json["wifiConfig"]["authmode"]);
  
  //mqtt info
  strcpy(mqtt_user, json["mqttConfig"]["user"]);
  strcpy(mqtt_pass, json["mqttConfig"]["password"]);
  strcpy(mqtt_endpoint, json["mqttConfig"]["endpoint"]);

  //create topic strings
  strcpy(outTopic, dev_id);
  strcat(outTopic, "/outTopic");

  strcpy(inTopic, dev_id);
  strcat(inTopic, "/inTopic");

  strcpy(shadow, "$aws/things/");
  strcat(shadow, dev_id);
  strcat(shadow, "/shadow/update/accepted");

  //used to initialize a device shadow
  strcpy(shadowUpdate, "$aws/things/");
  strcat(shadowUpdate, dev_id);
  strcat(shadowUpdate, "/shadow/update");
  
  return true;
}

void hubless_mqtt_setup() {
  //load the configuration file and use configurations to connect to wifi
  if(!loadConfig()) {
    Serial.println("Failed to mount file system.");
    return;
  }

  //wifi shouldn't be connected yet
  connectedToWifi = 0;

  //set output pins
  pinMode(led, OUTPUT);
  pinMode(switchPin, OUTPUT);

  //set desired state based on reading the output pin
  if(digitalRead(switchPin) == HIGH) {
    strcpy(currentStateStr, "on");
    currentState = HIGH;
  }
  else{
    strcpy(currentStateStr, "off");
    currentState = LOW;
  }

  //start serial monitor output
  Serial.begin(115200);

  //connect to wifi
  setup_wifi();
  
  //set mqtt server
  client.setServer(mqtt_endpoint, mqtt_port);

  //set callback function
  client.setCallback(callback);
}

void hubless_mqtt_loop() {

  if (!client.connected()) {
    connectedToWifi = 0;
    reconnect();
  }
  connectedToWifi = 1;
  client.loop();
  delay(10);

  //demo output
//  long now = millis();  
//  if (now - lastMsg > 2000) {
//    lastMsg = now;
//    ++value;
//    
//    //publish demo message to '<dev_id>/outTopic'
//    snprintf (msg, 75, "hello world #%ld", value);
//    Serial.print(outTopic);
//    Serial.print(" : ");
//    Serial.println(msg);
//    client.publish(outTopic, msg);
//  }
}
