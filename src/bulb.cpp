#include "bulb.h"
// #include <Adafruit_NeoMatrix.h>
// #include "config.h"

///////////////////////////////////////////////////////////////////////////
//   CONSTRUCTOR, INIT() AND LOOP()
///////////////////////////////////////////////////////////////////////////
RGBBulb::RGBBulb(void) {
  matrix = new Adafruit_NeoMatrix(
    8, 8, MATRIX_PIN,
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
    NEO_GRB + NEO_KHZ800);
}

void RGBBulb::init(void) {
  m_state = false;
  m_brightness = 60;
  m_color.red = 0;
  m_color.green = 0;
  m_color.blue = 0;

  initMatrix();
}

void RGBBulb::initMatrix(void) {
  matrix->begin();
  matrix->setBrightness(m_brightness);
  matrix->fillScreen(matrix->Color(m_color.red, m_color.green, m_color.blue));
  matrix->show();
  delay(1000);
  matrix->clear();
}

void RGBBulb::loop(void) {

}

///////////////////////////////////////////////////////////////////////////
//   STATE
///////////////////////////////////////////////////////////////////////////
bool RGBBulb::getState(void) {
  return m_state;
}

bool RGBBulb::setState(bool p_state) {
  // checks if the given state is different from the actual state
  if (p_state == m_state)
    return false;
  
  if (p_state) {
    m_state = true;
    setColor();
  } else {
    m_state = false;
    matrix->clear();
  }
  matrix->show();

  return true;
}

///////////////////////////////////////////////////////////////////////////
//   BRIGHTNESS
///////////////////////////////////////////////////////////////////////////
uint8_t RGBBulb::getBrightness(void) {
  return m_brightness;
}

bool RGBBulb::setBrightness(uint8_t p_brightness) {
  // checks if the value is smaller, bigger or equal to the actual brightness value
  if (p_brightness < 0 || p_brightness > 255 || p_brightness == m_brightness)
    return false;

  // saves the new brightness value
  m_brightness = p_brightness;

  matrix->setBrightness(m_brightness);

  return setColor();
}

///////////////////////////////////////////////////////////////////////////
//   RGB COLOR
///////////////////////////////////////////////////////////////////////////
Color RGBBulb::getColor(void) {
  return m_color;
}

bool RGBBulb::setColor(uint8_t p_red, uint8_t p_green, uint8_t p_blue) {
  if ((p_red < 0 || p_red > 255) || (p_green < 0 || p_green > 255) || (p_blue < 0 || p_blue > 255))
    return false;

  // saves the new values
  m_color.red = p_red;
  m_color.green = p_green;
  m_color.blue = p_blue;

  return setColor();
}

bool RGBBulb::setColor() {
  // sets the new color
  matrix->fillScreen(matrix->Color(m_color.red, m_color.green, m_color.blue));
  matrix->show();

  return true;
}

///////////////////////////////////////////////////////////////////////////
//   COLOR TEMPERATURE
///////////////////////////////////////////////////////////////////////////
uint16_t RGBBulb::getColorTemperature(void) {
  return m_colorTemperature;
}

bool RGBBulb::setColorTemperature(uint16_t p_colorTemperature) {
  // checks if the value is equal to the actual color temperature
  if (p_colorTemperature < COLOR_TEMP_HA_MIN_IN_MIRED || p_colorTemperature == m_colorTemperature || p_colorTemperature > COLOR_TEMP_HA_MAX_IN_MIRED)
    return false;

  // saves the new colour temperature
  m_colorTemperature = p_colorTemperature;

  // https://fr.wikipedia.org/wiki/Mired
  // http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
  // M = 1000000 / T <> T [kelvin] = 1000000 / M [mired]
      int tmpKelvin = 1000000 / m_colorTemperature;
  
      if (tmpKelvin < 1000) {
        tmpKelvin = 1000;
      } else if (tmpKelvin > 40000) {
        tmpKelvin = 40000;
      }

  //int tmpKelvin = map(p_colorTemperature, COLOR_TEMP_HA_MIN_IN_MIRED, COLOR_TEMP_HA_MAX_IN_MIRED, COLOR_TEMP_MAX_IN_KELVIN, COLOR_TEMP_MIN_IN_KELVIN);
  tmpKelvin = tmpKelvin / 100;

  // computes red
  if (tmpKelvin <= 66) {
    m_color.red = 255;
  } else {
    float red = tmpKelvin - 60;
    red = 329.698727446 * pow(red, -0.1332047592);
    if (red < 0) {
      m_color.red = 0;
    } else if (red > 255) {
      m_color.red = 255;
    } else {
      m_color.red = red;
    }
  }

  // computes green
  if (tmpKelvin <= 66) {
    float green = tmpKelvin;
    green = 99.4708025861 * log(green) - 161.1195681661;
    if (green < 0) {
      m_color.green = 0;
    } else if (green > 255) {
      m_color.green = 255;
    } else {
      m_color.green = green;
    }
  } else {
    float green = tmpKelvin - 60;
    green = 288.1221695283 * pow(green, -0.0755148492);
    if (green < 0) {
      m_color.green = 0;
    } else if (green > 255) {
      m_color.green = 255;
    } else {
      m_color.green = green;
    }
  }

  // computes blue
  if (tmpKelvin <= 66) {
    m_color.blue = 255;
  } else {
    if (tmpKelvin <= 19) {
      m_color.blue = 0;
    } else {
      float blue = tmpKelvin - 10;
      blue = 138.5177312231 * log(blue) - 305.0447927307;
      if (blue < 0) {
        m_color.blue = 0;
      } else if (blue > 255) {
        m_color.blue = 255;
      } else {
        m_color.blue = blue;
      }
    }
  }
  
  return setColor();
}

