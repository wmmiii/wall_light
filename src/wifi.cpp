#include "leds.h"
#include "wifi.h"

#include <WiFi.h>

namespace wifi {

// Replace with your network credentials
const char* ssid = "CertainlyPorn";
const char* password = "7195343577";

// Set web server port number to 80
WiFiServer server(80);

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

void setup() {

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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void sendIndex(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();

  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");

  client.println(
    "<style>\n"
    "button {\n"
    "  display: block;\n"
    "  padding: 1em;\n"
    "}\n"
    "</style>\n"
  );

  client.println(
    "<script>\n"
    "function setHue(hue) {\n"
    "  fetch(`/hue/${hue}`, {\n"
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
  );
  client.println("</head>");
  // CSS to style the on/off buttons 
  // Feel free to change the background-color and font-size attributes to fit your preferences
  
  // Web Page Heading
  client.println("<body><h1>Wall Light</h1>");
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

void loop(){
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

            if (header.indexOf("GET / HTTP") >=0) {
              sendIndex(client);
            } else if (header.indexOf("PUT /cycle") >= 0) {
              led::set_cycle();

              client.println("HTTP/1.1 202 Accepted");
              client.println();
            } else if (header.indexOf("PUT /hue") >= 0 ) {
              // auto first_space = header.indexOf(" ");
              // auto second_space = header.indexOf(9, first_space + 1);
              String hue_string = header.substring(9);
              led::set_hue(hue_string.toInt());

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
