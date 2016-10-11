#include <OneWire.h>

/* DS18S20 Temperature chip i/o */

OneWire  ds(4);  // on pin 4
#define MAX_DS1820_SENSORS 2
byte addr[MAX_DS1820_SENSORS][8];
void setup(void) 
{
  Serial.begin(9600);
  
  Serial.println("DS1820 Test");
  if (!ds.search(addr[0])) 
  {
    Serial.println("No more addresses.");
    ds.reset_search();
    delay(250);
    return;
  }
  if ( !ds.search(addr[1])) 
  {
    Serial.println("No more addresses.");
    ds.reset_search();
    delay(250);
    return;
  }
}
int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
char buf[20];

void loop(void) 
{
  byte i, sensor;
  byte present = 0;
  byte data[12];

  for (sensor=0;sensor<MAX_DS1820_SENSORS;sensor++)
  {
    if ( OneWire::crc8( addr[sensor], 7) != addr[sensor][7]) 
    {
      Serial.println("CRC is not valid");
      return;
    }

    if ( addr[sensor][0] != 0x10) 
    {
      Serial.println("Device is not a DS18S20 family device.");
      return;
    }

    ds.reset();
    ds.select(addr[sensor]);
    ds.write(0x44,1);         // start conversion, with parasite power on at the end

    delay(1000);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.

    present = ds.reset();
    ds.select(addr[sensor]);    
    ds.write(0xBE);         // Read Scratchpad

    for ( i = 0; i < 9; i++) 
    {           // we need 9 bytes
      data[i] = ds.read();
    }

    LowByte = data[0];
    HighByte = data[1];
    TReading = (HighByte << 8) + LowByte;
    SignBit = TReading & 0x8000;  // test most sig bit
    if (SignBit) // negative
    {
      TReading = (TReading ^ 0xffff) + 1; // 2's comp
    }
    Tc_100 = (TReading*100/2);    

    Whole = Tc_100 / 100;  // separate off the whole and fractional portions
    Fract = Tc_100 % 100;

    sprintf(buf, "%d:%c%d.%d\337C     ",sensor,SignBit ? '-' : '+', Whole, Fract < 10 ? 0 : Fract);


    Serial.println(buf);
  }
}
