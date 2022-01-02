#pragma once

#define DHTpin D6
#define MATRIX_PIN D5

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define FRIENDLY_NAME "Smart Shoji Lamp"

// -- WiFi
#define WIFI_SSID     ""
#define WIFI_PASSWORD ""

///////////////////////////////////////////////////////////////////////////
//   MQTT
///////////////////////////////////////////////////////////////////////////
#define MQTT_USERNAME     ""
#define MQTT_PASSWORD     ""
#define MQTT_SERVER       ""
#define MQTT_SERVER_PORT  1883

#define MQTT_HOME_ASSISTANT_SUPPORT

#if defined(MQTT_HOME_ASSISTANT_SUPPORT)
// template: <discovery prefix>/light/<chip ID>/config, status, state or set
#define MQTT_CONFIG_TOPIC_TEMPLATE  "%s/light/%s/config"
#else

#endif

#define MQTT_HOME_ASSISTANT_DISCOVERY_PREFIX  "homeassistant"
#define MQTT_STATE_ON_PAYLOAD   "ON"
#define MQTT_STATE_OFF_PAYLOAD  "OFF"

#define MQTT_CONNECTION_TIMEOUT 5000

