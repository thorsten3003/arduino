#include <Arduino.h>
#include "WiFi.h"
#include "SSD1306Wire.h"
#include <ArduinoOTA.h>
SSD1306Wire  display(0x3c, 5, 4);


 const int  ssidsize = 2;  //Laenge des ssid Arrays
 const char *ssid[]       = {"TS-EG" ,      "TS-DG-OG" };
 const char *password[]   = {"DieMeeries" , "DieMeeries" } ;

void WIFI_Connect()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin("TS-EG" ,"DieMeeries");
    // Wait for connection
  for (int i = 0; i < 25; i++)
  {
    if ( WiFi.status() != WL_CONNECTED ) {
      delay ( 250 );
      Serial.print ( "." );
      delay ( 250 );
    }
  }
}
 
 void setup()
 {
    Serial.begin(115200);
    display.init();
    display.flipScreenVertically();
    display.setContrast(255);
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_AP_STA);
    WiFi.disconnect();
    delay(500);

//  Scan and Connect Wifi
    if( WiFi.status() != WL_CONNECTED ) {
        display.clear();
        display.drawString(0,0 , "Wifi Scan start ");
        display.display();
        delay(500);
      
      // WiFi.scanNetworks will return the number of networks found
      int anzahlWifiNetzwerke = WiFi.scanNetworks();
        display.clear();
        display.drawString(0,0 , "Wifi Scan beended. ");
        display.display();
        delay(500);
      if (anzahlWifiNetzwerke == 0) {
          display.clear();
          display.drawString(0,0 , "Keine Netzwerke gefunden. ");
          display.display();
      } else {
          display.clear();
          display.drawString(0,0 , String(anzahlWifiNetzwerke));
          display.drawString(10,0 , " Netzwerke gefunden:");
          display.display();
          
          for (int i = 0; i < anzahlWifiNetzwerke; ++i) { //Netzwerke anzeigen
              display.drawString(0,(10*(i+1)) , String(i+1) + ": " + String(WiFi.SSID(i)) + " (" + String(WiFi.RSSI(i)) + ")" );
              display.display(); 
              delay(300);
          }
          delay(1500);

       }
   }
  
} // ende setup

void infoAnzeigen() {
     display.clear();
     display.drawString(0,0 , "Wifi Verbunden mit: ");
     display.drawString(0,10 , "SSID: " + String( WiFi.SSID()) ); 
     display.drawString(0,20 , "Signal: " + String( WiFi.RSSI()) );
     display.drawString(0,30 , "IP-Adresse: " + WiFi.localIP().toString());
     display.display();
}

void loop()
{
 
}
