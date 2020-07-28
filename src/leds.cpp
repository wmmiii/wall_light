#include "leds.h"

#define DATA_PIN 14
#define CLOCK_PIN 12

#define WIDTH 13
#define HEIGHT 13

#define CENTER_X 6
#define CENTER_Y 5.5

#define NUM_LEDS (WIDTH * HEIGHT)

#define NUM_DROPLETS 20
#define DROPLET_FALL_DIST (HEIGHT * 8)

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

static inline uint8_t index(uint8_t x, uint8_t y) {
  uint8_t index = y * WIDTH;
  if (y % 2) {
    index += (WIDTH - x) - 1;
  } else {
    index += x;
  }
  return index;
}

static inline uint8_t random(uint8_t seed) {
  return (1664525 * seed + 1013904223);
}

static CHSV getColor(uint32_t t) {
  if (config.cycle) {
    return CHSV((t / 32) % 256, 255, 96);
  } else {
    return config.base_color;
  }
}

static void breathe(uint32_t t,
    CRGBArray<NUM_LEDS> leds,
    Configuration config) {
  for(int i = 0; i < NUM_LEDS; i++) {
    int8_t pX = x(i);
    int8_t pY = y(i);

    double w = WIDTH / 2.0;
    double h = HEIGHT / 2.0;
    double cX = (pX - w + 0.5) / w;
    double cY = (pY - h + 0.5) / h;

    double dist = sqrt(cX * cX + cY * cY);
    double brightness = sin((dist * 2.0 + -t / 128.0));
    uint8_t value = max(brightness * 128.0 - 32.0, 0.0);

    CHSV color = getColor(t);
    color.value = value;

    leds[i] = color;
  }
}

static void rain(uint32_t time,
    CRGBArray<NUM_LEDS> leds,
    Configuration config) {
  double t = time / 16.0;

  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i].fadeToBlackBy(255);
  }

  CHSV color(getColor(time));

  for (int d = 0; d < NUM_DROPLETS; ++d) {
    uint8_t x = random(d * 27 + 5) % WIDTH;
    uint8_t speed = random(d * 5 + 10) % 3 + 1;
    double y = t * speed + floor(random(d * 13 + 20));
    uint32_t y_floor = floor(y);

    double diff = y - y_floor;
    
    color.value = max(diff * 96.0, 0.0);
    if (y_floor % DROPLET_FALL_DIST < HEIGHT) {
      leds[index(x, y_floor % DROPLET_FALL_DIST)] = color;
    }

    uint8_t TAIL_LENGTH = 5;
    for (int i = 0; i < TAIL_LENGTH; ++i) {
      double amount = 1.0 - (i + diff) / TAIL_LENGTH;
      uint32_t dY = y_floor - i - 1;
      color.value = max(amount * 96.0, 0.0);
      if (dY % DROPLET_FALL_DIST < HEIGHT) {
        leds[index(x, dY % DROPLET_FALL_DIST)] = color;
      }
    }
  }
}

void setup() {
  FastLED.delay(16);
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
}

void loop(){
  const uint32_t t = millis() / 8;
  rain(t, leds, config);
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
