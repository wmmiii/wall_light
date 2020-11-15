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
    Effect effect = BREATHE;
  };

  void setup(uint32_t time);

  void loop();

  void configuration(Configuration config);

  void set_effect(Effect effect);
  void set_cycle();
  void set_hue(uint8_t hue);
}
