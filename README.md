# SmartShojiLamp
Smart Shoji Lamp with temperature sensor, LED matrix &amp; 0.97" display.
Smart Shoji Lamp have MQTT implemented for Home Assistant JSON light protocol.

## Home Assistant configuration.yaml
```
light:
  platform: mqtt
  name: 'Smart Shoji'
  schema: json
  state_topic: 'office/rgb1/light'
  command_topic: 'office/rgb1/light/set'
  brightness: true
  rgb: true

sensor:
  - platform: mqtt
    name: 'Temperature'
    state_topic: 'office/sensor1'
    unit_of_measurement: 'C'
    value_template: '{{ value_json.temperature }}'
  - platform: mqtt
    name: 'Humidity'
    state_topic: 'office/sensor1'
    unit_of_measurement: '%'
    value_template: '{{ value_json.humidity }}'
```

## Used HW
* NodeMCU ESP8266 dev board (or any esp8266)
* DHT11 module sensor (or DHT22)
* LCD Matrix 8x8 5V
* MicroUSB PCB to connect power to NodeMCU & LED Matrix
* SSD1306 0.97" Monochromatic Display 124x64px

## PINOUT
(D Pins is used for NodeMCU)
### SSD1306 Display
* D1 -> SCL
* D2 -> SDA
* GND & VCC -> NodeMCU GND & 3.3V
### DHT11 Sensor
* D6 -> DATA IN
* GND & VCC -> NodeMCU GND & 3.3V
### LED Matrix 8x8
* D5 -> DATA IN
* GND & VCC -> USB PCB GND & 5V
