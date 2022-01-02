# SmartShojiLamp
Smart Shoji Lamp with temperature sensor, LED matrix &amp; display

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
