//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DHTTYPE DHT22
#define DHTPIN 4


const char* ssid = "iPhone ของ Pon";
const char* password = "ponBamkyrr";

const char* mqtt_server = "172.20.10.5";

WiFiClient espClient;
PubSubClient client(espClient);


const int DHT_PIN = 2;

static char celsiusTemp[7];
static char humidity[7];
static char fahrenheitTemp[7];

DHT dht(DHTPIN, DHTTYPE);


long now = millis();
long lastMeasure = 0;


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;


  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

 
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  client.subscribe("DHT");

}



void DHT_READ() {

  now = millis();

  if (now - lastMeasure > 3000) {
    lastMeasure = now;

    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hic = dht.computeHeatIndex(f, h);
    dtostrf(hic, 6, 2, fahrenheitTemp);
    // Compute heat index in Celsius (isFahreheit = false)
    float hif = dht.computeHeatIndex(t, h, false);
    dtostrf(hif, 6, 2, celsiusTemp);
    dtostrf(h, 6, 2, humidity);


    // Publishes Temperature and Humidity values
    client.publish("room/temperature", celsiusTemp);
    //Serial.print("temp = ");
    //  Serial.println(Temperature);
    client.publish("room/humidity", humidity);
    client.publish("room/fahrenheit", fahrenheitTemp);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_wifi();
  dht.begin();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
    
  }
  if (!client.loop()) {
    client.connect("ESP8266Client");
    
  }

  DHT_READ();
}
