/*
 * Board: Wemos Lolin32 oder TTGO LoRa32-OLED-V1
 *   WiFi.mode(WIFI_STA);
 * Downloaded from: technik-fan.de/index.php/Open_Energy_Monitor_mit_dem_ESP32
 */
#include <Arduino.h>
#include "WiFi.h"
#include "DHT.h"                
#include <ArduinoOTA.h>

//Setup variables
int numberOfSamples = 1000;

const char* host = "v92140.1blu.de";

//Set Voltage and current input pins
int inPinI1 = 34;

//First Run Counter
int firstrun = 0;

//Power Calculation
double PowerSum = 0;

//WiFi Part
//const char* ssid     = "TS-EG";
const char* ssid     = "TS-DG-OG";
const char* password = "DieMeeries";

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// CT: Voltage depends on current, burden resistor, and turns
#define CT_BURDEN_RESISTOR    62
#define CT_TURNS              1800
//#define VOLTAGE               230
double VOLTAGE = 230 ;

//Calibration coeficients
//These need to be set in order to obtain accurate results
//Set the above values first and then calibrate futher using normal calibration method described on how to build it page.
double ICAL = 1.08;

// Initial gueses for ratios, modified by VCAL/ICAL tweaks
double I_RATIO = (long double)CT_TURNS / CT_BURDEN_RESISTOR * 3.3 / 4096 * ICAL;

//Filter variables 1
double lastFilteredI1, filteredI1;
double sqI1,sumI1;
//Sample variables
int lastSampleI1,sampleI1;


//Useful value variables
double Irms1;
double Solar1;
unsigned long timer;

//EspClass ESPm;
String url = "";

//WiFi
void WIFI_Connect()
{
  WiFi.disconnect();
  Serial.println("Connecting WIFI...");
     
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
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


void setup() {
  dht.begin();
  //Set Analog Inputs
  pinMode(inPinI1,INPUT);
  adcAttachPin(inPinI1);

  Serial.begin(115200);
  delay(500);
  
  //WiFi Part
  WIFI_Connect();
            // OTA ---------->
              // Port defaults to 8266
              // ArduinoOTA.setPort(8266);
            
              // Hostname defaults to esp8266-[ChipID]
              ArduinoOTA.setHostname("ESP32Ohneoled_SolarWechselstrom_Temp");
            
              // No authentication by default
              ArduinoOTA.setPassword((const char *)"ESP32Ohneoled_SolarWechselstrom_Temp");
            
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
                if (error == OTA_AUTH_ERROR) {
                  Serial.println("Auth Failed");
                }
                else if (error == OTA_BEGIN_ERROR) {
                  Serial.println("Begin Failed");
                }
                else if (error == OTA_CONNECT_ERROR) {
                  Serial.println("Connect Failed");
                }
                else if (error == OTA_RECEIVE_ERROR) {
                  Serial.println("Receive Failed");
                }
                else if (error == OTA_END_ERROR) {
                  Serial.println("End Failed");
                }
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

//Wifi WatchDog
WiFi.onEvent(WiFiEvent);


//*******************************************************************************
double h = dht.readHumidity();    // Lesen der Luftfeuchtigkeit und speichern in die Variable h
double t = dht.readTemperature(); // Lesen der Temperatur in °C und speichern in die Variable t
if (isnan(h) || isnan(t)) {       
    Serial.println("Fehler beim auslesen des Sensors!");
    return;
  }

  // Nun senden wir die gemessenen Werte an den PC dise werden wir im Seriellem Monitor sehen
  Serial.print("Luftfeuchtigkeit: ");
  Serial.print(h);                  // Ausgeben der Luftfeuchtigkeit
  Serial.print("%\t");              // Tabulator
  Serial.print("Temperatur: ");
  Serial.print(t);                  // Ausgeben der Temperatur
  Serial.write('°');                // Schreiben des ° Zeichen
  Serial.println("C");  
   
//**************************************************************************
//Phase1
  for (int n=0; n<numberOfSamples; n++)
{
   
   //Used for offset removal
   lastSampleI1=sampleI1;
   
   //Read in voltage and current samples.  
   sampleI1 = analogRead(inPinI1);
 
   //Used for offset removal
   lastFilteredI1 = filteredI1;
 
   //Digital high pass filters to remove 1.6V DC offset.
   filteredI1 = 0.9989 * (lastFilteredI1+sampleI1-lastSampleI1);
   
   //Root-mean-square method current
   //1) square current values
   sqI1 = filteredI1 * filteredI1;
   //2) sum
   sumI1 += sqI1;
   delay(0.00002);
}

//Calculation of the root of the mean of the voltage and current squared (rms)
//Calibration coeficients applied.
Irms1 = (I_RATIO * sqrt(sumI1 / numberOfSamples)) - 0.35;     // - 1
if ((Irms1 < 0) || (firstrun < 2)){ Irms1 = 0; }; //Set negative Current to zero and ignore the first 2 calculations
sumI1 = 0;
Solar1 = Irms1 * VOLTAGE;

Serial.println("Irms1:"+String(Irms1));

//Check if WiFi is here
//Automatically reconnect the ESP32 if the WiFi Router is not there...
if (WiFi.status() != WL_CONNECTED)
    {
      WiFi.onEvent(WiFiEvent);
      WIFI_Connect();
    }   else {


//**************************************************************************

//Send the data to the server...
  if ((WiFi.status() == WL_CONNECTED) && (firstrun >= 2)) { //WiFi Check, ignore the first data
   
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }
    // We now create a URI for the request
    // http://v92140.1blu.de/energie/emoncms/input/post?node=WGS220&fulljson={"Phase1":100,"Phase2":200,"Phase3":300}
    String url = "/energie/emoncms/input/post.json?node=WGS220&apikey=4f76b2121bcaee97658de17a5cb0ca41&json={" ;
    url = url + "Solar1:" + Solar1 + ",";
    url = url + "Temp_DG_links:" + t + ",";
    url = url + "Luftfeuchte_DG_links:" + h + "}";

    
    Serial.print("Requesting URL: ");
    Serial.println(url);
     
        
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    /*
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }*/
   
    }
 
  if (firstrun <= 2) {firstrun++;}; //Counter for Trash Data
  delay(1000);
    }//WiFi Watchdog
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
