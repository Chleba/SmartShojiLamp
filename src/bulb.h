#pragma once

// #include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
// #include "config.h"

#include <Adafruit_NeoMatrix.h>
#include "config.h"

#define COLOR_TEMP_HA_MIN_IN_MIRED   154    // Home Assistant minimum color temperature
#define COLOR_TEMP_HA_MAX_IN_MIRED   500    // Home Assistant maximum color temperature
#define COLOR_TEMP_MIN_IN_KELVIN     1000   // minimum color temperature
#define COLOR_TEMP_MAX_IN_KELVIN     15000  // maximum color temperature

struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

enum CMD {
  CMD_NOT_DEFINED,
  CMD_STATE_CHANGED,
};

class RGBBulb {
  public:
    RGBBulb(void);
    
    void      init(void);
    void      loop(void);
    
    bool      getState(void);
    bool      setState(bool p_state);
    
    uint8_t   getBrightness(void);
    bool      setBrightness(uint8_t p_brightness);
    
    Color     getColor(void);
    bool      setColor(uint8_t p_red, uint8_t p_green, uint8_t p_blue);
    
    uint16_t  getColorTemperature(void);
    bool      setColorTemperature(uint16_t p_colorTemperature);

  private:
    bool      m_state;    
    uint8_t   m_brightness;
    Color     m_color;
    uint16_t  m_colorTemperature;

    Adafruit_NeoMatrix *matrix;
    
    void      initMatrix();
    bool      setColor();
};

