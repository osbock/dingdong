/*
 WiFi Doorbell --
 Simultaneously ring wireless doorbell, and wired, also publishing messages about button presses.

*/
#include <SPIFFS.h>
#include <WiFiSettings.h>
#include <RCSwitch.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define BUTTON 12
#define RELAY 14
#define DOORBELLCODE 10514898
RCSwitch mySwitch = RCSwitch();
WiFiClient espClient;
PubSubClient client(espClient);
char mqtt_server[] ="xxx.xxx.xxx.xxx";
void setup() {

  Serial.begin(115200);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(RELAY,OUTPUT);
  SPIFFS.begin(true);  // Will format on the first run after failing to mount

  // Use stored credentials to connect to your WiFi access point.
  // If no credentials are stored or if the access point is out of reach,
  // an access point will be started with a captive portal to configure WiFi.
  WiFiSettings.connect();
  
  // Transmitter is connected to Arduino Pin #5
  mySwitch.enableTransmit(18);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  
}

// gotta make this so it doesn't hang for 5 seconds waiting to reconnect if the server is down.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // ... and resubscribe
      client.subscribe("dingdong.ring");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  if (digitalRead(BUTTON) == 0){
    ringBells();
    client.publish("dingdong.button", "1");
    delay(1000);
  }
  if (Serial.available() >0){
    char c= Serial.read();
    if( c == 'r'){
      ringBells();
      delay(1000);
    }
  }
    if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
void ringBells(){
  Serial.println("ring-a-ling");
  mySwitch.send(DOORBELLCODE, 24);
  digitalWrite(RELAY,HIGH);
  delay(500);
  digitalWrite(RELAY,LOW);
  
}
