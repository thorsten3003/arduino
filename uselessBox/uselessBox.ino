
#include "Servo.h"
unsigned long LEDprev; //Zeitspeicher für LED
unsigned long ServoArmprev; //Zeitspeicher für Servo
Servo ServoDeckel;
Servo ServoArm;
int ServoArmpos = 0;
int ServoDeckelpos = 0;
int ServoArmIntervall;
int LEDgruen = 7;
int LEDIntervall = 1000; //1 Sekunde
int LEDStatus = HIGH;

int SchalterPin = 8;
int SchalterStatus;
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
        zufall = random(1, 5); //wenn noch kein Zufall gefunden wurde, dann Zahlen von 1 bis "eine Zahl weniger"
        ServoArmprev = millis(); //Zeitzählung ab Schalter betätigung
        ServoArmIntervall =1000 * random(1,5);  //Auf jeden Fall ein paar Sekunden warten
       }
    switch (zufall)
    {
      case 1: // nach ServoArmIntervall Zeit, Deckel heben und ausschalten
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {
          ServoDeckel.write(65);
          delay(1000); 
          ServoArm.write(180);  // Schalter ausschalten
          delay(400);  
          ServoArm.write(0);  // Arm wieder zurück ins Körbchen
          delay(1000);  
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
          for (int i = 0; i <= random(3, 6); i++)
          { 
            int winkel = random(45, 65);
            ServoDeckel.write(winkel);
            delay(300); 
            ServoDeckel.write(25);
            delay(300);
          }  
            ServoDeckel.write(0);
            delay(500); 
          
          zufall = 1; // beim nächsten Durchlauf ausmachen
        }  
        break;
      case 4: // langsam Deckel auf ,  langsam auschalten, langsam Deckel zu
        if((millis() - ServoArmprev) > ServoArmIntervall)
        {        
          for (ServoDeckelpos = 0; ServoDeckelpos < 75; ServoDeckelpos++) {
            ServoDeckel.write(ServoDeckelpos);
            delay(15);
          }
          for (ServoArmpos = 0; ServoArmpos < 180; ServoArmpos++) {
            ServoArm.write(ServoArmpos);
            delay(15);
          }
          for (ServoArmpos = 180   ; ServoArmpos > 1   ; ServoArmpos--) {
            ServoArm.write(ServoArmpos);
            delay(15);
          }
          for (ServoDeckelpos = 75 ; ServoDeckelpos >  15; ServoDeckelpos--) {
            ServoDeckel.write(ServoDeckelpos);
            delay(15);
          }
        }
        break;
      case 5:
        // langsam Deckel auf ,  langsam auschalten, langsam Deckel zu

        break;
      case 6:
        // langsam Deckel auf ,  schnell auschalten, schnell Deckel zu

        break;
      case 7:
        // Deckel auf, bei Annäherung schnell auschalten, schnell Deckel zu
        break;
      case 8: // Deckel auf, Ultraschall bei Annäherung Deckel zu, Entfernung merken
        // 3 Sekunden warten , Spielchen wiederholen
        // Nach 3 -4 mal Schalter ausmachen

        break;
      case 9:  // TESTEN
        for (ServoArmpos = 0; ServoArmpos < 180; ServoArmpos++) {
          ServoArm.write(ServoArmpos);
          delay(15);
        }
        for (ServoArmpos    ; ServoArmpos > 0   ; ServoArmpos--) {
          ServoArm.write(ServoArmpos);
          delay(15);
        }

        for (ServoDeckelpos = 0; ServoDeckelpos < 50; ServoDeckelpos++) {
          ServoDeckel.write(ServoDeckelpos);
          delay(15);
        }
        for (ServoDeckelpos    ; ServoDeckelpos >  0; ServoDeckelpos--) {
          ServoDeckel.write(ServoDeckelpos);
          delay(15);
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


}



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
