#pragma once

#include <FastLED.h>

namespace led {

  enum Effect {
    STEADY,
    BREATHE,
    RAIN,
  };

  struct Configuration {
    CHSV base_color = CHSV(0, 0, 0);
    bool cycle = true;
    Effect effect = STEADY;
  };

  void setup(uint32_t time);

  void loop();

  void configuration(Configuration config);

  Configuration get_config();

  void set_effect(Effect effect);
  void set_cycle();
  void set_hue(uint8_t hue);
  void set_hsv(uint8_t hue, uint8_t saturation, uint8_t value);
}
