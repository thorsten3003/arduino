
#include "Servo.h"
unsigned long LEDprev; //Zeitspeicher für LED
unsigned long USprev; //Zeitspeicher für Ultraschall
unsigned long ServoArmprev; //Zeitspeicher für Servo
Servo ServoDeckel;
Servo ServoArm;
int ServoArmpos = 0;  // min. 0, max. 18
int ServoDeckelpos = 0; // min. 0, max. 60, Platz für Arm: 40, Ultraschall: 50
int ServoArmIntervall;
int LEDgruen = 7;
int LEDIntervall = 700; //1 Sekunde
int LEDStatus = HIGH;
int SchalterPin = 8;
int SchalterStatus;
#define trigger 9  // Arduino Pin an HC-SR04 Trig
#define echo 10     // Arduino Pin an HC-SR04 Echo
long duration = 0;
long distance = 0;
long distance_old = 0;
int zufall = 0; // was Kiste macht ist rein zufällig
int AusZufall = 1;
boolean debug = true;
int Ultraschalldeckel=53; //Deckelstellung ab welcher mit US Messung begonnen wird
boolean z;  // Abbruchbedingung

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
  z = false;
  SchalterStatus = digitalRead(SchalterPin);
  if (SchalterStatus == LOW) //ALSO EINGESCHALTET !!!
  {
    digitalWrite(LEDgruen, LOW); //LED An
    if (zufall == 0) // Beim ersten Durchlauf...
    {
      zufall = random(1,12);//(1, 11); //wenn noch kein Zufall gefunden wurde, dann Zahlen von 1 bis "eine Zahl weniger" 1, 11
      ServoArmprev = millis(); //Zeitzählung ab Schalter betätigung
      ServoArmIntervall = 100 * random(1 , 30); //Auf jeden Fall ein paar Sekunden warten
    }

    switch (zufall)
    {
      case 1:  // Deckel auf, bis Ultraschall unter 12 cm dann Deckel zu
        if (debug) Serial.println("case 1");
        BewegeDeckel(55, 25, true); // Stopposition, speed(delay) in ms
        delay(100);
        StelleDeckel(53, 20);  //Winkel , Wartezeit in ms
        while ( ! doUltraschall() )
        {
          SchalterStatus = digitalRead(SchalterPin);
          if (SchalterStatus == HIGH) break;
          
          if ((millis() - ServoArmprev) > 120000) { break; }
        }
        zufall = 0;
        break;
      case 2: // schnell Deckel heben,schnell ausschalten
        if (debug) Serial.println("Case 2");
        if ((millis() - ServoArmprev) > 200)
        {
          StelleDeckel(43, 100); //Winkel , Wartezeit in ms
          SchalterAUS(); //Wartet bis der Schalter aus ist
          StelleArm(0, 400); //Winkel , Wartezeit in ms
          Grundstellung();
        }
        break;

      case 3: // schnell ohne Deckel heben ausschalten
        if (debug) Serial.println("Case 3");
        if ((millis() - ServoArmprev) > 170)
        {
          SchalterAUS();
          StelleArm(0, 0); //Winkel , Wartezeit in ms
          Grundstellung();
        }
        break;

      case 4: //  mit dem Deckel klappern
        if (debug) Serial.println("Case 4");
        if ((millis() - ServoArmprev) > ServoArmIntervall)
        {
          for (int i = 0; i <= random(1, 4); i++)
          {
            int winkel = random(25, 35);
            StelleDeckel(winkel, 220); //Winkel , Wartezeit in ms
            StelleDeckel(0, 220); //Winkel , Wartezeit in ms
          }
          Grundstellung(); // beim nächsten Durchlauf ausmachen
        }
        break;

      case 5: // langsam Deckel auf ,  langsam - schnell auschalten, langsam Deckel zu
        if (debug) Serial.println("Case 5");
        if ((millis() - ServoArmprev) > ServoArmIntervall)
        {
          BewegeDeckel(55, 15, false); //  Stopposition, speed(delay) in ms  , Ultraschall
          StelleDeckel(53, 0); //Winkel , Wartezeit in ms
          BewegeArm(120, 15, false); // Stopposition, speed(delay) in ms  , Ultraschall
          delay(500);
          SchalterAUS();
          BewegeArm(0, 5, false); //  Stopposition, speed(delay) in ms  , Ultraschall
          BewegeDeckel(0, 15, false); // Stopposition, speed(delay) in ms
          Grundstellung();
        }
        break;

      case 6: // langsam Deckel auf ,  langsam Deckel zu, schnell Deckel auf, schnell Deckel zu
        if (debug) Serial.println("Case 6");
        if ((millis() - ServoArmprev) > 3500)
        {
          BewegeDeckel(50, 25, false); // Stopposition, speed(delay) in ms
          delay(1000);
          BewegeDeckel(0, 22, false); // Stopposition, speed(delay) in ms
          delay(600);
          StelleDeckel(45, 1500);  //Winkel , Wartezeit in ms
          StelleDeckel(0, 3000);  //Winkel , Wartezeit in ms
          zufall = 3; // beim nächsten Durchlauf ausmachen
        }
        break;


      case 7: //langsam auf, langsam Arm, langsam Arm rein, langsam Deckel zu
        if (debug) Serial.println("Case 7");
        if ((millis() - ServoArmprev) > ServoArmIntervall)
        {
          BewegeDeckel(55, 15, false); //  Stopposition, speed(delay) in ms
          StelleDeckel(53, 0);  //Winkel , Wartezeit in ms
          BewegeArm(180, 15, false); // Stopposition, speed(delay) in ms  , Ultraschall
          SchalterAUS();
          BewegeArm(0, 15, false); // Stopposition, speed(delay) in ms  , Ultraschall
          BewegeDeckel(0, 15, false); // Stopposition, speed(delay) in ms
          Grundstellung();
        }
        break;

      case 8: // langsam Deckel auf ,  langsam - schnell auschalten, langsam Deckel zu
        if (debug) Serial.println("Case 8");
        if ((millis() - ServoArmprev) > ServoArmIntervall)
        {
          BewegeDeckel(55, 10, false); //  Stopposition, speed(delay) in ms
          StelleDeckel(53, 0);  //Winkel , Wartezeit in ms
          BewegeArm(165, 15, false); // Stopposition, speed(delay) in ms  , Ultraschall
          delay(1200);
          SchalterAUS();
          BewegeArm(0, 3, false); // Stopposition, speed(delay) in ms  , Ultraschall
          BewegeDeckel(0, 3, false); // Stopposition, speed(delay) in ms
          Grundstellung();
        }
        break;

      case 9: //langsam auf, US, langsam Arm raus, US , langsam Arm rein, schnell Deckel zu
        if (debug) Serial.println("Case 9");
        if ((millis() - ServoArmprev) > 5000)
        {
          BewegeDeckel(55, 20, false); //  Stopposition, speed(delay) in ms
          StelleDeckel(53, 0);  //Winkel , Wartezeit in ms
          BewegeArm(180, 15, false); // Stopposition, speed(delay) in ms  , Ultraschall
          SchalterAUS();
          BewegeArm(0, 15, false); //  Stopposition, speed(delay) in ms  , Ultraschall
          Grundstellung();
        }
        break;

      case 10: //
        if (debug) Serial.println("Case 10");
        if ((millis() - ServoArmprev) > 200)
        {
          StelleArm(170, 1500); //Winkel , Wartezeit in ms
          StelleDeckel(50, 1500); //Winkel , Wartezeit in ms
          SchalterAUS();
          delay(800);
          StelleArm(0, 200); //Winkel , Wartezeit in ms
          Grundstellung();
        }
        break;
      case 11: //
        if (debug) Serial.println("Case 11");
        if ((millis() - ServoArmprev) > 400)
        {
          StelleArm(170, 1500); //Winkel , Wartezeit in ms
          BewegeDeckel(55, 20, false); //Winkel , Wartezeit in ms, US
          StelleDeckel(53, 0); //Winkel , Wartezeit in ms
          delay(1500);
          SchalterAUS();
          delay(1000);
          StelleArm(0, 300); //Winkel , Wartezeit in ms
          Grundstellung();
        }
        break;
    }

  }
  else  //Schalter aus
  { if (debug) Serial.println("else  //Schalter aus");
    if ((millis() - ServoArmprev) > 1200000) //12000 Sekunden?
    {
      AusZufall = random(1, 3);
      switch (AusZufall)
      {
        case 1:  // Deckel klappern
          if (debug) Serial.println("case Aus1");
          for (int i = 0; i <= random(1, 4); i++)
          {
            int winkel = random(25, 35);
            StelleDeckel(winkel, 220); //Winkel , Wartezeit in ms
            StelleDeckel(0, 220); //Winkel , Wartezeit in ms
          }
          Grundstellung();
          break;

        case 2:  // Deckel auf, wenn Ultraschall unter 12 cm dann Deckel zu
          if (debug) Serial.println("case Aus2");
          BewegeDeckel(55, 25, true); // Stopposition, speed(delay) in ms
          StelleDeckel(53, 100);  //Winkel , Wartezeit in ms
          while ( ! doUltraschall() ) {}
          Grundstellung();
          break;
      }


      LEDStatus = HIGH;
      digitalWrite(LEDgruen, LEDStatus);   // LED aus
      Grundstellung();
    }
  }
} //Ende Loop

