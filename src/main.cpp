#include "./leds.h"
#include "./wifi.h"

#include <FastLED.h>

void setup() {
  Serial.begin(9600);
  Serial.print("LED Setup...");
  led::setup();
  Serial.print("Wifi Setup...");
  wifi::setup();
  Serial.print("Setup complete.");
}

void loop(){
  led::loop();
  wifi::loop();
}
