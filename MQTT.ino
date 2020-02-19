#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <PubSubClient.h>

#define lm35 A0

float rawvalue = 0;
float milivolt = 0;
float celcius = 0;

const char* ssid = "MI A1";
const char* password = "angga010899";

const char* mqtt_server = "m12.cloudmqtt.com";

const char *mqtt_user = "jzdgxkrk";
const char *mqtt_pass = "8oJUquTPzj0X"; 

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMillis = 0;
char celc[4];

void setup_wifi() {
  delay(100);
  //connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
 
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // ... and resubscribe
      delay(1000);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup() {
  Serial.begin(115200);
  pinMode(lm35, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, 10668);
  client.setCallback(callback);
  reconnect();
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  rawvalue = analogRead(lm35);
  milivolt = (rawvalue / 2048.0) * 3300;
  celcius = milivolt * 0.1;
  gcvt(celcius,4,celc);
  client.publish("test1",celc);
  Serial.println(celcius);
  client.loop();
  delay(1000);
  
}
