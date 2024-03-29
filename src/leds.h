#pragma once

#include <FastLED.h>

namespace led {

enum Effect {
  OFF = -1,
  STEADY = 0,
  BREATHE = 1,
  RAIN = 2,
  GRADIENT = 3,
  RAINBOW = 4,
  SPIRAL = 5,
};

struct Configuration {
  CRGB base_color = CRGB(0, 255, 0);
  bool cycle = true;
  Effect effect = STEADY;
};

void setup(uint32_t time);

void loop();

void configuration(Configuration config);

Configuration get_config();

void set_effect(Effect effect);
void set_cycle();
void set_cycle(bool cycle);
void set_hue(uint8_t hue);
void set_hsv(uint8_t hue, uint8_t saturation, uint8_t value);
void set_rgb(uint8_t r, uint8_t g, uint8_t b);
} // namespace led
