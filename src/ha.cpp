#include "ha.h"
#include "config.h"
#include "leds.h"

#include <ArduinoHA.h>
#include <WiFi.h>

namespace home_assistant {

HADevice device;
WiFiClient client;
HAMqtt mqtt(client, device);

#if defined(BOX)
const std::string name = "Box Light";
const std::string prefix = "box";
#elif defined(BED)
const std::string name = "Bed Light";
const std::string prefix = "bed";
#else
const std::string name = "Unknown";
const std::string prefix = "unknown";
#endif

const std::string rgb_id = prefix + "_rgb";
const std::string cycle_id = prefix + "_cycle";
const std::string rainbow_id = prefix + "_rainbow";
const std::string mode_id = prefix + "_mode";

const std::string rgb_name = name + " RGB";
const std::string mode_name = name + " Mode";

HALight rgb_light(rgb_id.c_str(), HALight::Features::RGBFeature);
HALight cycle_light(cycle_id.c_str());
HALight rainbow_light(cycle_id.c_str());

HASelect mode_select(select_id.c_str());

void updateInterface() {
  led::Configuration config = led::get_config();
  if (config.effect == led::Effect::OFF) {
    rgb_light.setState(false);
    cycle_light.setState(false);
    rainbow_light.setState(false);
  } else if (config.effect == led::Effect::RAINBOW) {
    rgb_light.setState(false);
    cycle_light.setState(false);
    rainbow_light.setState(true);
  } else if (config.cycle) {
    rgb_light.setState(false);
    cycle_light.setState(true);
    rainbow_light.setState(false);
  } else {
    rgb_light.setState(true);
    cycle_light.setState(false);
    rainbow_light.setState(false);
    rgb_light.setRGBColor(HALight::RGBColor(
        config.base_color.r, config.base_color.g, config.base_color.b));
  }
}

void onColorCycleStateCommand(bool state, HALight *sender) {
  if (state) {
    led::set_cycle(true);
    led::set_effect(led::Effect::STEADY);
  } else {
    led::set_effect(led::Effect::OFF);
  }
  updateInterface();
}

void onRainbowStateCommand(bool state, HALight *sender) {
  if (state) {
    led::set_cycle(false);
    led::set_effect(led::Effect::RAINBOW);
  } else {
    led::set_effect(led::Effect::OFF);
  }
}

void onRGBStateCommand(bool state, HALight *sender) {
  led::set_cycle(false);
  if (state) {
    led::set_effect(led::Effect::STEADY);
  } else {
    led::set_effect(led::Effect::OFF);
  }
  updateInterface();
}

void onRGBCommand(HALight::RGBColor color, HALight *sender) {
  led::set_cycle(false);
  led::set_effect(led::Effect::STEADY);
  led::set_rgb(color.red, color.green, color.blue);
  updateInterface();
}

void onModeSelect(int8_t index, HASelect *sender) {
  led::set_effect(static_cast<led::Effect>(index - 1));
  updateInterface();
}

void setup() {
  byte mac[6];
  WiFi.macAddress(mac);
  device.setUniqueId(mac, sizeof(mac));

  Serial.print("MAC & Device ID: ");
  for (uint8_t i = 0; i < 6; ++i) {
    char hexCar[2];

    sprintf(hexCar, "%02X", mac[i]);
    Serial.print(hexCar);
  }
  Serial.println();

  device.setName(name.c_str());
  device.setManufacturer("https://github.com/wmmiii/");
  device.setModel(name.c_str());
  device.setSoftwareVersion("0.1.0");

  rgb_light.setName(rgb_name.c_str());
  rgb_light.onStateCommand(onRGBStateCommand);
  rgb_light.onRGBColorCommand(onRGBCommand);

  mode_select.setName(mode_name.c_str());
  mode_select.setOptions("Off;Steady;Breathe;Rain;Gradient;Rainbow;Spiral");
  mode_select.onCommand(onModeSelect);

  cycle_light.setName(cycle_name.c_str());
  cycle_light.onStateCommand(onColorCycleStateCommand);

  rainbow_light.setName(rainbow_name.c_str());
  rainbow_light.onStateCommand(onRainbowStateCommand);

  mqtt.begin(BROKER_ADDR, BROKER_PORT, BROKER_USERNAME, BROKER_PASSWORD);

  updateInterface();
}

void loop() { mqtt.loop(); }

} // namespace home_assistant
