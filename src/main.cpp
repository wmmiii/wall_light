#include "./leds.h"
#include "./wifi.h"

#include <FastLED.h>

void setup() {
  Serial.begin(9600);
  Serial.println("Wifi Setup...");
  long time = wifi::setup();
  Serial.print("Current Time: ");
  Serial.println(time);
  Serial.println("LED Setup...");
  led::setup(time);
  Serial.println("Setup complete.");
}

void loop(){
  led::loop();
  wifi::loop();
}
