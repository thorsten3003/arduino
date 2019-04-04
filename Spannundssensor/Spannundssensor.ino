/*
  Web client
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



void softwareReset( uint8_t prescaller) {
  Serial.println("Reset in 60 Sekunden:");
  delay(60000);
  wdt_enable( prescaller);
  while(1) {}
}


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
}

  int doSteckdose(int status) {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.print("connecting... ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    if(status ==0) {
      client.println("GET /api/relay/0?apikey=DEA130E8984A22D7&value=0 HTTP/1.1");
    }
    if(status ==1) {
      client.println("GET /api/relay/0?apikey=DEA130E8984A22D7&value=1 HTTP/1.1");
    }
    if(status ==3) {
      client.println("GET /api/relay/0?apikey=DEA130E8984A22D7 HTTP/1.1");
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
  
void loop() {
  float vout = (analogRead(SENSOR) * MAX_VIN) / 1024.0f;
  float vin = vout / (R2/(R1+R2)); 
  Serial.print("Eingangsspannung ");
  Serial.print(vin,2);
  Serial.println("V");
 
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

  
 delay(60000);  //1 Minute Intervall
  if(vin<=12.00) {
    doSteckdose(1);

    delay(3600000);  //1Stunde warten
    doSteckdose(0); //wieder ausschalten
  }


  if(!isImNetz) {
    //Neustart des arduinos falls nicht im Netz
    softwareReset( WDTO_60MS);
  }
}
