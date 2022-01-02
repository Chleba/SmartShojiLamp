#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN            8
#define NUMPIXELS      64
#define BUTTON_PIN   A4
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int largest[ ] = {7,6,5,4,3,2,1,0,8,16,24,32,40,48,56,57,58,59,60,61,62,63,55,47,39,31,23,15};
int large[]={14,13,12,11,10,9,17,25,33,41,49,50,51,52,53,54,46,38,30,22};
int medium [] = {21,20,19,18,26,34,42,43,44,45,37,29};
int small [] = {28,27,35,36};
int full [] = {7,6,5,4,3,2,1,0,8,16,24,32,40,48,56,57,58,59,60,61,62,63,55,47,39,31,23,15,14,13,12,11,10,9,17,25,33,41,49,50,51,52,53,54,46,38,30,22,21,20,19,18,26,34,42,43,44,45,37,29,28,27,35,36};
int delayval = 25;
bool oldState = HIGH;
int showType = 0;

void setup() {
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
pixels.begin();
pixels.show();
pixels.setBrightness(100);
}

void loop() {
  
bool newState = digitalRead(BUTTON_PIN);
if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(BUTTON_PIN);
    if (newState == LOW) {
      showType++;
      if (showType > 5)
        showType=0;
      startShow(showType);
    }
  }
  oldState = newState;
}
void startShow(int i) {
  switch(i){
    case 0: 
    for (int i : full){
    pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
    }
    break;
    
    case 1: 
    
    for (int i : small){
    pixels.setPixelColor(i, pixels.Color(243, 5, 0)); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
    }
    break;
    
    case 2: 
    
    for (int i : medium){
    pixels.setPixelColor(i, pixels.Color(255, 30, 0)); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
    }
    break;
    
    case 3: 
    
    for (int i : large){
    pixels.setPixelColor(i, pixels.Color(255,50,0)); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
    }
    break;
    
    case 4:  
    
    for (int i : largest){
    pixels.setPixelColor(i, pixels.Color(255,69,0)); 
    pixels.show(); 
    delay(delayval);
    }
    break;

    case 5:  
    for (int i : full){
    pixels.setPixelColor(i, pixels.Color(255,255,255)); 
    pixels.show(); 
    delay(delayval); 
    }
    break;
  }
}

