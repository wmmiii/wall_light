# Wall Light

This repo contains code to drive my custom ESP32 based light fixtures. The devices themselves proxy to a GitHub hosted page that can control the local fixtures as well as integrate with Home Assistant via MQTT.

## Updating the devices

1. Update configuration information in `/src/config.h`
2. Use PlatformIO to upload to the ESP32 devices themselves

   `pio run -t upload`

## Updating the custom webapp

You must have Webpack installed locally to build the frontend. Maybe someday I'll remove the dependency on this monolith.

1. `cd web`
2. `webpack`
3. Upload the changes to the `/doc` directory
