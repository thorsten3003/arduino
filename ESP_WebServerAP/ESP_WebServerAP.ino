/*/*
 * Board: Wemos Lolin32 oder TTGO LoRa32-OLED-V1
 *   WiFi.mode(WIFI_STA);
 * Downloaded from: technik-fan.de/index.php/Open_Energy_Monitor_mit_dem_ESP32
 *
 * --------------------------------------------------
  HTTP 1.1 Webserver as AccessPoint for ESP8266
  for ESP8266 adapted Arduino IDE

  by Stefan Thesen 08/2015 - free for anyone

  Does HTTP 1.1 with defined connection closing.
  Handles empty requests in a defined manner.
  Handle requests for non-exisiting pages correctly.

  This demo allows to switch two functions:
  Function 1 creates serial output and toggels GPIO2
  Function 2 just creates serial output.

  Serial output can e.g. be used to steer an attached
  Arduino, Raspberry etc.
  --------------------------------------------------*/

#include "WiFi.h"
#include "DHT.h"                

//Spannung messen
int Sensor = 34;
float Spannung = 0.0;
int Sensorwert = 0;
int NumberOfSamples = 1000;

//Temperatur messen
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//WIFI
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";
unsigned long ulReqcount;
unsigned long timer;

// Create an instance of the server on Port 80
WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
  dht.begin();

  // setup globals
  ulReqcount = 0;

  // prepare GPIO2
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);

  // start serial
  Serial.begin(9600);
  delay(1);

  // AP mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.begin();
}

int timeSinceLastRead = 0;
void loop()
{
  timer = millis();

  //Spannung messen
  Sensorwert = analogRead(Sensor);
  Spannung = Sensorwert * 0.01325 ;
  Serial.print("INPUT V= ");
  Serial.println(Spannung, 1);
  Serial.print("Value= ");
  Serial.println(Sensorwert);

  //*******************************************************************************
  if (timeSinceLastRead > 2000) {
    double h = dht.readHumidity();    // Lesen der Luftfeuchtigkeit und speichern in die Variable h
    double t = dht.readTemperature(); // Lesen der Temperatur in °C und speichern in die Variable t
    if (isnan(h) || isnan(t)) {
      Serial.println("Fehler beim auslesen des Sensors!");
      return;
    }
    Serial.print("Luftfeuchtigkeit: ");
    Serial.print(h);                  // Ausgeben der Luftfeuchtigkeit
    Serial.print("%\t");              // Tabulator
    Serial.print("Temperatur: ");
    Serial.print(t);                  // Ausgeben der Temperatur
    Serial.write('°');                // Schreiben des ° Zeichen
    Serial.println("C");

    //**************************************************************************
    
    timeSinceLastRead = 0;
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  unsigned long ultimeout = millis() + 250;
  while (!client.available() && (millis() < ultimeout) )
  {
    delay(1);
  }
  if (millis() > ultimeout)
  {
    Serial.println("client connection time-out!");
    return;
  }

  // Read the first line of the request
  String sRequest = client.readStringUntil('\r');
  //Serial.println(sRequest);
  client.flush();

  // stop client, if request is empty
  if (sRequest == "")
  {
    Serial.println("empty request! - stopping client");
    client.stop();
    return;
  }

  // get path; end of path is either space or ?
  // Syntax is e.g. GET /?pin=MOTOR1STOP HTTP/1.1
  String sPath = "", sParam = "", sCmd = "";
  String sGetstart = "GET ";
  int iStart, iEndSpace, iEndQuest;
  iStart = sRequest.indexOf(sGetstart);
  if (iStart >= 0)
  {
    iStart += +sGetstart.length();
    iEndSpace = sRequest.indexOf(" ", iStart);
    iEndQuest = sRequest.indexOf("?", iStart);

    // are there parameters?
    if (iEndSpace > 0)
    {
      if (iEndQuest > 0)
      {
        // there are parameters
        sPath  = sRequest.substring(iStart, iEndQuest);
        sParam = sRequest.substring(iEndQuest, iEndSpace);
      }
      else
      {
        // NO parameters
        sPath  = sRequest.substring(iStart, iEndSpace);
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  // output parameters to serial, you may connect e.g. an Arduino and react on it
  ///////////////////////////////////////////////////////////////////////////////
  if (sParam.length() > 0)
  {
    int iEqu = sParam.indexOf("=");
    if (iEqu >= 0)
    {
      sCmd = sParam.substring(iEqu + 1, sParam.length());
      Serial.println(sCmd);
    }
  }


  ///////////////////////////
  // format the html response
  ///////////////////////////
  String sResponse, sHeader;

  ////////////////////////////
  // 404 for non-matching path
  ////////////////////////////
  if (sPath != "/")
  {
    sResponse = "<html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL was not found on this server.</p></body></html>";

    sHeader  = "HTTP/1.1 404 Not found\r\n";
    sHeader += "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n";
    sHeader += "Content-Type: text/html\r\n";
    sHeader += "Connection: close\r\n";
    sHeader += "\r\n";
  }
  ///////////////////////
  // format the html page
  ///////////////////////
  else
  {
    ulReqcount++;
    sResponse  = "<html><head><title>Demo f&uumlr ESP8266 Steuerung</title></head><body>";
    sResponse += "<font color=\"#000000\"><body bgcolor=\"#d0d0f0\">";
    sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
    sResponse += "<h1>Demo f&uumlr ESP8266 Access Point</h1>";
    sResponse += "Funktion 1 schaltet GPIO2 und erzeugt eine serielle Ausgabe.<BR>";
    sResponse += "Funktion 2 erzeugt nur eine serielle Ausgabe.<BR>";

    sResponse += "Spannung= ";
    sResponse += Spannung;
    sResponse += "<FONT SIZE=+1>";
    sResponse += "<p>Funktion 1 <a href=\"?pin=FUNCTION1ON\"><button>einschalten</button></a>&nbsp;<a href=\"?pin=FUNCTION1OFF\"><button>ausschalten</button></a></p>";
    sResponse += "<p>Funktion 2 <a href=\"?pin=FUNCTION2ON\"><button>einschalten</button></a>&nbsp;<a href=\"?pin=FUNCTION2OFF\"><button>ausschalten</button></a></p>";

    //////////////////////
    // react on parameters
    //////////////////////
    if (sCmd.length() > 0)
    {
      // write received command to html page
      sResponse += "Kommando:" + sCmd + "<BR>";

      // switch GPIO
      if (sCmd.indexOf("FUNCTION1ON") >= 0)
      {
        digitalWrite(2, 1);
      }
      else if (sCmd.indexOf("FUNCTION1OFF") >= 0)
      {
        digitalWrite(2, 0);
      }
    }

    sResponse += "<FONT SIZE=-2>";
    sResponse += "<BR>Aufrufz&auml;hler=";
    sResponse += ulReqcount;
    sResponse += "<BR>";
    sResponse += "Stefan Thesen 08/2015<BR>";
    sResponse += "</body></html>";

    sHeader  = "HTTP/1.1 200 OK\r\n";
    sHeader += "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n";
    sHeader += "Content-Type: text/html\r\n";
    sHeader += "Connection: close\r\n";
    sHeader += "\r\n";
  }

  // Send the response to the client
  client.print(sHeader);
  client.print(sResponse);

  // and stop the client
  client.stop();
  Serial.println("Client disonnected");
}
