#include <OneWire.h>
#include <LiquidCrystal.h>
#include <Bounce2.h>

// LiquidCrystal
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/* DS18S20 Temperature */
OneWire  ds(9);  // on pin 9
#define MAX_DS1820_SENSORS 2
byte addr[MAX_DS1820_SENSORS][8];

//Ultraschall
#define trigger 6  // Arduino Pin an HC-SR04 Trig
#define echo 7     // Arduino Pin an HC-SR04 Echo

//Bounce
#define BUTTON_PIN 10
#define LED_PIN 13

int ledState = LOW;

// Instantiate a Bounce object
Bounce debouncer = Bounce();

//Variablen der Teilprogramme
int Programmnummer=0;
int maxProgrammnummer=5;

unsigned long prev; // Previous Time fuer Wartezeit
int intervall=1000; // 1 Sekunde Wartezeit

void setup(void) 
{
  pinMode(trigger, OUTPUT); //Ultraschall
  pinMode(echo, INPUT);     //Ultraschall

  pinMode(BUTTON_PIN,INPUT);      // Setup the button
  digitalWrite(BUTTON_PIN,HIGH);  // Activate internal pull-up
    
  // After setting up the button, setup debouncer
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(50);
  
  //Setup the LED
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,ledState);
    
  //Setup LCD
  lcd.begin(16, 2);
  lcd.clear();
  
  // Setup DS1820
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
    
    prev=millis();
}

//Variablen DS1820
int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
char buf[20];

long duration=0;      //Ultraschall
long distance=0;      //Ultraschall
long distance_old=0;  //Ultraschall

void loop() 
{
  TasterAbfragen();
  switch(Programmnummer)
{
   case 1: 
    lcd.setCursor(0,0);
    lcd.print("Programm 1");
          
   break;

   case 2:
    lcd.setCursor(0,0);
    //lcd.print("Ultraschall");
     showDistance();
   break;

   case 3:
     readTemp();
   break;
   
   case 4:
     
     lcd.setCursor(0,0);
     lcd.print("Programm 4");
   break;
   
   case 5:
     
     lcd.setCursor(0,0);
     lcd.print("Programm 5");
   break;
   
   default:
     
     lcd.setCursor(0,1);
     lcd.print("... idle ...");
}
  
} 



void TasterAbfragen()
{
  boolean stateChanged = debouncer.update();
  int state = debouncer.read();
  
  // Detect the falling edge
   if ( stateChanged && state == LOW ) {
      Programmnummer++;
      if(Programmnummer>maxProgrammnummer) 
        Programmnummer=1;
        lcd.clear();
      
      
                 //DEBUG
                 if ( ledState == LOW ) {
                   ledState = HIGH;
                 } else {
                   ledState = LOW;
                 }
                 digitalWrite(LED_PIN,ledState);     
 }
}

void showDistance()
{
  //while(true){
      
  digitalWrite(trigger, LOW);  
  delayMicroseconds(2); 

  digitalWrite(trigger, HIGH);  
  delayMicroseconds(10);

  digitalWrite(trigger, LOW);
  duration = pulseIn(echo, HIGH); // Echo-Zeit messen

  // Echo-Zeit halbieren (weil hin und zurueck, der doppelte Weg ist)
  duration = (duration/2); 
  // Zeit des Schalls durch Luft in Zentimeter umrechnen
  distance = duration / 29.1;

  // Anzeige nur Aendern wenn neue Entfernung
  if(distance!=distance_old){ 
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Entfernung: ");
     lcd.setCursor(0,1);
    lcd.print(distance);
    lcd.print(" cm");
    }

  distance_old=distance; // letze Entfernung speichern
  delay(200);
// }
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
