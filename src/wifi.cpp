#include "config.h"
#include "leds.h"
#include "wifi.h"

#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>

namespace wifi {

IPAddress timeServerIP;
const char* ntpServerName = "0.us.pool.ntp.org";

// Set web server port number to 80
WiFiServer server(80);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

long setup() {

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  #if defined BOX
  WiFi.setHostname("boxlight");
  #elif defined BED
  WiFi.setHostname("bedlight");
  #endif
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  long time = timeClient.getEpochTime() * 1000;
  timeClient.end();
  server.begin();
  return time;
}

void sendIndex(WiFiClient client) {
  String httpBody =
    "HTTP/1.1 200 OK\n"
    "Content-type:text/html\n"
    "Connection: close\n"
    "\n"

    // Display the HTML web page
    "<!DOCTYPE html><html>\n"
    "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
    "<title>Light</title>\n"
      "<style>\n"
      "button {\n"
      "  display: block;\n"
      "  padding: 1em;\n"
      "}\n"
      "</style>\n"

    "<script>\n"
      "function setEffect(effect) {\n"
      "  fetch(`/effect/${effect}`, {\n"
      "    method: 'PUT'\n"
      "  });\n"
      "}\n"
      "\n"
      "function setHue(hue) {\n"
      "  fetch(`/hue/${hue}`, {\n"
      "    method: 'PUT'\n"
      "  });\n"
      "}\n"
      "\n"
      "function setHSV(h, s, v) {\n"
      "  fetch(`/hue/${h}-${s}-${v}`, {\n"
      "    method: 'PUT'\n"
      "  });\n"
      "}\n"
      "\n"
      "function setCycle(hue) {\n"
      "  fetch(`/cycle`, {\n"
      "    method: 'PUT'\n"
      "  });\n"
      "}\n"
    "</script>\n"
    "</head>\n"
    
    // Web Page Heading
    "<body>\n"
      "<div id=\"app\">\n"
        "<h1>Light</h1>\n"
        "<p>Could not load app, falling back to basic interface.</p>\n"
        "<button onclick=\"setEffect(0)\">Steady</button>\n"
        "<button onclick=\"setEffect(1)\">Breathe</button>\n"
        "<button onclick=\"setEffect(2)\">Rain</button>\n"
        "<button onclick=\"setEffect(-1)\">Off</button>\n"
        "<br />\n"
        "<button onclick=\"setCycle()\">Cycle</button>\n"
        "<button onclick=\"setHue(0)\">Red</button>\n"
        "<button onclick=\"setHue(32)\">Orange</button>\n"
        "<button onclick=\"setHue(64)\">Yellow</button>\n"
        "<button onclick=\"setHue(96)\">Green</button>\n"
        "<button onclick=\"setHue(128)\">Cyan</button>\n"
        "<button onclick=\"setHue(160)\">Blue</button>\n"
        "<button onclick=\"setHue(208)\">Magenta</button>\n"
      "</div>\n"
      "<script src=\"https://wmmiii.github.io/wall_light/index.js\"></script>\n"
    "</body></html>\n";

  client.println(httpBody);
}

void sendInfo(WiFiClient client) {
  #if defined BED
  String name = "Bed";
  #elif defined BOX
  String name = "Box";
  #endif
  led::Configuration config = led::get_config();
  CHSV color = rgb2hsv_approximate(config.base_color);
  String body = "{\"name\":\"" + name + "\","
      "\"effect\":" + String(config.effect) + ","
      "\"h\":" + String(color.h) + ","
      "\"s\":" + String(color.s) + ","
      "\"v\":" + String(color.v) + ","
      "\"cycle\":" + (config.cycle ? "true" : "false") + "}";
  
  client.println("HTTP/1.1 200 OK");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Content-type:application/json");
  client.println("Content-Length: " + String(body.length() + 2));
  client.println("Connection: close");
  client.println();
  client.print(body);
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    // Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        // Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            if (header.indexOf("GET / HTTP") >= 0) {
              sendIndex(client);

            } else if (header.indexOf("OPTIONS") >= 0) {
              client.println("HTTP/1.1 204 No Content");
              client.println("Allow: OPTIONS, GET, PUT");
              client.println("Access-Control-Allow-Methods: OPTIONS, GET, PUT");
              client.println("Access-Control-Allow-Origin: *");
              client.println();

            } else if (header.indexOf("GET /lightInfo HTTP") >= 0) {
              sendInfo(client);

            } else if (header.indexOf("PUT /cycle") >= 0) {
              led::set_cycle();

              sendInfo(client);

            } else if (header.indexOf("PUT /hue") >= 0 ) {
              String hue_string = header.substring(9, header.indexOf(" HTTP"));
              led::set_hue(hue_string.toInt());

              sendInfo(client);

            } else if (header.indexOf("PUT /color/") >= 0 ) {
              const String delimiter = "-";
              String s = header.substring(11);
              int first = s.indexOf(delimiter);
              int second = s.indexOf(delimiter, first + 1);
              int third = s.indexOf(" HTTP/1.1");
              if (first > 0 && second > 0) {
                uint8_t hue = s.substring(0, first).toInt();
                uint8_t saturation = s.substring(first + 1, second).toInt();
                uint8_t value = s.substring(second + 1, third).toInt();

                Serial.print("Set value: ");
                Serial.println(value);

                led::set_hsv(hue, saturation, value);

                sendInfo(client);
              } else {
                client.println("HTTP/1.1 400 Bad Request");
                client.println();
              }


            } else if (header.indexOf("PUT /effect") >= 0 ) {
              String effect_string = header.substring(12, header.indexOf(" HTTP"));
              led::set_effect(static_cast<led::Effect>(effect_string.toInt()));

              sendInfo(client);

            } else {
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-type:text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Not found");
            }
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    // Serial.println("Client disconnected.");
    // Serial.println("");
  }
}

} // namespace wifi
