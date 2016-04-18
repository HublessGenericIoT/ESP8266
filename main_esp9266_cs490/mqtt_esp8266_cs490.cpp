#include "mqtt_esp8266_cs490.h"

// Update these with values suitable for your network.
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const char* dev_id;

const char* sta_ssid;
const char* sta_pass;
const char* sta_auth;

const char* mqtt_user;
const char* mqtt_pass;
const char* mqtt_endpoint;

int connectedToWifi;

String deviceLastStatus = "";

const int led = 2;

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
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  delay(10);
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
      char outTopic[128];
      char outMsg[128];
      snprintf(outTopic, 128, "%s/outTopic", dev_id);
      snprintf(outMsg, 128, "%s is connected!", dev_id);
      
      client.publish(outTopic, outMsg);
      
      // ... and resubscribe
      char subMe[128];
      snprintf(subMe, 128, "proxy/%s/inTopic");

      char subShadow[128];
      snprintf(subShadow, 128, "$aws/things/%s/shadow/update/accepted");
      
      client.subscribe(subMe);
      client.subscribe(subShadow);
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
  dev_id   = json["id"];
  
  //wifi info
  sta_ssid  = json["wifiConfig"]["ssid"];
  sta_pass  = json["wifiConfig"]["password"];
  sta_auth  = json["wifiConfig"]["authmode"];
  
  //mqtt info
  mqtt_user = json["mqttConfig"]["user"];
  mqtt_pass = json["mqttConfig"]["password"];
  mqtt_endpoint = json["mqttConfig"]["endpoint"];
  
  return true;
}

void hubless_mqtt_setup() {
  //load the configuration file and use configurations to connect to wifi
  if(!loadConfig()) {
    Serial.println("Failed to mount file system.");
    return;
  }
  
  connectedToWifi = 0;
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  //connect to wifi
  setup_wifi();
  
  //set mqtt server
  client.setServer(mqtt_endpoint, MQTT_PORT);

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

//  check_shadow();

//  long now = millis();  
//  if (now - lastMsg > 2000) {
//    lastMsg = now;
//    ++value;
//    snprintf (msg, 75, "hello world #%ld", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);
//
//    //check and update device shadow
//    client.publish("esp_8266/outTopic", msg);
//  }
}
