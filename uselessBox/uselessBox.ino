
#include "Servo.h"
unsigned long LEDprev; //Zeitspeicher für LED
unsigned long ServoArmprev; //Zeitspeicher für Servo
Servo ServoDeckel;
Servo ServoArm;
int ServoArmpos = 0;
int ServoDeckelpos = 0; // min. 0, max. 60, Platz für Arm: 40, Ultraschall: 50
int ServoArmIntervall;
int LEDgruen = 7;
int LEDIntervall = 1000; //1 Sekunde
int LEDStatus = HIGH;

int SchalterPin = 8;
int SchalterStatus;

#define trigger 9  // Arduino Pin an HC-SR04 Trig
#define echo 10     // Arduino Pin an HC-SR04 Echo
long duration=0;
long distance=0;
long distance_old=0;


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

  LEDprev = millis(); //Zeit merken
  ServoArmprev = millis(); // Zeit merken
}

void loop() {
  if (debug) Serial.println("Start loop");
  SchalterStatus=digitalRead(SchalterPin);

  if (SchalterStatus == LOW) //ALSO EINGESCHALTET !!!
  {
    DoLED();  // LED blinken
    if (zufall == 0) // Beim ersten Durchlauf...
      {
        zufall = random(1, 7); //wenn noch kein Zufall gefunden wurde, dann Zahlen von 1 bis "eine Zahl weniger"
        ServoArmprev = millis(); //Zeitzählung ab Schalter betätigung
        ServoArmIntervall =1000 * random(1,2);  //Auf jeden Fall ein paar Sekunden warten
       }
    switch (zufall)
    {
      case 1: // nach ServoArmIntervall Zeit, Deckel heben und ausschalten
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
      case 2: // nach ServoArmIntervall Zeit ohne Deckel heben ausschalten
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {
          ServoArm.write(180);  // Schalter ausschalten
          delay(400);  
          ServoArm.write(0);  // Arm wieder zurück ins Körbchen
          ServoArmprev = millis();
        }
        break;        
      case 3: //  mit dem Deckel klappern
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {
          for (int i = 0; i <= random(1, 2); i++)
          { 
            int winkel = random(25, 35);
            ServoDeckel.write(winkel);
            delay(300); 
            ServoDeckel.write(0);
            delay(300);
            DoLED();  // LED blinken
          }  
          zufall = 2; // beim nächsten Durchlauf ausmachen
        }  
        break;
      case 4: // langsam Deckel auf ,  langsam schnell auschalten, langsam Deckel zu
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {        
          for (ServoDeckelpos = 0; ServoDeckelpos < 45; ServoDeckelpos++) {
            ServoDeckel.write(ServoDeckelpos);
            delay(15);
            DoLED();  // LED blinken
          }
          for (ServoArmpos = 0; ServoArmpos < 120; ServoArmpos++) {
            ServoArm.write(ServoArmpos);
            delay(15);
            }
            DoLED();  // LED blinken
            delay(500);
            ServoArm.write(180);
            delay(400);
            DoLED();  // LED blinken
          for (ServoArmpos = 180   ; ServoArmpos > 1   ; ServoArmpos--) {
            ServoArm.write(ServoArmpos);
            delay(5);
            }
            DoLED();  // LED blinken
          for (ServoDeckelpos = 45 ; ServoDeckelpos >  0; ServoDeckelpos--) {
            ServoDeckel.write(ServoDeckelpos);
            delay(15);
          }
          ServoDeckel.write(0);
        }
        break;
      case 5:
        // langsam Deckel auf ,  langsam Deckel zu, langsam Deckel zu
       if((millis() - ServoArmprev) > ServoArmIntervall)
        {        
          for (ServoDeckelpos = 0; ServoDeckelpos < 45; ServoDeckelpos++) {
            ServoDeckel.write(ServoDeckelpos);
            delay(15);
            DoLED();  // LED blinken
          }
          delay(500);
          DoLED();  // LED blinken
          delay(500);
          for (ServoDeckelpos = 45 ; ServoDeckelpos >  0; ServoDeckelpos--) {
            ServoDeckel.write(ServoDeckelpos);
            delay(15);
          }
          ServoDeckel.write(0);
          delay(100);
          DoLED();  // LED blinken
          
          ServoDeckel.write(20);
          delay(500);
          DoLED();  // LED blinken
          delay(500);
          DoLED();  // LED blinken
          ServoDeckel.write(0);
          delay(300);
          DoLED();  // LED blinken
          
         zufall = 0; // beim nächsten Durchlauf ausmachen 
        }
        break;
      case 6:
        // Deckel auf, wenn Ultraschall unter 10 cm dann Deckel zu 
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {        
          for (ServoDeckelpos = 0; ServoDeckelpos < 25; ServoDeckelpos++) {
            ServoDeckel.write(ServoDeckelpos);
            delay(20);
          }
          ServoDeckel.write(50);
          DoLED();  // LED blinken
          delay(200);
          
          while (true)
          {
            DoLED();  // LED blinken
            delay(100);
            doUltraschall();
           
            if(distance<=12) 
              { 
                ServoArm.write(0);  // Arm wieder zurück ins Körbchen
                ServoDeckel.write(0);
                ServoArmprev = millis();
                zufall=0;
                break;
              }
          }
          
        }
        break;
      case 7:
        zufall=6;
        break;
      case 8: 
        zufall=6;
        break;
      case 9: 
        //noch nix
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
  if (debug) Serial.println("start DoLED");
  if ((millis() - LEDprev) > LEDIntervall)
  {
    if (debug) Serial.println("LED toggle");
    LEDprev = millis();
    LEDStatus = !LEDStatus;  //Toggeln des LED Status
    digitalWrite(LEDgruen, LEDStatus);
  }
}

void doUltraschall()
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
    
    if(debug) {
      Serial.print(distance);
      Serial.println(" cm");
    }
}
