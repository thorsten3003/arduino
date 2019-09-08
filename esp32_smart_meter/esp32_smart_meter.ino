/*
 * Energiemonitor
 * 
 * Make: <www.make-magazin.de>
 * Florian Schäffer
 * X/2018
 * 
 * Sensor: SCT-013-30
 * IoT: https://thingspeak.com 
 * 
 * Downloaded from: technik-fan.de/index.php/Open_Energy_Monitor_mit_dem_ESP32
 */
 
#include <Arduino.h>
#include <WiFi.h>

// Network information:
const char* ssid     = "TS-DG-OG";
const char* password = "DieMeeries";
// ThingSpeak Settings:
String writeAPIKey = "4TN19HQ8J267A0UR";    // API-Key fuer Ihren Channel
char server[] = "api.thingspeak.com";   
char host[] = "v92140.1blu.de";

// CT: Voltage depends on current, burden resistor, and turns
#define CT_BURDEN_RESISTOR    62
#define CT_TURNS              1800
#define VOLTAGE               230
#define LED_BUILTIN 2

const int inPinI = 39;   //Set Voltage and current input pins

//Calibration coeficients
//These need to be set in order to obtain accurate results
//Set the above values first and then calibrate futher using normal calibration method described on how to build it page.
double OFFSET = 0.58;
double ICAL = 1.095;

// Initial gueses for ratios, modified by VCAL/ICAL tweaks
double I_RATIO = (long double)CT_TURNS / CT_BURDEN_RESISTOR * 3.3 / 4096 * ICAL;

//Filter variables 1
double lastFilteredI, filteredI;
double sqI,sumI;
//Sample variables
int lastSampleI,sampleI;

double Irms;
int firstrun = 0; //First Run Counter
float PowerSum = 0; //Power Calculation
const int numberOfSamples = 1000;
const unsigned long DELAY = 1000;   //every 15 seconds one message

void BlinkLED (int anz)
{
  for (int i = 0; i < anz; i++)
  {
      digitalWrite(LED_BUILTIN, LOW);
      delay ( 50 );
      digitalWrite(LED_BUILTIN, HIGH);
      delay ( 50 );
  }
}

void connectWiFi(){

    while (WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password);
        delay(3000);
    }

    // Show the user a connection is successful.
    Serial.println("Connected");
    BlinkLED (25);
}

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT); //Set Information LED
  pinMode(inPinI,INPUT);     //Set Analog Inputs
  adcAttachPin(inPinI);
  Serial.begin(115200);
  connectWiFi();
}

void loop() 
{
    // In each loop, make sure there is always an internet connection.
    if (WiFi.status() != WL_CONNECTED) { 
        connectWiFi();
    }
          
  for (int n=0; n<numberOfSamples; n++)
  {
 
   //Used for offset removal
   lastSampleI=sampleI;
 
   //Read in voltage and current samples.   
   sampleI = analogRead(inPinI);
 
   //Used for offset removal
   lastFilteredI = filteredI;
 
   //Digital high pass filters to remove 1.6V DC offset.
   filteredI = 0.9989 * (lastFilteredI+sampleI-lastSampleI);
 
   //Root-mean-square method current
   //1) square current values
   sqI = filteredI * filteredI;
   //2) sum 
   sumI += sqI;
   delay(0.0002);
  }
  
  //Calculation of the root of the mean of the voltage and current squared (rms)
  //Calibration coeficients applied. 
    Irms = (I_RATIO * sqrt(sumI / numberOfSamples)) - OFFSET;
  if (Irms < 0){ Irms = 0; }; //Set negative Current to zero
    sumI = 0;
 
  Serial.println("Irms:" + String(Irms));
  PowerSum = (Irms * VOLTAGE);  //Calculate Power

  if (firstrun <= 2)  //Counter for Trash Data
    firstrun++;
  else
  {
    Serial.println(PowerSum);
    httpRequest(PowerSum);
  }

  delay (DELAY);
}

void httpRequest(float field1Data) 
{
   WiFiClient client;
    if (!client.connect(host, 80)){
      
        Serial.println("connection failed");
        client.stop();
        return;     
    }
    else{
        

    // We now create a URI for the request
    // http://v92140.1blu.de/energie/emoncms/input/post?node=WGS220&fulljson={"Phase1":100,"Phase2":200,"Phase3":300}
    String url = "/energie/emoncms/input/post.json?node=WGS220&apikey=4f76b2121bcaee97658de17a5cb0ca41&json={" ;
    url = url + "Phase1:" + 0 + ",";
    url = url + "Phase2:" + 0 + ",";
    url = url + "Phase3:" + 0 + ",";
    url = url + "PowerSum:" + PowerSum + "}";

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
    client.stop();
}
