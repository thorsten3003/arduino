/*
 * Downloaded from: technik-fan.de/index.php/Open_Energy_Monitor_mit_dem_ESP32
 */
#include <Arduino.h>
#include "WiFi.h"
#include "SSD1306Wire.h"
#include <ArduinoOTA.h>
SSD1306Wire  display(0x3c, 5, 4);

unsigned long timer;

//WiFi Part
const char* ssid     = "TS-EG";
const char* password = "DieMeeries";


//WiFi
void WIFI_Connect()
{
  digitalWrite(2,1);
  WiFi.disconnect();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
    // Wait for connection
  for (int i = 0; i < 25; i++)
  {
    if ( WiFi.status() != WL_CONNECTED ) {
      delay ( 250 );
      digitalWrite(2,0);
      Serial.print ( "." );
      delay ( 250 );
      digitalWrite(2,1);
    }
  }
  digitalWrite(2,0);
}


void setup() {
  //Set Information LED
  pinMode(2, OUTPUT);
  //Set Analog Inputs
 
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
  display.setContrast(255);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  delay(500);
  
  //WiFi Part
  WIFI_Connect();
              // OTA ---------->
              // Port defaults to 8266
              // ArduinoOTA.setPort(8266);
            
              // Hostname defaults to esp8266-[ChipID]
              ArduinoOTA.setHostname("ESP32oled2_SoloarWechselstrom_messen");
            
              // No authentication by default
              ArduinoOTA.setPassword((const char *)"ESP32oled2_SoloarWechselstrom_messen");
            
              ArduinoOTA.onStart([]() {
                Serial.println("OTA Start");
              });
              ArduinoOTA.onEnd([]() {
                Serial.println("OTA End");
              });
              ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
                Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
              });
              ArduinoOTA.onError([](ota_error_t error) {
                Serial.printf("Error[%u]: ", error);
                if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
                else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
                else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
                else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
                else if (error == OTA_END_ERROR) Serial.println("End Failed");
              });
              ArduinoOTA.begin();
              Serial.println("OTA Ready");
              Serial.print("IP address: ");
              Serial.println(WiFi.localIP());
              // OTA <----------
}

void loop() {
ArduinoOTA.handle();
timer = millis();
display.clear();
display.drawString(0,0 , " Timer.");
display.drawString(0,10 , String(timer) );
display.display();


//Check if WiFi is here
//Automatically reconnect the ESP32 if the WiFi Router is not there...
if (WiFi.status() != WL_CONNECTED)
    {
      WIFI_Connect();
    }   else {


    }
}
