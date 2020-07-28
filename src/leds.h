#pragma once

#include <FastLED.h>

namespace led {

  struct Configuration {
    CHSV base_color = CHSV(0, 0, 0);
    bool cycle = true;
  };

  void setup();

  void loop();

  void configuration(Configuration config);

  void set_cycle();
  void set_hue(uint8_t hue);
}
