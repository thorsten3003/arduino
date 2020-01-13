/*
   Board: arduino uno
   HanRun Eth Shield
   Strom messen mit Stromstärkesensor (ACS712) 30 Amp.
   30A Version: Chip ACS712ELCTR-30A-T
   Spannungsversorgung: 5V | Messbereich: -30A bis +30A |
   Spannungsänderung am Sensor pro 1A: 66mV
   https://funduino.de/nr-41-stromstaerkesensor-arduino

  Temperatur und Luftfeuchtigkeit messen mit DHT22
  Spannungsversorgung: 3.3V
  Good for 0-100% humidity readings with 2-5% accuracy
  Good for -40 to 80°C temperature readings ±0.5°C accuracy
  No more than 0.5 Hz sampling rate (once every 2 seconds)
  https://github.com/adafruit/DHT-sensor-library/blob/master/examples/DHT_Unified_Sensor/DHT_Unified_Sensor.ino
  https://learn.adafruit.com/dht

  Ethernet https://www.uweziegenhagen.de/?p=2830
*/
#include <SPI.h>
#include <Ethernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "v92140.1blu.de";    // name address for Google (using DNS)
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(172, 17, 2, 217);
IPAddress myDns(172, 17, 2, 1);
EthernetClient client;
bool connected = false;

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 2
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
DHT_Unified dht(DHTPIN, DHTTYPE);
double humidity = 0;
double temp = 0;


int Sensor = A0; // Der Stromstärkesensor wird am Pin A0 (Analog "0") angeschlossen.
int VpA = 66; // Millivolt pro Ampere (100 für 20A Modul und 66 für 30A Modul)
int sensorwert = 0;
double Nullpunkt = 2490.234; // Spannung in mV bei dem keine Stromstärke vorhanden ist
double SensorSpannung = 0;
double Ampere = 0;
double Solar1;
double Volt = 230;
int numberOfSamples = 50;

unsigned long timer;
unsigned long loop_timer;

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));


  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  timer = millis();
}

void loop() {
  loop_timer = millis();
  if (loop_timer - timer >= 10000) {
    // Temperatur Start************************************************************

    sensors_event_t event; // Get temperature event and print its value.
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println("Error reading temperature!");
    }
    else {
      Serial.print("Temperature: ");
      temp = event.temperature;
      Serial.print(temp);
      Serial.println("°C");
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println("Error reading humidity!");
    }
    else {
      Serial.print("Humidity: ");
      humidity = event.relative_humidity;
      Serial.print(humidity);
      Serial.println("%");
    }
    timer = millis();
  }  // Temperatur Ende************************************************************


  // Strom Start ***************************************************************
  double SolarSum = 0;
  for (int n = 0; n < numberOfSamples; n++)
  {
    sensorwert = analogRead(Sensor);
    SensorSpannung = (sensorwert / 1024.0) * 5000; // Hier wird der Messwert in den Spannungswert in mV am Sensor umgewandelt.
    Ampere = ((SensorSpannung - Nullpunkt) / VpA); // Im zweiten Schritt wird hier die Stromstärke berechnet.
    Solar1 = (Volt * Ampere) -7; // Solar1 in Watt minus Korrekturwert im Leerlauf gemessen
    
    // Ausgabe der Ergebnisse am Seriellen Monitor
    Serial.print("Sensorwert = " ); // Ausgabe des reinen Sensorwertes
    Serial.print(sensorwert);
    Serial.print("\t Sensorspannung in mV = "); // Zeigt die Sensorspannung an
    Serial.print(SensorSpannung, 3); // Die "3" hinter dem Komma erzeugt drei Nachkommastellen
    Serial.print("\t Ampere = "); // shows the voltage measured
    Serial.println(Ampere, 3); // Die "3" hinter dem Komma erzeugt drei Nachkommastellen

    SolarSum = SolarSum + Solar1;
    delay(0.00002);
  }
  Solar1 = SolarSum / numberOfSamples ;
  Serial.print("Solar1 in Watt: ");
  Serial.println(Solar1);
  // Strom Ende ***************************************************************



  if (!connected) {
    Serial.println("Not connected");
    if (client.connect(server, 80)) {
      connected = true;
      Serial.print("Connected to Server ");
      Serial.println(server);
      Serial.println("Sending request");
      client.print( "GET /energie/emoncms/input/post.json?node=WGS220&apikey=4f76b2121bcaee97658de17a5cb0ca41&json={Solar1:" );
      client.print(Solar1);
      client.print(",Temp_DG_links:");
      client.print(temp);
      client.print(",Luftfeuchte_DG_links:");
      client.print(humidity);
      client.print( "}" );
      client.println(" HTTP/1.1");
      client.println("Host: v92140.1blu.de");
      client.println("Accept: text/html");
      /*client.println("User-Agent: Arduino");
      */
      client.println();
    } else {
      Serial.println("Cannot connect to server");
    }
  }
  else {
    delay(500);
    while (client.connected() && client.available()) {
      char c = client.read();
      Serial.print(c);
    }
    Serial.println();
    client.stop();
    connected = false;
  }

  Serial.println("Done now");

}
