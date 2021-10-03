#include "config.h"
#include "leds.h"

#define BOARD_CURRENT_MA 200
#define MAX_DRAW_MA 2000

#define DATA_PIN 14
#define CLOCK_PIN 12

#define DEFAULT_BRIGHTNESS 96

#if defined BOX
const uint8_t WIDTH = 13;
const uint8_t HEIGHT = 13;
const uint8_t CENTER_X = 6;
const uint8_t CENTER_Y = 5.5;
#elif defined BED
const uint8_t WIDTH = 83;
const uint8_t HEIGHT = 3;
const uint8_t CENTER_X = 41;
const uint8_t CENTER_Y = 1;
#endif

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

static void printColor(CRGB color) {
  Serial.print("R: ");
  Serial.print(color.r);
  Serial.print("\tG: ");
  Serial.print(color.g);
  Serial.print("\tB: ");
  Serial.println(color.b);
}

static CHSV getColor(uint32_t t) {
  if (config.cycle) {
    return CHSV((t / 32) % 256, 255, DEFAULT_BRIGHTNESS);
  } else {
    return config.base_color;
  }
}
static void steady(uint32_t t, CRGBArray<NUM_LEDS> leds) {
  CHSV color = getColor(t);
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
}

static void breathe(uint32_t t, CRGBArray<NUM_LEDS> leds) {
  for(int i = 0; i < NUM_LEDS; i++) {
    int8_t pX = x(i);
    int8_t pY = y(i);

    double w = WIDTH / 2.0;
    double h = HEIGHT / 2.0;
    double cX = (pX - w + 0.5) / w;
    double cY = (pY - h + 0.5) / h;

    double dist = sqrt(cX * cX + cY * cY);
    double brightness = sin((dist * 2.0 + -t / 128.0));
    uint8_t value = max(brightness * 255.0, 0.0);

    CHSV color = getColor(t);
    color.value = value;

    leds[i] = color;
  }
}

static void rain(uint32_t time, CRGBArray<NUM_LEDS> leds) {
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
    
    color.value = max(diff * DEFAULT_BRIGHTNESS, 0.0);
    if (y_floor % DROPLET_FALL_DIST < HEIGHT) {
      leds[index(x, y_floor % DROPLET_FALL_DIST)] = color;
    }

    uint8_t TAIL_LENGTH = 5;
    for (int i = 0; i < TAIL_LENGTH; ++i) {
      double amount = 1.0 - (i + diff) / TAIL_LENGTH;
      uint32_t dY = y_floor - i - 1;
      color.value = max(amount * DEFAULT_BRIGHTNESS, 0.0);
      if (dY % DROPLET_FALL_DIST < HEIGHT) {
        leds[index(x, dY % DROPLET_FALL_DIST)] = color;
      }
    }
  }
}

static void gradient(uint32_t t, CRGBArray<NUM_LEDS> leds) {
  const int16_t DIFFERENCE = 20;
  CHSV color = getColor(t);
  uint8_t hue = color.hue;
  uint8_t high = (hue + DIFFERENCE) % 256;
  uint8_t low = (hue - DIFFERENCE) % 256;
  CHSVPalette256 palette(
      CHSV(low, 255, DEFAULT_BRIGHTNESS),
      CHSV(high, 255, DEFAULT_BRIGHTNESS),
      CHSV(low, 255, DEFAULT_BRIGHTNESS));
  for(int i = 0; i < NUM_LEDS; i++) {
    int64_t pX = (x(i) << 3) + (t >> 1);

    leds[i] = palette[(uint8_t)(pX % 256)];
  }
}

static void rainbow(uint32_t t, CRGBArray<NUM_LEDS> leds) {
  for(int i = 0; i < NUM_LEDS; i++) {
    uint64_t pX = (x(i) << 8) / WIDTH + t;

    leds[i] = CHSV(pX % 256, 255, DEFAULT_BRIGHTNESS);
  }
}

static void spiral(uint32_t t, CRGBArray<NUM_LEDS> leds) {
  for(int i = 0; i < NUM_LEDS; i++) {
    int8_t pX = x(i);
    int8_t pY = y(i);

    double w = WIDTH / 2.0;
    double h = HEIGHT / 2.0;
    double cX = (pX - w + 0.5) / w;
    double cY = (pY - h + 0.5) / h;
    double dist = sqrt(cX * cX + cY * cY);

    double brightness = sin((dist * 2.0 + -t / 128.0));
    uint8_t value = map(acos(cX / dist), -PI, PI, 0, 255);

    CHSV color = getColor(t);
    color.value = value;

    leds[i] = color;
  }
}

uint32_t timeOffset;

void setup(uint32_t time) {
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_DRAW_MA - BOARD_CURRENT_MA);
  FastLED.delay(16);
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);

  timeOffset = time;
}

void loop(){
  const uint32_t t = (millis() + timeOffset) / 8;
  switch (config.effect) {
    case STEADY: 
      steady(t, leds);
      break;
    case BREATHE: 
      breathe(t, leds);
      break;
    case RAIN:
      rain(t, leds);
      break;
    case GRADIENT:
      gradient(t, leds);
      break;
    case RAINBOW:
      rainbow(t, leds);
      break;
    default:
      delay(100);
      break;
  }
  FastLED.delay(8);
}

Configuration get_config() {
  return config;
}

void set_effect(Effect effect) {
  config.effect = effect;
  if (effect < 0) {
    FastLED.clear();
  }
}

void set_cycle() {
  config.cycle = true;
}

void set_hue(uint8_t hue) {
  config.base_color = CHSV(hue, 255, DEFAULT_BRIGHTNESS);
  config.cycle = false;
}

void set_hsv(uint8_t hue, uint8_t saturation, uint8_t value) {
  uint16_t big_value = value * DEFAULT_BRIGHTNESS / 255;
  config.base_color = CHSV(hue, saturation, static_cast<uint8_t>(big_value));
  config.cycle = false;
}

} // namespace led
