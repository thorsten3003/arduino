
#include "Servo.h"
unsigned long LEDprev; //Zeitspeicher für LED
unsigned long USprev; //Zeitspeicher für Ultraschall
unsigned long ServoArmprev; //Zeitspeicher für Servo
Servo ServoDeckel;
Servo ServoArm;
int ServoArmpos = 0;
int ServoDeckelpos = 0; // min. 0, max. 60, Platz für Arm: 40, Ultraschall: 50
int ServoArmIntervall;
int LEDgruen = 7;
int LEDIntervall = 700; //1 Sekunde
int LEDStatus = HIGH;
int SchalterPin = 8;
int SchalterStatus;
#define trigger 9  // Arduino Pin an HC-SR04 Trig
#define echo 10     // Arduino Pin an HC-SR04 Echo
long duration=0;
long distance=0;
long distance_old=0;
int z=0;
int zufall = 0; // was Kiste macht ist rein zufällig
boolean debug = true;

void setup() {
  Serial.begin(9600); //For debugging
  randomSeed(analogRead(0)); //für zufällige Zufallszahlen
  pinMode(LEDgruen , OUTPUT);
  pinMode(SchalterPin, INPUT);
  digitalWrite(SchalterPin, HIGH); //Einschalten des internen PullUp Widerstandes (S.193)
  ServoArm.attach(5);
  ServoDeckel.attach(6);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);

  // Nach dem Strom anstecken alles auf 0
  digitalWrite(LEDgruen, LEDStatus);  
  ServoArm.write(0);
  ServoDeckel.write(0);
  
  USprev = millis(); //Zeit merken
  LEDprev = millis(); //Zeit merken
  ServoArmprev = millis(); // Zeit merken
}

