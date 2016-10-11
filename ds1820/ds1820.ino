/*
Tested on Arduino 1.0.3
UcontrolIt.TV
Bulding Block 2 - Sketch 1 Single Sensor to Serial with High Low and C/F
 
Add two more libraries: OneWire and DallasTemperature
 
Mark Johnson
uControlIt.tv
December 2012
 
License: GNU General Public License
 
*/
 
// Load Libraries for DS1820 and OneWire
#include <OneWire.h>
#include <DallasTemperature.h>

 
// Variables for temperature readings
float myTemp;
float myHighTemp;
float myLowTemp = 50;
 
// DS1820 Data wire is plugged into pin 4 on the Arduino
#define ONE_WIRE_BUS 4
 
// Setup oneWire instance to communicate with devices
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
 
void setup()
{
 
   // Start the OneWire library
   sensors.begin();
}
 
void loop()
{
 
  // Read the temperature
  readtemp();
  // Write the Results to the serial Monitor
  serialPrint();
 
}
 
void readtemp()
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures
  myTemp = (sensors.getTempCByIndex(0));
 
// Set High or Low Temp
  if (myTemp < myLowTemp) {     myLowTemp = myTemp;   }   if (myTemp > myHighTemp) {
    myHighTemp = myTemp;
  }
}
 
void serialPrint()
{
Serial.print("Current Temp: ");
Serial.print(myTemp);
Serial.print("C");
Serial.print("  Lowest Temp: ");
Serial.print(myLowTemp);
Serial.print("C");
Serial.print("  Highest Temp: ");
Serial.print(myHighTemp);
Serial.println("C");
delay (500);
Serial.print("Or if you prefer : ");
Serial.print(DallasTemperature::toFahrenheit(myTemp));
Serial.print("F / ");
Serial.print(DallasTemperature::toFahrenheit(myLowTemp));
Serial.print("F / ");
Serial.print(DallasTemperature::toFahrenheit(myHighTemp));
Serial.println("F ");
Serial.println();
delay (500);
}
