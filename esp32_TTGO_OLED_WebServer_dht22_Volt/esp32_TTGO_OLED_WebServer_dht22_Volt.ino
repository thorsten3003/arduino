/*
 *   Board: ESP32 Dev Module, Upload 921600, CPU 160MHz, Flash 80MHz, FlashMode QIC, Flash Size 2MB (16MB), Schema: Default 4 with Spiffs, PSRAM disabled
 *   https://randomnerdtutorials.com/esp32-access-point-ap-web-server/
 *   DHT: http://www.iotsharing.com/2017/05/how-to-arduino-esp32-dht11-dht22-temperature-humidity-sensor.html
// https://github.com/Xinyuan-LilyGO/TTGO-T-Display?spm=a2g0o.detail.1000023.1.59f89ca72kY3yP
// http://www.lilygo.cn/claprod_view.aspx?TypeId=21&Id=1128&FId=t28:21:28
// https://github.com/Bodmer/TFT_eSPI
// https://github.com/LennartHennigs/Button2
*/

#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include <Button2.h>
#include "DHT.h"              


#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23

#define TFT_BL          4  // Display backlight control pin
#define ADC_EN          14
#define ADC_PIN         34
#define BUTTON_1        35
#define BUTTON_2        0

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

char buff[512];
int vref = 1100;
int btnCick = false;


//Spannung messen
int Sensor = 32;
float Spannung = 0.0;
int Sensorwert = 0;
const float R1 = 30000.0f; // Der Widerstand R1 hat eine größe von 30 kOhm
const float R2 = 7500.0f; //  Der Widerstand R2 hat eine größe von 7,5 kOhm
const float MAX_VIN = 5.0f;
int NumberOfSamples = 1000;
float h = 0;
float t = 0;
    
//Temperatur messen
#define DHTPIN 33
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//WIFI
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

unsigned long timer;
unsigned long timeSinceLastRead ;

String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;



void setup() {
  Serial.begin(115200);
  Serial.println("Setup");
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  tft.print("IP: ");
  tft.println(IP);
  
  server.begin();
  dht.begin();
  
  timeSinceLastRead = millis();
}


void loop()
{
  timer = millis();
  
  if (timer - timeSinceLastRead > 5000) 
  {
    //Spannung messen
    float vout = (analogRead(Sensor) * MAX_VIN) / 4096.0f;
    Spannung = vout / (R2/(R1+R2)); 
    Serial.println("INPUT V= " + String(Spannung, 1) );
    Serial.println("Value= "   + String(Sensorwert) );
    
    //*******************************************************************************
  
       h = dht.readHumidity();    // Lesen der Luftfeuchtigkeit und speichern in die Variable h
       t = dht.readTemperature(); // Lesen der Temperatur in °C und speichern in die Variable t
      if (isnan(h) || isnan(t)) 
      {
        Serial.println("Fehler beim auslesen des Sensors!");
        h=0;
        t=0;
       }
      Serial.print("Luftfeuchtigkeit: ");
      Serial.print(h,2);                  // Ausgeben der Luftfeuchtigkeit
      Serial.print(" %\t");              // Tabulator
      Serial.print("Temperatur: ");
      Serial.print(t,2);                  // Ausgeben der Temperatur
      Serial.write(' °');                // Schreiben des ° Zeichen
      Serial.println("C");
      //***************************************************************
      tft.fillScreen(TFT_BLACK);
      tft.drawString("Spannung:  " + String(Spannung, 1) + " V",0 , 50 );
      tft.drawString("Humidity:  " + String(h,2) + " %" , 0 , 70 );
      tft.drawString("Temp:      " + String(t,2) + " C" , 0 , 90 );
  
      //**************************************************************************
      
      timeSinceLastRead = millis();
  }
  
  WiFiClient client = server.available();   // Listen for incoming clients
 
   if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
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
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");

            // Sensorwerte
            client.println("<h2>Spannung = " + String(Spannung, 1) + " V</h2>");
            Serial.println("INPUT V= " + String(Spannung, 1) );
            Serial.println("Value= "   + String(Sensorwert) );

            client.print("<h2>Luftfeuchtigkeit: " + String(h,2) + " %</h2>");
            client.print("<h2>Temperatur: " + String(t,2) + " C</h2>");

             client.print("<hr>");
             
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
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
