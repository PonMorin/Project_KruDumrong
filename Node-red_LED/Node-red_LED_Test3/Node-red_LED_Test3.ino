#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char*ssid = "iPhone ของ Pon";
const char*password = "ponBamkyrr";

const char*mqttServer = "172.20.10.5";


WiFiClient espClient;
PubSubClient client(espClient);

const int LEDPIN = 12;

long now = millis();
long lastMeasure = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
  
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while(!client.connected()){
    Serial.println("Connecting to MQTT");
    if(client.connect("ESP8266Client")){
      Serial.println("connected");
    }
    else{
      Serial.print("failed with state");
      Serial.print(client.state());
      delay(2000);
    }
    
  }
//
//  client.publish("esp/led","Hello from ESP8266");
//
//  client.subscribe("esp/led");
}

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
  if(topic=="esp/led"){
      Serial.print("Changing Room lamp to ");
      if(messageTemp == "on"){
        digitalWrite(LEDPIN, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "off"){
        digitalWrite(LEDPIN, LOW);
        Serial.print("Off");
      }
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (! client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    /*
     YOU MIGHT NEED TO CHANGE THIS LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a new name to the ESP8266.
     Here's how it looks:
       if (client.connect("ESP8266Client")) {
     You can do it like this:
       if (client.connect("ESP1_Office")) {
     Then, for the other ESP:
       if (client.connect("ESP2_Garage")) {
      That should solve your MQTT multiple connections problem
    */
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
    client.subscribe("esp/led");
}


void loop() {
  // put your main code here, to run repeatedly:
if (!client.connected()) {
    reconnect();
  }
  client.subscribe("esp/led");

  if(client.loop())
    client.connect("ESP8266Client");
  

}