void loop() {
  //if (debug) Serial.println("Start loop");
  z=0; // Abbruchbedingungsmerker
  SchalterStatus=digitalRead(SchalterPin);

  if (SchalterStatus == LOW) //ALSO EINGESCHALTET !!!
  {
    digitalWrite(LEDgruen, LOW);
    if (zufall == 0) // Beim ersten Durchlauf...
      {
        zufall = random(2, 11); //wenn noch kein Zufall gefunden wurde, dann Zahlen von 1 bis "eine Zahl weniger"
        ServoArmprev = millis(); //Zeitzählung ab Schalter betätigung
        ServoArmIntervall =100 * random(3 ,30);  //Auf jeden Fall ein paar Sekunden warten
       }
    switch (zufall)
    {
      case 1: // schnell Deckel heben,schnell ausschalten
      if (debug) Serial.println("Case 1");
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {
          ServoDeckel.write(40);
          delay(400);
          ServoArm.write(180);  // Schalter ausschalten
          delay(400);  
          ServoArm.write(0);  // Arm wieder zurück ins Körbchen
          delay(600);
          ServoDeckel.write(0);
          ServoArmprev = millis();
        }
        break;
      case 2: // schnell ohne Deckel heben ausschalten
      if (debug) Serial.println("Case 2");
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {
          ServoArm.write(180);  // Schalter ausschalten
          delay(400);  
          ServoArm.write(0);  // Arm wieder zurück ins Körbchen
          ServoArmprev = millis();
        }
        break;        
      case 3: //  mit dem Deckel klappern
      if (debug) Serial.println("Case 3");
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {
          for (int i = 0; i <= random(1, 4); i++)
          { 
            int winkel = random(25, 35);
            ServoDeckel.write(winkel);
            delay(220); 
            ServoDeckel.write(0);
            delay(220);
          }  
          zufall = 1; // beim nächsten Durchlauf ausmachen
        }  
        break;
      case 4: // langsam Deckel auf ,  langsam - schnell auschalten, langsam Deckel zu
      if (debug) Serial.println("Case 4");
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {        
          for (ServoDeckelpos = 0; ServoDeckelpos < 50; ServoDeckelpos++) {
            ServoDeckel.write(ServoDeckelpos);
            delay(15);
          }
          delay(100);
          if( doUltraschall() ) break;
              
          for (ServoArmpos = 0; ServoArmpos < 120; ServoArmpos++) {
            ServoArm.write(ServoArmpos);
            delay(15);
              if( doUltraschall() ) { z=1; break; }
            }
              if(z==1) break; // den case abbrechen
              
            delay(500);
          if( doUltraschall() ) break;
            ServoArm.write(180);
            delay(400);
          if( doUltraschall() ) break;
          for (ServoArmpos = 180   ; ServoArmpos > 1   ; ServoArmpos--) {
            ServoArm.write(ServoArmpos);
            delay(5);
              if( doUltraschall() ) { z=1; break; }
            }
              if(z==1) break; // den case abbrechen
                      
          for (ServoDeckelpos = 45 ; ServoDeckelpos >  0; ServoDeckelpos--) {
            ServoDeckel.write(ServoDeckelpos);
            delay(15);
          }
          ServoDeckel.write(0);
        }
        break;
      case 5: // langsam Deckel auf ,  langsam Deckel zu, schnell Deckel auf, schnell Deckel zu
      if (debug) Serial.println("Case 5");
       if((millis() - ServoArmprev) > ServoArmIntervall)
        {        
          for (ServoDeckelpos = 0; ServoDeckelpos < 50; ServoDeckelpos++) {
            ServoDeckel.write(ServoDeckelpos);
            delay(15);
          }
          if( doUltraschall() ) break;
            delay(500);
          if( doUltraschall() ) break;
            delay(500);
          if( doUltraschall() ) break;
          
          for (ServoDeckelpos = 50 ; ServoDeckelpos >  0; ServoDeckelpos--) {
            ServoDeckel.write(ServoDeckelpos);
            delay(15);
          }
          ServoDeckel.write(0);
          delay(100);
          
          ServoDeckel.write(20);
          delay(800);
          ServoDeckel.write(0);
          delay(300);
          
         zufall = 1; // beim nächsten Durchlauf ausmachen 
        }
        break;
      case 6:  // Deckel auf, wenn Ultraschall unter 12 cm dann Deckel zu 
      if (debug) Serial.println("case 6");
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {        
          for (ServoDeckelpos = 0; ServoDeckelpos < 50; ServoDeckelpos++) {
            ServoDeckel.write(ServoDeckelpos);
            delay(25);
          }
          ServoDeckel.write(50);
          z=0;
          while (z<100)
          {
            DoLED(); //blinken
            z++;
            delay(100);
            if( doUltraschall() ) break;
          }
        }
        zufall=0;
        break;
      case 7: //langsam auf, US, langsam Arm raus, US , langsam Arm rein, langsam Deckel zu
     if (debug) Serial.println("Case 7"); 
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {        
          for (ServoDeckelpos = 0; ServoDeckelpos < 50; ServoDeckelpos++) {
            ServoDeckel.write(ServoDeckelpos);
            delay(15);
          }
          if( doUltraschall() ) break;
              
          for (ServoArmpos = 0; ServoArmpos < 180; ServoArmpos++) {
            ServoArm.write(ServoArmpos);
            delay(15);
            if( doUltraschall() ) { z=1; break; }
            }
              if(z==1) break; // den case abbrechen
            
            ServoArm.write(180);
            if( doUltraschall() ) break;
          for (ServoArmpos = 180   ; ServoArmpos > 1   ; ServoArmpos--) {
            ServoArm.write(ServoArmpos);
            delay(15);
            if( doUltraschall() ) { z=1; break; }
            }
              if(z==1) break; // den case abbrechen
            
          for (ServoDeckelpos = 50 ; ServoDeckelpos >  0; ServoDeckelpos--) {
            ServoDeckel.write(ServoDeckelpos);
            delay(15);
          }
          ServoDeckel.write(0);
        }
        break;
        case 8: // langsam Deckel auf ,  langsam - schnell auschalten, langsam Deckel zu
        if (debug) Serial.println("Case 8");
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {        
          for (ServoDeckelpos = 0; ServoDeckelpos < 50; ServoDeckelpos++) {
            ServoDeckel.write(ServoDeckelpos);
            delay(5);
          }
          delay(100);
          if( doUltraschall() ) break;
              
          for (ServoArmpos = 0; ServoArmpos < 165; ServoArmpos++) {
            ServoArm.write(ServoArmpos);
            delay(15);
            if( doUltraschall() ) { z=1; break; }
            }
              if(z==1) break; // den case abbrechen
              
            delay(500);
          if( doUltraschall() ) break;
            delay(500);
          if( doUltraschall() ) break;
            ServoArm.write(180);
            delay(400);
            
            for(int i=0; i<10; i++) {
              if( doUltraschall() ) {
                z=1; 
                break; // die for schleife abbrechen
              }
              delay(100);
            }
            if(z==1) break; // den case abbrechen
            
          for (ServoArmpos = 180   ; ServoArmpos > 1   ; ServoArmpos--) {
            ServoArm.write(ServoArmpos);
            delay(3);
            if( doUltraschall() ) { z=1; break; }
            }
              if(z==1) break; // den case abbrechen
              
          for (ServoDeckelpos = 50 ; ServoDeckelpos >  0; ServoDeckelpos--) {
            ServoDeckel.write(ServoDeckelpos);
            delay(3);
          }
          ServoDeckel.write(0);
        }
        break;
      case 9: //langsam auf, US, langsam Arm raus, US , langsam Arm rein, schnell Deckel zu
     if (debug) Serial.println("Case 9"); 
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {        
          for (ServoDeckelpos = 0; ServoDeckelpos < 50; ServoDeckelpos++) {
            ServoDeckel.write(ServoDeckelpos);
            delay(15);
          }
          if( doUltraschall() ) break;
              
          for (ServoArmpos = 0; ServoArmpos < 180; ServoArmpos++) {
            ServoArm.write(ServoArmpos);
            delay(15);
            if( doUltraschall() ) { z=1; break; }
            }
              if(z==1) break; // den case abbrechen
              
            ServoArm.write(180);

          for (ServoArmpos = 180   ; ServoArmpos > 1   ; ServoArmpos--) {
            ServoArm.write(ServoArmpos);
            delay(15);
            if( doUltraschall() ) { z=1; break; }
            }
              if(z==1) break; // den case abbrechen
              
          ServoDeckel.write(0);
        }
        break;      
      case 10: // schnell ohne Deckel heben ausschalten
      if (debug) Serial.println("Case 10");
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {
          ServoArm.write(170);  // Schalter ausschalten
          delay(1500); 
          ServoDeckel.write(40);
          delay(1500); 
          ServoDeckel.write(50);  
            for(int i=0; i<20; i++) {
              if( doUltraschall() ) {
                z=1; 
                break; // die for schleife abbrechen
              }
              delay(100);
            }
            if(z==1) break; // den case abbrechen

          ServoArm.write(180);  
          delay(800);
          ServoArm.write(0);
          delay(200);
          ServoDeckel.write(0);
        }
        break;
    }


  }
  else  //Schalter aus
  {
    LEDStatus = HIGH;
    zufall = 0; // Zufall wieder zurücksetzen
    digitalWrite(LEDgruen, LEDStatus);   // LED aus
    ServoArm.write(0);  // Arm wieder zurück ins Körbchen
    ServoDeckel.write(0);  // Deckel zu
  }
} //Ende Loop

void DoLED()
{
  //if (debug) Serial.println("start DoLED");
  if ((millis() - LEDprev) > LEDIntervall)
  {
    if (debug) Serial.println("LED toggle");
    LEDprev = millis();
    LEDStatus = !LEDStatus;  //Toggeln des LED Status
    digitalWrite(LEDgruen, LEDStatus);
  }
}

boolean doUltraschall()
{
  if ((millis() - USprev) > 100)
  {
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
    
    USprev = millis();
    if(debug) {
      Serial.print(distance);
      Serial.println(" cm");
    }      
      if(distance<=12)
      { 
        ServoArm.write(0);  // Arm wieder zurück ins Körbchen
        ServoDeckel.write(0);
        ServoArmprev = millis();
        zufall=0;
        DoLED();  //LED schonmal aus
        delay(4000);
        return(1);
      }
  }
  return(0);
}
