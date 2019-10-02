

// WiFi includes
 #include <WiFi.h>

 #include <ArduinoOTA.h>

 const char *ssid1         = "TS-DG-OG";
 const char *ssid         = "TS-EG";
 const char *password     = "DieMeeries";
 
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
// or #include "SH1106Wire.h", legacy include: `#include "SH1106.h"`
// For a connection via I2C using brzo_i2c (must be installed) include
// #include <brzo_i2c.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Brzo.h"
// #include "SH1106Brzo.h"
// For a connection via SPI include
// #include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Spi.h"
// #include "SH1106SPi.h"

// Use the corresponding display class:

// Initialize the OLED display using SPI
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
// SSD1306Spi        display(D0, D2, D8);
// or
// SH1106Spi         display(D0, D2);

// Initialize the OLED display using brzo_i2c
// D3 -> SDA
// D5 -> SCL
// SSD1306Brzo display(0x3c, D3, D5);
// or
// SH1106Brzo  display(0x3c, D3, D5);

// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, 5, 4);
// SH1106 display(0x3c, D3, D5);

#define DEMO_DURATION 3000
typedef void (*Demo)(void);

int demoMode = 0;
int counter = 1;


void setup() {
  display.init();
  display.flipScreenVertically();
  display.setContrast(255);
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  
  WiFi.begin ( ssid, password );
  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
         display.drawString(display.getWidth()/2, display.getHeight()/2 - 10, "Wait for Wifi");
         display.display();
    delay ( 10 );
  }
  display.clear();
  // Align text vertical/horizontal center
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.setFont(ArialMT_Plain_10);
  display.drawString(5,5 , "IP-Adresse: " + WiFi.localIP().toString());
  display.drawString(10,5 , "IP-Adresse:\n" + WiFi.localIP().toString());
  display.display();
display.clear();

  ArduinoOTA.begin();
  ArduinoOTA.onStart([]() {
    
    display.drawString(display.getWidth()/2, display.getHeight()/2 - 10, "OTA Update");
    display.display();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    display.drawProgressBar(4, 32, 120, 8, progress / (total / 100) );
    display.display();
  });

  ArduinoOTA.onEnd([]() {
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(display.getWidth()/2, display.getHeight()/2, "Restart");
    display.display();
    infoAnzeigen();
  });
}

void infoAnzeigen() {
  display.clear();
  // Align text vertical/horizontal center
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.setFont(ArialMT_Plain_10);
  display.drawString(5,5 , "IP-Adresse: " + WiFi.localIP().toString());
  display.drawString(10,5 , "IP-Adresse:\n" + WiFi.localIP().toString());
  display.display();
}





void loop() {
 // scanNetworks();
  ArduinoOTA.handle();
//  timer = millis();
  
  //Check if WiFi is here
  //Automatically reconnect the ESP32 if the WiFi Router is not there...
  if (WiFi.status() != WL_CONNECTED)
   {
      WiFi.onEvent(WiFiEvent);
    //  WIFI_Connect();
    }  //Wifi WatchDog
  WiFi.onEvent(WiFiEvent);
  
 
  






}

//Wifi Watchdog
void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d  - ", event);
  switch(event) {
  case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.print("IP address: "); Serial.println(WiFi.localIP());
      break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      WiFi.begin();   // <<<<<<<<<<<  added  <<<<<<<<<<<<<<<
      ESP.restart();
      break;
  case SYSTEM_EVENT_STA_START:
      Serial.println("ESP32 station start");
      break;
  case SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("ESP32 station connected to AP");
      break;
  case SYSTEM_EVENT_STA_LOST_IP:
      Serial.println("ESP32 Lost IP");
      WiFi.begin();   // <<<<<<<<<<<  added  <<<<<<<<<<<<<<<
      ESP.restart();
      break;
  }
}
