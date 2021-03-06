/*
  Web client
  Voltage Sensor http://henrysbench.capnfatz.com/henrys-bench/arduino-voltage-measurements/arduino-25v-voltage-sensor-module-user-manual/
 */
#define SENSOR A1
const float R1 = 30000.0f; // Der Widerstand R1 hat eine größe von 30 kOhm
const float R2 = 7500.0f; //  Der Widerstand R2 hat eine größe von 7,5 kOhm
const float MAX_VIN = 3.39f;

#include <SPI.h>
#include <Ethernet.h>
#include <avr/wdt.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "172.17.2.60";    // name address for Google (using DNS)
IPAddress ip(172, 17, 2, 177);
IPAddress myDns(172, 17, 2, 1);
EthernetClient client;
// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement
bool isImNetz=false;
int statusSteckdose=3;
int sollSteckdoseStatus=0;
unsigned long Zeitprev; //Zeitspeicher 
unsigned long  Zeiteingeschaltet; //Zeitspeicher wann eingeschatet wurde

// ##################################################

void softwareReset( uint8_t prescaller) {
  Serial.println("Reset in 60 Sekunden:");
  delay(60000);
  wdt_enable( prescaller);
  while(1) {}
}

  int doSteckdose(int Schaltstatus) {
       Serial.print("Schaltstatus: ");
       Serial.println(Schaltstatus);
  client.stop();
  
  if (client.connect(server, 80)) {
    Serial.println("connecting... ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    if(Schaltstatus ==10) {
      client.println("GET /api/relay/0?apikey=DEA130E8984A22D7&value=0 HTTP/1.1");
      Serial.println("GET /api/relay/0?apikey=DEA130E8984A22D7&value=0 HTTP/1.1");
    }
    else if(Schaltstatus ==11) {
      client.println("GET /api/relay/0?apikey=DEA130E8984A22D7&value=1 HTTP/1.1");
      Serial.println("GET /api/relay/0?apikey=DEA130E8984A22D7&value=1 HTTP/1.1");
    }
    else if(Schaltstatus ==13) {
      client.println("GET /api/relay/0?apikey=DEA130E8984A22D7 HTTP/1.1");
      Serial.println("GET /api/relay/0?apikey=DEA130E8984A22D7 HTTP/1.1");
    }
    client.println("Host: 172.17.2.60");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
    isImNetz=false;
  }
}

// ##################################################


void setup() {
  pinMode(SENSOR, INPUT);
  Serial.begin(9600);
  
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
    isImNetz=true;
  }
 
  delay(1000); // give the Ethernet shield a second to initialize:
  Zeitprev = millis(); //Zeit merken
}


  
void loop() {
   // if there are incoming bytes available
          // from the server, read them and print them:
          int len = client.available();
          if (len > 0) {
            byte buffer[80];
            if (len > 80) len = 80;
            client.read(buffer, len);
            if (printWebData) {
              Serial.write(buffer, len); // show in the serial monitor (slows some boards)
            }
            byteCount = byteCount + len;
          }
  
    if (!client.connected()) {
    endMicros = micros();
    client.stop();
    }

// ---------

  if ((millis() - Zeitprev) > 60000) { //60 Sekunden warten
    Zeitprev=millis();

  float vout = (analogRead(SENSOR) * MAX_VIN) / 1024.0f;
  float vin = vout / (R2/(R1+R2)); 
  Serial.print("Eingangsspannung ");
  Serial.print(vin,2);
  Serial.println("V");
  

  if( (vin<=12.00) && (sollSteckdoseStatus==0) ) {
    Serial.println("Schalte EIN ");
    doSteckdose(11); //einschalten 
    Zeiteingeschaltet=millis();
    sollSteckdoseStatus=1;
  }

     
 if ( (millis() - Zeiteingeschaltet > 3600000) && (vin<13.00) && (sollSteckdoseStatus==1) ) {
    Serial.println("Schalte AUS ");
    doSteckdose(10); //ausschalten
    sollSteckdoseStatus=0;
  }

  if(!isImNetz) {
    //Neustart des arduinos falls nicht im Netz
    softwareReset( WDTO_60MS);
  }
    }
}
