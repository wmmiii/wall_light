#include "./leds.h"
#include "./wifi.h"
#include "./ha.h"

#include <FastLED.h>

void setup() {
  Serial.begin(9600);
  Serial.println("Wifi Setup...");
  long time = wifi::setup();
  Serial.print("Current Time: ");
  Serial.println(time);
  Serial.println("LED Setup...");
  led::setup(time);
  Serial.println("Home Assistant Setup...");
  home_assistant::setup();
  Serial.println("Setup complete.");
}

void loop(){
  led::loop();
  wifi::loop();
  home_assistant::loop();
}
