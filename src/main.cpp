/**
 Using the Basic MQTT example from Joël Gähwiler's MQTT Library and added
 some lines to read the data from the DHT11 using the Library from Adafruit
*/
#include <Arduino.h>

#include <DHT.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

const char ssid[] = "<SSID>";
const char pass[] = "<PW>";

WiFiClientSecure net;
MQTTClient client;

#define DHTPIN D5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("<DeviceId>", "<CredentialId>@<Tenant>", "<Password>")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
}


void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);

  dht.begin();
  // Sadly the MQTT Library doesn't have DNS support yet. So I used the static IP.
  client.begin("52.29.250.153", 8883, net);

  connect();
}

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    // Reading temperature and humidity
    int h = dht.readHumidity();
    // Read temperature as Celsius
    int t = dht.readTemperature();
    char msg[130];
    sprintf(msg, "{\"temperature\":%d, \"humidity\": %d }", t,h);
    Serial.println(msg);
    client.publish("telemetry", msg);
  }
}
