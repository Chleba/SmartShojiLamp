#include "DHTesp.h"
#include "bulb.h"
#include "config.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <Wire.h>

DHTesp dht;

const long utcOffsetInSeconds = 3600;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
RGBBulb bulb;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

#if defined(MQTT_HOME_ASSISTANT_SUPPORT)
StaticJsonDocument<256> staticJsonBuffer;
char jsonBuffer[256] = {0};
#endif

volatile uint8_t cmd = CMD_NOT_DEFINED;

int temp = 0;
int humid = 0;

void PublishToMQTT(const char *p_topic, char *p_payload);

String getTimeNum(int num) {
  String a = "";
  if (num < 10) {
    a += "0";
  }
  a += num;
  return a;
}

void DisplayLines() {
  display.drawLine(64, 0, 64, 40, WHITE);
  display.drawLine(0, 40, 128, 40, WHITE);
}

void DisplayTime() {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(16, 50);

  display.print(getTimeNum(timeClient.getHours()));
  display.print(":");
  display.print(getTimeNum(timeClient.getMinutes()));
  display.print(":");
  display.print(getTimeNum(timeClient.getSeconds()));
}

void DisplayTH() {
  display.setTextSize(3);
  display.setTextColor(WHITE);

  display.setCursor(10, 10);
  display.print(int(temp));
  display.drawCircle(50, 14, 4, WHITE);

  display.setCursor(72, 10);
  display.print(int(humid));
  display.print("%");
}

void DisplayInfo() {
  display.clearDisplay();

  DisplayLines();
  DisplayTime();
  DisplayTH();
  display.display();
}

void ConnectWiFi() {
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Connected with IP:");
  Serial.println(WiFi.localIP());
}

char MQTT_CLIENT_ID[11] = "smartshoji";
#if defined(MQTT_HOME_ASSISTANT_SUPPORT)
char MQTT_CONFIG_TOPIC[sizeof(MQTT_HOME_ASSISTANT_DISCOVERY_PREFIX) +
                       sizeof(MQTT_CLIENT_ID) +
                       sizeof(MQTT_CONFIG_TOPIC_TEMPLATE) - 4] = {0};
#else

#endif

const PROGMEM char *MQTT_STATE_TOPIC = "office/rgb1/light";
const PROGMEM char *MQTT_COMMAND_TOPIC = "office/rgb1/light/set";

const PROGMEM char *MQTT_SENSOR_STATE_TOPIC = "office/sensor1";

volatile unsigned long lastMQTTConnection = MQTT_CONNECTION_TIMEOUT;
/*
   Function called when a MQTT message has arrived
   @param p_topic   The topic of the MQTT message
   @param p_payload The payload of the MQTT message
   @param p_length  The length of the payload
*/
void HandleMQTTMessage(char *p_topic, byte *p_payload, unsigned int p_length) {
  // concatenates the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }

  Serial.print("MQTT message from Topic: ");
  Serial.println(p_topic);
  Serial.print("message: ");
  Serial.println(payload);

  if (String(MQTT_COMMAND_TOPIC).equals(p_topic)) {
    DynamicJsonDocument root(1024);
    auto err = deserializeJson(root, p_payload);
    if (err) {
      return;
    }
    if (root.containsKey("state")) {
      if (strcmp(root["state"], MQTT_STATE_ON_PAYLOAD) == 0) {
        if (bulb.setState(true)) {
          cmd = CMD_STATE_CHANGED;
        }
      } else if (strcmp(root["state"], MQTT_STATE_OFF_PAYLOAD) == 0) {
        if (bulb.setState(false)) {
          cmd = CMD_STATE_CHANGED;
        }
      }
    }

    if (root.containsKey("brightness")) {
      uint8_t b = root["brightness"];
      bulb.setBrightness(b);
      cmd = CMD_STATE_CHANGED;
    }

    if (root.containsKey("color")) {
      uint8_t r = root["color"]["r"];
      uint8_t g = root["color"]["g"];
      uint8_t b = root["color"]["b"];

      if (bulb.setColor(r, g, b)) {
        cmd = CMD_STATE_CHANGED;
      }
    }

    if (root.containsKey("color_temp")) {
      if (bulb.setColorTemperature(root["color_temp"])) {
        cmd = CMD_STATE_CHANGED;
      }
    }
  }
}

