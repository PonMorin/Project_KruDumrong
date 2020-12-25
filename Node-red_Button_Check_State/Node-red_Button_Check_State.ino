#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "iPhone ของ Pon";
const char* password = "ponBamkyrr";

const char* mqtt_server = "172.20.10.5";

WiFiClient espClient;
PubSubClient client(espClient);

const int buttonPin = 16;
int buttonState = 0;
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

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


  client.subscribe("Button");
  
}

void Button(){
  int reading = digitalRead(buttonPin);
  if(reading != lastButtonState){
    lastButtonState = millis();
  }

  if((millis() - lastDebounceTime) > debounceDelay){
     if(reading != buttonState){
        client.publish("Button" , reading? "ON":"OFF");
        buttonState = reading;
    }
  }
}


void setup() {
  // put your setup code here, to run once:
  buttonState = digitalRead(buttonPin);
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
  setup_wifi();
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

  Button();
}
