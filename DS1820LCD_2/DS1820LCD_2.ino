/*
  Beschreibung
*/
//Load Libraries
#include <OneWire.h>
#include <LiquidCrystal.h>
#include <DallasTemperature.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// DS1820 Data wire is plugged into pin 4 on the Arduino
#define ONE_WIRE_BUS 9
 
// Setup oneWire instance to communicate with devices
OneWire oneWire(ONE_WIRE_BUS);
 
 
// Variables for temperature readings
float myTemp;
float myHighTemp;
float myLowTemp = 50;
 
// Pass oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
 
void setup()
{
   // Initialise the LCD display
    initialiseLCD();
 
   // Start the OneWire library
   sensors.begin();
}
 
void loop()
{
 
  // Read the temperature
  readtemp();
  // Write results to the LCD
   LCDPrint();
 
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
 
void LCDPrint()
{
  lcd.setCursor (15, 0 );
  lcd.print("*");
  lcd.setCursor ( 8, 0 );
  lcd.print(myTemp);
  lcd.setCursor ( 0, 1 );
  lcd.print("L=");
  lcd.setCursor ( 2, 1 );
  lcd.print(myLowTemp);
  lcd.setCursor ( 8, 1 );
  lcd.print("H=");
  lcd.setCursor ( 10, 1 );
  lcd.print(myHighTemp);
  delay (500);
  lcd.setCursor (15, 0 );
  lcd.print(" ");
  delay (500);
}
 
void initialiseLCD()
{
 
  lcd.begin (16,2);
 
/* Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
 */
// Goto first column (0 not 1!), first line (0 not 1!),
  lcd.setCursor ( 0, 0 );
// Print at cursor location
  lcd.print("Temp: ");
// Go to first column (0 not 1!), second line (which is 1 not 2!)
  lcd.setCursor ( 13, 0 );
// Print at cursor location
  lcd.print("C");
}
