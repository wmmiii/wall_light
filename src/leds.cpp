#include "leds.h"

#define DATA_PIN 14
#define CLOCK_PIN 12

#define WIDTH 13
#define HEIGHT 13

#define CENTER_X 6
#define CENTER_Y 5.5

#define NUM_LEDS (WIDTH * HEIGHT)

namespace led {

CRGBArray<NUM_LEDS> leds;
Configuration config;

static inline uint8_t x(int index) {
  if (index / WIDTH % 2) {
    return (WIDTH - 1) - index % WIDTH;
  } else {
    return index % WIDTH;
  }
}

static inline uint8_t y(int index) {
  return index / WIDTH;
}

static void breathe(uint32_t t,
    CRGBArray<NUM_LEDS> leds,
    uint8_t i,
    uint8_t pX,
    uint8_t pY,
    Configuration config) {
  double w = WIDTH / 2.0;
  double h = HEIGHT / 2.0;
  double cX = (pX - w + 0.5) / w;
  double cY = (pY - h + 0.5) / h;

  double dist = sqrt(cX * cX + cY * cY);
  double brightness = sin((dist * 2.0 + -t / 128.0));
  uint8_t value = max(brightness * 128.0 - 32.0, 0.0);

  CHSV color;
  if (config.cycle) {
    color = CHSV((t / 32) % 256, 255, value);
  } else {
    color = config.base_color;
    color.value = value;
  }

  leds[i] = color;
}

void setup() {
  FastLED.delay(16);
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
}

void loop(){
  const uint32_t t = millis() / 8;

  for(int i = 0; i < NUM_LEDS; i++) {
    int8_t pX = x(i);
    int8_t pY = y(i);

    breathe(t, leds, i, pX, pY, config);
  }
  FastLED.delay(8);
}


void set_cycle() {
  config.base_color = CHSV(0, 0, 0);
  config.cycle = true;
}

void set_hue(uint8_t hue) {
  config.base_color = CHSV(hue, 255, 0);
  config.cycle = false;
}

} // namespace led