void Grundstellung()
{
  StelleArm(0, 0); //Winkel , Wartezeit in ms
  StelleDeckel(0, 0); //Winkel , Wartezeit in ms
  ServoArmprev = millis();
  zufall = 0;
}
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
  if ((millis() - USprev) > 40)
  {
    duration = 0;
    digitalWrite(trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
    duration = pulseIn(echo, HIGH); // Echo-Zeit messen
    // Echo-Zeit halbieren (weil hin und zurueck, der doppelte Weg ist)
    duration = (duration / 2);
    // Zeit des Schalls durch Luft in Zentimeter umrechnen
    distance = duration / 29.1;

    USprev = millis();
    if (debug) {
      Serial.print(distance);
      Serial.println(" cm");
    }
    if (distance > 0 && distance <= 12)
    {
      z=true;
      Grundstellung();
      digitalWrite(LEDgruen, HIGH); //LED  aus
      delay(1000);
      exit(0);
      return (1);
    }
    return (0);
  }
}

void StelleDeckel(int pos, int wartezeitms) //Winkel , Wartezeit in ms
{
  if ( pos >= 0 && pos <= 60 )
  {
    ServoDeckelpos = pos;
    ServoDeckel.write(ServoDeckelpos);
    delay(wartezeitms);
  }
}

