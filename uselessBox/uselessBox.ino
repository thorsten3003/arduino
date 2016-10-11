
#include "Servo.h"
unsigned long LEDprev; //Zeitspeicher für LED
unsigned long ServoArmprev; //Zeitspeicher für Servo
Servo ServoDeckel;
Servo ServoArm;
int ServoArmIntervall=15000; //15 Sekunden

int LEDgruen=3;
int LEDIntervall = 1000; //1 Sekunde
int LEDStatus = LOW;

int SchalterPin = 4;
int zufall = 0; // was Kiste macht ist rein zufällig

void setup() {
  randomSeed(analogRead(0)); //für zufällige Zufallszahlen
  pinMode(LEDgruen ,OUTPUT);
  pinMode(SchalterPin,INPUT);
  digitalWrite(SchalterPin,HIGH); //Einschalten des internen PullUp Widerstandes
  ServoDeckel.attach(5);
  ServoArm.attach(6);

  // Nach dem Strom anstecken alles auf 0
  digitalWrite(LEDgruen, LEDStatus);  //LOW
  ServoArm.write(0);
  ServoDeckel.write(0); 

  LEDprev=millis();  //Zeit merken
  ServoArmprev=millis(); // Zeit merken
}

void loop() {
  if(SchalterPin==LOW)  //ALSO EINGESCHALTET !!!
    {
      // LED blinken
      if((millis() - LEDprev) > LEDIntervall)
        {
          LEDprev = millis();
          LEDStatus = !LEDStatus;  //Toggeln des LED Status
          digitalWrite(LEDgruen, LEDStatus); 
        }
      //Random was machen
      if(zufall == 0) int zufall=random(1,2); //wenn noch kein Zufall gefunden wurde, dann Zahlen von 1 bis "eine Zahl weniger"
      
      switch (zufall)
        {
          case 1:
                        // nach ServoArmIntervall Zeit ausschalten
                        if((millis() - ServoArmprev) > ServoArmIntervall)
                          {
                            ServoArmprev = millis();
                            ServoArm.write(180);  // Schalter ausschalten
                            delay(500);  //0,5 Sekunden Zeit geben
                            ServoArm.write(0);  // Arm wieder zurück ins Körbchen
                          }
          break;
          case 2:
                        //  mit dem Deckel klappern
                        for (int i = 0; i <= random(3,6); i++)
                        {
                          ServoDeckel.write(0); 
                          delay(200); // 0,2 Sekunden  
                          ServoDeckel.write(42); 
                          delay(random(2000,8001)); // 2 bis 8 Sekunden 
                        }
                        zufall =0; // beim nächsten Durchlauf was anderes
          break;
          case 3:
                      // Deckel auf, Ultraschall bei Annäherung Deckel zu, Entfernung merken
                      // 3 Sekunden warten , Spielchen wiederholen
                      // Nach 3 -4 mal Schalter ausmachen
          break;
          case 4:
                      // langsam Deckel auf ,  langsam auschalten, langsam Deckel zu
          
          break;
          case 5:
                      // langsam Deckel auf ,  schnell auschalten, schnell Deckel zu
          
          break;
          case 6:
                    // Deckel auf, bei Annäherung schnell auschalten, schnell Deckel zu
          break;
          case 7:
          
          break;
          case 8:
          
          break; 
          }
          
      
    }
    else  //Schalter aus
    {
       LEDStatus = LOW;
       zufall=0; // Zufall wieder zurücksetzen
       digitalWrite(LEDgruen, LEDStatus);   // LED aus
       ServoArm.write(0);  // Arm wieder zurück ins Körbchen
       ServoDeckel.write(0);  // Deckel zu
    }

}