/*
  Function called to subscribe to a MQTT topic
*/
void SubscribeToMQTT(const char *p_topic) {
  if (mqttClient.subscribe(p_topic)) {
    Serial.print(F("INFO: Sending the MQTT subscribe succeeded for topic: "));
    Serial.println(p_topic);
  } else {
    Serial.print(F("ERROR: Sending the MQTT subscribe failed for topic: "));
    Serial.println(p_topic);
  }
}

/*
  Function called to publish to a MQTT topic with the given payload
*/
void PublishToMQTT(const char *p_topic, char *p_payload) {
  if (mqttClient.publish(p_topic, p_payload, true)) {
    Serial.print(F("INFO: MQTT message published successfully, topic: "));
    Serial.print(p_topic);
    Serial.print(F(", payload: "));
    Serial.println(p_payload);
  } else {
    Serial.println(F("ERROR: MQTT message not published, either connection "
                     "lost, or message too large. Topic: "));
  }
}

void ConnectToMQTT() {
  if (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT Server..");
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println(
          F("INFO: The client is successfully connected to the MQTT broker"));

      // PublishToMQTT(MQTT_STATE_TOPIC, MQTT_STATE_OFF_PAYLOAD);
      cmd = CMD_STATE_CHANGED;

      SubscribeToMQTT(MQTT_COMMAND_TOPIC);
    } else {
      Serial.println(F("ERROR: The connection to the MQTT broker failed"));
    }
    lastMQTTConnection = millis();
  }
}

void HandleCMD() {
  switch (cmd) {
  case CMD_NOT_DEFINED:
    break;
  case CMD_STATE_CHANGED:
    cmd = CMD_NOT_DEFINED;
    DynamicJsonDocument root(1024);
    root["state"] =
        bulb.getState() ? MQTT_STATE_ON_PAYLOAD : MQTT_STATE_OFF_PAYLOAD;
    root["brightness"] = bulb.getBrightness();
    JsonObject color = root.createNestedObject("color");
    color["r"] = bulb.getColor().red;
    color["g"] = bulb.getColor().green;
    color["b"] = bulb.getColor().blue;
    root["color_temp"] = bulb.getColorTemperature();
    // root.printTo(jsonBuffer, sizeof(jsonBuffer));
    serializeJson(root, jsonBuffer);
    PublishToMQTT(MQTT_STATE_TOPIC, jsonBuffer);
    break;
  }
}

void PublishTempHum() {
  DynamicJsonDocument root(256);
  root["temperature"] = temp;
  root["humidity"] = humid;
  serializeJson(root, jsonBuffer);
  PublishToMQTT(MQTT_SENSOR_STATE_TOPIC, jsonBuffer);
}

void setup() {
  Serial.begin(9600);
  delay(500);

  ConnectWiFi();

  mqttClient.setServer(MQTT_SERVER, MQTT_SERVER_PORT);
  mqttClient.setCallback(HandleMQTTMessage);
  ConnectToMQTT();

  bulb.init();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
  }
  // -- DHT sensor init
  dht.setup(DHTpin, DHTesp::DHT11); // Connect DHT sensor to GPIO 14 (pin 5)

  display.clearDisplay();
  display.display();
  delay(1000);

  cmd = CMD_STATE_CHANGED;
}

void loop() {
  timeClient.update();
  ConnectToMQTT();
  mqttClient.loop();
  HandleCMD();
  bulb.loop();

  float t = dht.getTemperature();
  if (!isnan(t)) {
    if(int(t) != int(temp)){
      PublishTempHum();
    }
    temp = t;
  }
  float h = dht.getHumidity();
  if (!isnan(h)) {
    if(int(h) != int(humid)){
      PublishTempHum();
    }
    humid = h;
  }

  DisplayInfo();

  delay(1000);
}