void StelleArm(int pos, int wartezeitms) //Winkel , Wartezeit in ms
{
  if ( pos >= 0 && pos <= 180 )
  {
    ServoArmpos = pos;
    ServoArm.write(ServoArmpos);
    delay(wartezeitms);
  }
}

void SchalterAUS()
{
  while (!SchalterStatus)
  {
    ServoArm.write(180);  // Schalter ausschalten
    ServoArmpos=180;
    SchalterStatus = digitalRead(SchalterPin);
  }
  digitalWrite(LEDgruen, HIGH); //LED  aus
}

void BewegeDeckel(int stoppos, int speedms, boolean Ultraschall)
{
  if ( stoppos >= 0 && stoppos <= 60 )
  {
    if (ServoDeckelpos < stoppos) // Deckel langsam öffnen
    {
      for (ServoDeckelpos; ServoDeckelpos < stoppos + 2; ServoDeckelpos++)
      {
        if(ServoDeckelpos >= Ultraschalldeckel && Ultraschall==true ) { doUltraschall(); }
        StelleDeckel(ServoDeckelpos, speedms); //Winkel , Wartezeit in ms
      }
      StelleDeckel(stoppos, 0); //Winkel , Wartezeit in ms
    }
    else //Deckel langsam schließen
    {
      for (ServoDeckelpos; ServoDeckelpos > stoppos; ServoDeckelpos--)
      {
        if(ServoDeckelpos >= Ultraschalldeckel && Ultraschall==true ) { doUltraschall(); }
        StelleDeckel(ServoDeckelpos, speedms); //Winkel , Wartezeit in ms
      }
      StelleDeckel(stoppos, 0); //Winkel , Wartezeit in ms
    }
  }
}

void BewegeArm(int stoppos, int speedms, boolean Ultraschall)
{
  if ( stoppos >= 0 && stoppos <= 180 )
  {
    if (ServoArmpos < stoppos) // Arm langsam raus
    {
      for (ServoArmpos; ServoArmpos < stoppos; ServoArmpos++)
      {
        if(ServoDeckelpos >= Ultraschalldeckel && Ultraschall==true ) { doUltraschall(); }
        StelleArm(ServoArmpos, speedms); //Winkel , Wartezeit in ms
      }
      StelleArm(stoppos, 0); //Winkel , Wartezeit in ms
    }
    else //Arm langsam rein
    {
      for (ServoArmpos; ServoArmpos > stoppos; ServoArmpos--)
      {
        if(ServoDeckelpos >= Ultraschalldeckel && Ultraschall==true ) { doUltraschall(); }
        StelleArm(ServoArmpos, speedms); //Winkel , Wartezeit in ms
      }
      StelleArm(stoppos, 0); //Winkel , Wartezeit in ms
    }
  }

}
