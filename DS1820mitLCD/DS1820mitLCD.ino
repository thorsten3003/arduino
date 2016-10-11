#include <OneWire.h>
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/* DS18S20 Temperature chip i/o */
OneWire  ds(9);  // on pin 9
#define MAX_DS1820_SENSORS 2
byte addr[MAX_DS1820_SENSORS][8];

void setup(void) 
{
  lcd.begin(16, 2);

  byte sensor;
   for (sensor=0;sensor<MAX_DS1820_SENSORS;sensor++)
    {
      if (!ds.search(addr[sensor])) 
      {
      //lcd.setCursor(0,sensor);
      //lcd.print("No more addresses.");
      ds.reset_search();
      delay(250);
      return;
      }
    } 
}
int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
char buf[20];


void loop(void) 
{
  readTemp();
} 

void readTemp()
{
  
  byte i, sensor;
  byte present = 0;
  byte data[12];

  for (sensor=0;sensor<MAX_DS1820_SENSORS;sensor++)
  {
    if ( OneWire::crc8( addr[sensor], 7) != addr[sensor][7]) 
    {
      lcd.setCursor(0,sensor);
      lcd.print("CRC not valid");
      return;
    }

    if ( addr[sensor][0] != 0x10) 
    {
      lcd.setCursor(0,sensor);
      lcd.print("No DS1820 Device.");
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

    sprintf(buf, "Temp%d  %c%d.%d \337C     ",sensor,SignBit ? '-' : '+', Whole, Fract < 10 ? 0 : Fract);

    lcd.setCursor(0,sensor);
    lcd.print(buf);
  }
}
