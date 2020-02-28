#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <PubSubClient.h>

#define lm35 A0
#define lamp D6

float rawvalue = 0;
float milivolt = 0;
float celcius = 0;

const char* ssid = "...."; //ssid network
const char* password = "..."; //password network

const char* mqtt_server = "..."; //server cloud
int port = .....; //port cloud network

const char *mqtt_user = "...."; //username cloud
const char *mqtt_pass = "...."; //password cloud

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
  String messageTemp;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp = (char)payload[i];
  }
  if(messageTemp == "1"){
    digitalWrite(...., HIGH); //topic mqtt out
    Serial.print("On");
  }
  else if(messageTemp == "0"){
    digitalWrite(lamp, LOW);//topic mqtt out
    Serial.print("Off");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX); //Client ID
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("ang");
      Serial.println(clientId);
      //delay(1000);
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
  pinMode(lamp, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, port);
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
  client.publish("test1",celc); //topic lm35
  Serial.println(celcius);
  client.loop();
  delay(1000);
  
}
