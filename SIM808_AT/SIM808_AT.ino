#include <SoftwareSerial.h>
#define PIN_TX 10
#define PIN_RX 11
SoftwareSerial mySerial(PIN_TX,PIN_RX);

void setup()
{
mySerial.begin(9600);   
Serial.begin(9600);   
delay(100);
}
void loop()
{
 if (Serial.available()>0)
 mySerial.write(Serial.read());
 if (mySerial.available()>0)
 Serial.write(mySerial.read());
}
