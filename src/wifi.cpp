#include "config.h"
#include "leds.h"
#include "wifi.h"

#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>

namespace wifi {

// Replace with your network credentials
const char* ssid = "ssid";
const char* password = "password";

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
  Serial.println(ssid);
  WiFi.begin(ssid, password);
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
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();

  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<title>Light</title>");

  client.println(
    "<style>\n"
    "button {\n"
    "  display: block;\n"
    "  padding: 1em;\n"
    "}\n"
    "</style>\n"
  );

  client.println("<script src=\"https://wmmiii.github.io/wall_light/light.js\"></script>\n");
  client.println("</head>");
  
  // Web Page Heading
  client.println("<body><h1>Light</h1>");
  client.println("<button onclick=\"setEffect(0)\">Steady</button>");
  client.println("<button onclick=\"setEffect(1)\">Breathe</button>");
  client.println("<button onclick=\"setEffect(2)\">Rain</button>");
  client.println("<button onclick=\"setEffect(-1)\">Off</button>");
  client.println("<br />");
  client.println("<button onclick=\"setCycle()\">Cycle</button>");
  client.println("<button onclick=\"setHue(0)\">Red</button>");
  client.println("<button onclick=\"setHue(32)\">Orange</button>");
  client.println("<button onclick=\"setHue(64)\">Yellow</button>");
  client.println("<button onclick=\"setHue(96)\">Green</button>");
  client.println("<button onclick=\"setHue(128)\">Cyan</button>");
  client.println("<button onclick=\"setHue(160)\">Blue</button>");
  client.println("<button onclick=\"setHue(208)\">Magenta</button>");
  client.println("</body></html>");
}

void sendInfo(WiFiClient client) {
  #if defined BED
  String name = "Bed";
  #elif defined BOX
  String name = "Box";
  #endif
  led::Configuration config = led::get_config();
  String body = "{\"name\":\"" + name + "\","
      "\"effect\":" + String(config.effect) + ","
      "\"h\":" + String(config.base_color.h) + ","
      "\"s\":" + String(config.base_color.s) + ","
      "\"v\":" + String(config.base_color.v) + ","
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
              String hue_string = header.substring(9);
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
                uint8_t saturation = s.substring(first, second).toInt();
                uint8_t value = s.substring(second, third).toInt();

                led::set_hsv(hue, saturation, value);

                sendInfo(client);
              } else {
                client.println("HTTP/1.1 400 Bad Request");
                client.println();
              }


            } else if (header.indexOf("PUT /effect") >= 0 ) {
              String hue_string = header.substring(12);

              led::set_effect(static_cast<led::Effect>(hue_string.toInt()));

              client.println("HTTP/1.1 202 Accepted");
              client.println();

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
