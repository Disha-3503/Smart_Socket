#include <ESP8266WiFi.h>


const char* ssid     = "Patil";
const char* password = "1234567890";


WiFiServer server(80);// Set port to 80


String header; // This storees the HTTP request

// Declare the pins to which the appliances are connected via relays
int plug = D2; //appliance 1
//int app2 = D4; //appliance 2`

#define  app2 LED_BUILTIN
//you can add more more appliances below.

String plugstate = "off";// state of appliance1
String app2state = "off";// state of appliance2


void setup() {
  Serial.begin(115200);
 // Set the pinmode of the pins to which the LEDs are connected and turn them low to prevent flunctuations
  pinMode(plug, OUTPUT);
  pinMode(app2, OUTPUT);
  digitalWrite(plug, LOW);
  digitalWrite(app2, LOW);
  //connect to access point
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());// this will display the Ip address of the Pi which should be entered into your browser
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /plug/on") >= 0) {
              Serial.println("App 1 on");
              plugstate = "on";
              digitalWrite(plug, HIGH);
            } else if (header.indexOf("GET /plug/off") >= 0) {
              Serial.println("App 1 off");
              plugstate = "off";
              digitalWrite(plug, LOW);
            } else if (header.indexOf("GET /app2/on") >= 0) {
              Serial.println("App 2 on");
              app2state = "on";
//              digitalWrite(app2, HIGH);
            } else if (header.indexOf("GET /app2/off") >= 0) {
              Serial.println("App 2 off");
              app2state = "off";
//              digitalWrite(app2, LOW);
            }
       
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            //client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>plug - State " + plugstate + "</p>");
            // If Appliance 1 is off, it displays the ON button       
            if (plugstate == "off") {
              client.println("<p><a href=\"/plug/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/plug/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>app2 - State " + app2state + "</p>");
            // If Appliance 2 is off, it displays the ON button       
            if (app2state == "off") {
              client.println("<p><a href=\"/app2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/app2/off\"><button class=\"button button2\">OFF</button></a></p>");
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
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
