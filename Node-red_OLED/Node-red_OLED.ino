//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "dw_font.h"
#include "SSD1306.h"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4



//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "iPhone ของ Pon";
const char* password = "ponBamkyrr";

const char* mqtt_server = "172.20.10.5";

WiFiClient espClient;
PubSubClient client(espClient);


SSD1306   display(0x3c, 21, 22);


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


  if (not strcmp(topic , "OLED")) {
    Serial.println(messageTemp);
    display.clear();
    dw_font_setfont(&myfont, &font_th_sarabunpsk_regular40);
    dw_font_goto(&myfont, 10, 40);
    dw_font_print(&myfont,(char*) messageTemp.c_str());
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

  client.subscribe("OLED");
  
}


void setup() {
  // put your setup code here, to run once:

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

}
