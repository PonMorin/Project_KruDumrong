//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "dw_font.h"
#include "SSD1306.h"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4

#define DHTTYPE DHT22
#define DHTPIN 4


//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "iPhone ของ Pon";
const char* password = "ponBamkyrr";

const char* mqtt_server = "172.20.10.5";

WiFiClient espClient;
PubSubClient client(espClient);


SSD1306   display(0x3c, 21, 22);

const int LED = 12;
const int LED2 = 26;
const int LED3 = 32;
const int DHT_PIN = 2;

const int pir = 25;

const int buttonPin = 16;
int buttonState = 0;
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

static char celsiusTemp[7];
static char humidity[7];
static char fahrenheitTemp[7];

DHT dht(DHTPIN, DHTTYPE);


long now = millis();
long lastMeasure = 0;


extern dw_font_info_t font_th_sarabunpsk_regular40;
dw_font_t myfont;

void draw_pixel(int16_t x, int16_t y)
{
  display.setColor(WHITE);
  display.setPixel(x, y);
}

void clear_pixel(int16_t x, int16_t y)
{
  display.setColor(BLACK);
  display.setPixel(x, y);
}


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

  if (messageTemp == "GET") {
    client.publish("LED/ON-OFF", (digitalRead(LED) ? "LEDON" : "LEDOFF"));
    Serial.println("Send !");
    return;
  }

  //if (topic == "LED/ON-OFF") {
    //Serial.print("Changing Room lamp to ");
    if (messageTemp == "on") {
      digitalWrite(LED, HIGH);
      Serial.print("On");
    }
    else if (messageTemp == "off") {
      digitalWrite(LED, LOW);
      Serial.print("Off");
    }

    else if (isNumeric(messageTemp)) {
      int x = messageTemp.toInt();
      ledcWrite(ledChannel, x);
    }

    Serial.println();
    


  if (not strcmp(topic , "OLED")) {
    Serial.println(messageTemp);
//    display.clearDisplay();
//    display.setTextSize(2);
//    display.setTextColor(WHITE);
//    display.setCursor(0, 0);
    display.clear();
    dw_font_setfont(&myfont, &font_th_sarabunpsk_regular40);
    dw_font_goto(&myfont, 10, 40);
    dw_font_print(&myfont,(char*) messageTemp.c_str());
    //display.print(messageTemp);
    display.display();
  }

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


  client.subscribe("LED/ON-OFF");
  client.subscribe("DHT");
  client.subscribe("OLED");
  client.subscribe("Button");
  
}

boolean isNumeric(String str) {
  unsigned int stringLength = str.length();

  if (stringLength == 0) {
    return false;
  }

  boolean seenDecimal = false;

  for (unsigned int i = 0; i < stringLength; ++i) {
    if (isDigit(str.charAt(i))) {
      continue;
    }

    if (str.charAt(i) == '.') {
      if (seenDecimal) {
        return false;
      }
      seenDecimal = true;
      continue;
    }
    return false;
  }
  return true;
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

void Motion_Sensor() {
  int val = digitalRead(pir);
  if (val == HIGH){
      digitalWrite(LED3, HIGH);
      Serial.print("Detected");
    }

  else{
    digitalWrite(LED3,LOW);
  }
  
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
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(pir, INPUT);

  buttonState = digitalRead(buttonPin);
  pinMode(buttonPin, INPUT);
  
  
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(LED2, ledChannel);

//  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
//    Serial.println(F("SSD1306 allocation failed"));
//    for (;;);
//  }
  
  display.init();
  display.flipScreenVertically();

  dw_font_init(&myfont,
               128,
               64,
               draw_pixel,
               clear_pixel);

  dw_font_setfont(&myfont, &font_th_sarabunpsk_regular40);
  dw_font_goto(&myfont, 10, 40);
//    dw_font_goto(&myfont, 10, 60);
  dw_font_print(&myfont, "สวัสดีAB");
  display.display();

//  display.clearDisplay();
 // display.setTextColor(WHITE);

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
  Button();
}
