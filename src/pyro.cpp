// Load Wi-Fi library
#include <WiFi.h>
#include "sensors.h"
#include "pyro.h"

// WIFI credentials
const char* ssid     = "yourSSID"; //custom ssid
const char* password = "yourPassword"; //custom pw

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String pyroState = "off";

void setupWIFI() {
  // Initialize the output variables as outputs
  pinMode(PYRO_CHANNEL_PIN, OUTPUT);
  // Set outputs to LOW
  digitalWrite(PYRO_CHANNEL_PIN, LOW);

  // Set up AP Mode
  Serial.print("Setting AP...");
  WiFi.softAP(ssid, password);

  // Print local IP address and start web server
  Serial.println("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();

}

void WIFIloop(){
  WiFiClient client = server.available();

  // Start connection
  if (client){
    Serial.println("New client.");
    String currentLine = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
        header += c;

        if (c == '\n') {
          // Send HTTP response
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Turns the EMatch on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("EMatch on");
              pyroState = "on";
              digitalWrite(PYRO_CHANNEL_PIN, HIGH);

            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("EMatch off");
              pyroState = "off";
              digitalWrite(PYRO_CHANNEL_PIN, LOW);

            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #FFFFFF; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for EMatch  
            client.println("<p>EMatch - State " + pyroState + "</p>");

            // If the pyroState is off, it displays the ON button       
            if (pyroState=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();

            // Break out of the while loop
            break;

          } else { // if you got a newline, then clear currentLine
            currentLine = "";

          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;

        }
      }
    }
    client.println("</body></html>");

    // Clear the header variable
    header = "";

    // Close the connection
    Serial.println();
    Serial.println("Disconnecting...");
    client.stop();
  }
}
