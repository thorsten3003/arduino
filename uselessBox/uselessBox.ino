
#include "Servo.h"
#include "musical_notes.h"
#include "pitches.h"
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
int speakerPin = 11; // speaker connected to digital pin 6
#define trigger 9  // Arduino Pin an HC-SR04 Trig
#define echo 10     // Arduino Pin an HC-SR04 Echo
long duration = 0;
long distance = 0;
long distance_old = 0;
int zufall = 0; // was Kiste macht ist rein zufällig
int AusZufall = 1;
boolean debug = true;
int Ultraschalldeckel = 53; //Deckelstellung ab welcher mit US Messung begonnen wird
int UltraschallAlarm; // Zähler
int z;  // Abbruchbedingung
int jinglebells[] = { NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_G3, NOTE_C3, NOTE_D3, NOTE_E3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_G3, NOTE_F3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_D3, NOTE_D3, NOTE_E3, NOTE_D3, NOTE_G3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_G3, NOTE_C3, NOTE_D3, NOTE_E3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_G3, NOTE_F3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_G3, NOTE_G3, NOTE_F3, NOTE_D3, NOTE_C3};
int jinglebellsdauer[] = { 4, 4, 2, 4, 4, 2, 4, 4, 4, 4 , 1, 4, 4, 4, 4, 4, 4, 4, 8, 8, 4, 4, 4, 4, 2, 2, 4, 4, 2, 4, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 8, 8, 4, 4, 4, 4, 1};
int jinglebellsanzahlToene = 52;
int Spieluhr = 0;

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

  pinMode(speakerPin, OUTPUT); // sets the speakerPin to be an output
  // Nach dem Strom anstecken alles auf 0
  digitalWrite(LEDgruen, LEDStatus);
  ServoArm.write(0);
  ServoDeckel.write(0);

  USprev = millis(); //Zeit merken
  LEDprev = millis(); //Zeit merken
  ServoArmprev = millis(); // Zeit merken
  UltraschallAlarm = 0;
  zufall = 0;
  z = 0;

  // Auswahl Spieluhr oder useless Box
  digitalWrite(LEDgruen, LOW); //LED An
  delay(4000);
  SchalterStatus = digitalRead(SchalterPin);
  if (SchalterStatus == LOW) //ALSO EINGESCHALTET !!!
  {
    Spieluhr = 1;
    squeak();
    delay(4000);
  }
  Grundstellung();
  digitalWrite(LEDgruen, HIGH); //LED Aus
}

void loop() {
  SchalterStatus = digitalRead(SchalterPin);
  if (SchalterStatus == LOW) //ALSO EINGESCHALTET !!!
  {
    digitalWrite(LEDgruen, LOW); //LED An
    if (Spieluhr == 1)
    {
      zufall = 12;
    }


    if (zufall == 0) // Beim ersten Durchlauf...
    {
      zufall = random(1, 13); //(1, 11); //wenn noch kein Zufall gefunden wurde, dann Zahlen von 1 bis "eine Zahl weniger" 1, 11
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

          if ((millis() - ServoArmprev) > 120000) {
            break;
          }
        }
        ohhh();
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
          BewegeArm(120, 15, true); // Stopposition, speed(delay) in ms  , Ultraschall
          if (z == 1)
          { Serial.println("z == 1");
            z = 0; Grundstellung();
          }
          else
          { Serial.println("else Teil z == 0");
            ohhh();   //delay(500);
            SchalterAUS();
            BewegeArm(0, 5, false); //  Stopposition, speed(delay) in ms  , Ultraschall
            BewegeDeckel(0, 15, false); // Stopposition, speed(delay) in ms
          }
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
          delay(2000);
        }
        break;


      case 7: //langsam auf, langsam Arm, langsam Arm rein, langsam Deckel zu
        if (debug) Serial.println("Case 7");
        if ((millis() - ServoArmprev) > ServoArmIntervall)
        {
          BewegeDeckel(55, 15, false); //  Stopposition, speed(delay) in ms
          StelleDeckel(53, 0);  //Winkel , Wartezeit in ms
          BewegeArm(180, 15, true); // Stopposition, speed(delay) in ms  , Ultraschall
          if (z == 1)
          { Serial.println("z == 1");
            z = 0; Grundstellung();
          }
          else
          { Serial.println("else Teil z == 0");
            SchalterAUS();
            BewegeArm(0, 15, false); // Stopposition, speed(delay) in ms  , Ultraschall
            BewegeDeckel(0, 15, false); // Stopposition, speed(delay) in ms
          }
          Grundstellung();
        }
        break;

      case 8: // langsam Deckel auf ,  langsam - schnell auschalten, langsam Deckel zu
        if (debug) Serial.println("Case 8");
        if ((millis() - ServoArmprev) > ServoArmIntervall)
        {
          BewegeDeckel(55, 10, false); //  Stopposition, speed(delay) in ms
          StelleDeckel(53, 0);  //Winkel , Wartezeit in ms
          BewegeArm(165, 15, true); // Stopposition, speed(delay) in ms  , Ultraschall
          if (z == 1)
          { Serial.println("z == 1");
            z = 0; Grundstellung();
          }
          else
          { Serial.println("else Teil z == 0");
            delay(1200);
            SchalterAUS();
            BewegeArm(0, 3, false); // Stopposition, speed(delay) in ms  , Ultraschall
            BewegeDeckel(0, 3, false); // Stopposition, speed(delay) in ms
          }
          Grundstellung();
        }
        break;

      case 9: //langsam auf, US, langsam Arm raus, US , langsam Arm rein, schnell Deckel zu
        if (debug) Serial.println("Case 9");
        if ((millis() - ServoArmprev) > 5000)
        {
          BewegeDeckel(55, 20, false); //  Stopposition, speed(delay) in ms
          StelleDeckel(53, 0);  //Winkel , Wartezeit in ms
          BewegeArm(180, 15, true); // Stopposition, speed(delay) in ms  , Ultraschall
          if (z == 1)
          { Serial.println("z == 1");
            z = 0; Grundstellung();
          }
          else
          { Serial.println("else Teil z == 0");
            SchalterAUS();
            BewegeArm(0, 15, false); //  Stopposition, speed(delay) in ms  , Ultraschall
          }
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

      case 12: // Spieluhr
        if (Spieluhr == 1)  //Nur ausführen wenn auch Spieluhr gewählt
        {
          if (debug) Serial.println("Case Spieluhr");
          if ((millis() - ServoArmprev) > 4000)
          {
            BewegeDeckel(15, 25, true); // Stopposition, speed(delay) in ms
            StelleDeckel(13, 0);  //Winkel , Wartezeit in ms
            play(jinglebells, jinglebellsdauer, jinglebellsanzahlToene);
            if (SchalterStatus == LOW) //Eingeschaltet
              {
                BewegeDeckel(48, 15, false); //  Stopposition, speed(delay) in ms
                StelleDeckel(46, 0);  //Winkel , Wartezeit in ms
                BewegeArm(180, 15, true); // Stopposition, speed(delay) in ms  , Ultraschall
                SchalterAUS();
                BewegeArm(0, 15, false); // Stopposition, speed(delay) in ms  , Ultraschall
              }  
            BewegeDeckel(0, 15, false); // Stopposition, speed(delay) in ms
            Grundstellung();
            LEDStatus = HIGH;
            digitalWrite(LEDgruen, LEDStatus);   // LED aus
          }
        }
    }
  }
  else  //Schalter aus
  { if (debug) Serial.println("else - Schalter aus");
    if ((millis() - ServoArmprev) > 1200000) //12000 Sekunden?
    {
      AusZufall = random(1, 5);
      int SoundZufall;
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
          squeak();
          while ( ! doUltraschall() ) {}
          Grundstellung();
          break;

        case 3:  // Sound
          if (debug) Serial.println("case Aus3");
          BewegeDeckel(15, 25, true); // Stopposition, speed(delay) in ms
          StelleDeckel(13, 0);  //Winkel , Wartezeit in ms

          SoundZufall = random(1, 3);
          switch (SoundZufall)
          {
            case 1:
              closeEncounters();
              break;
            case 2:
              ariel();
              break;
          }
          Grundstellung();
          break;

        case 4:  // Sound
          if (debug) Serial.println("case Aus4");
          SoundZufall = random(1, 8);
          switch (SoundZufall)
          {
            case 1:
              squeak();
              break;
            case 2:
              catcall();
              break;
            case 3:
              ohhh();
              break;
            case 4:
              laugh();
              break;
            case 5:
              laugh2();
              break;
            case 6:
              waka();
              break;
            case 7:
              r2D2();
              break;
          }
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
      UltraschallAlarm++;
    }
    else
    {
      UltraschallAlarm = 0;
    }

    if ( UltraschallAlarm > 2 )
    {
      z = 1;
      Grundstellung();
      digitalWrite(LEDgruen, HIGH); //LED  aus
      delay(1000);
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
    ServoArmpos = 180;
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
        if (ServoDeckelpos >= Ultraschalldeckel && Ultraschall == true ) {
          if (doUltraschall() == 1 ) {
            ServoDeckelpos = stoppos + 2;
            break;
          }
        }
        StelleDeckel(ServoDeckelpos, speedms); //Winkel , Wartezeit in ms
      }
      StelleDeckel(stoppos, 0); //Winkel , Wartezeit in ms
    }
    else //Deckel langsam schließen
    {
      for (ServoDeckelpos; ServoDeckelpos > stoppos; ServoDeckelpos--)
      {
        if (ServoDeckelpos >= Ultraschalldeckel && Ultraschall == true ) {
          if (doUltraschall() == 1 ) {
            ServoDeckelpos = stoppos;
            break;
          }
        }
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
        if (ServoDeckelpos >= Ultraschalldeckel && Ultraschall == true ) {
          if (doUltraschall() == 1 ) {
            ServoArmpos = stoppos;
            break;
          }
        }
        StelleArm(ServoArmpos, speedms); //Winkel , Wartezeit in ms
      }
      StelleArm(stoppos, 0); //Winkel , Wartezeit in ms
    }
    else //Arm langsam rein
    {
      for (ServoArmpos; ServoArmpos > stoppos; ServoArmpos--)
      {
        if (ServoDeckelpos >= Ultraschalldeckel && Ultraschall == true ) {
          if (doUltraschall() == 1 ) {
            ServoArmpos = stoppos;
            break;
          }
        }
        StelleArm(ServoArmpos, speedms); //Winkel , Wartezeit in ms
      }
      StelleArm(stoppos, 0); //Winkel , Wartezeit in ms
    }
  }
}
void beep (int speakerPin, float noteFrequency, long noteDuration)
{
  int x;
  // Convert the frequency to microseconds
  float microsecondsPerWave = 1000000 / noteFrequency;
  // Calculate how many HIGH/LOW cycles there are per millisecond
  float millisecondsPerCycle = 1000 / (microsecondsPerWave * 2);
  // Multiply noteDuration * number or cycles per millisecond
  float loopTime = noteDuration * millisecondsPerCycle;
  // Play the note for the calculated loopTime.
  for (x = 0; x < loopTime; x++)
  {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(microsecondsPerWave);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(microsecondsPerWave);
  }
}

void scale()
{
  beep(speakerPin, note_C7, 500); //C: play the note C for 500ms
  beep(speakerPin, note_D7, 500); //D
  beep(speakerPin, note_E7, 500); //E
  beep(speakerPin, note_F7, 500); //F
  beep(speakerPin, note_G7, 500); //G
  beep(speakerPin, note_A7, 500); //A
  beep(speakerPin, note_B7, 500); //B
  beep(speakerPin, note_C8, 500); //C
}

void r2D2() {
  beep(speakerPin, note_A7, 100); //A
  beep(speakerPin, note_G7, 100); //G
  beep(speakerPin, note_E7, 100); //E
  beep(speakerPin, note_C7, 100); //C
  beep(speakerPin, note_D7, 100); //D
  beep(speakerPin, note_B7, 100); //B
  beep(speakerPin, note_F7, 100); //F
  beep(speakerPin, note_C8, 100); //C
  beep(speakerPin, note_A7, 100); //A
  beep(speakerPin, note_G7, 100); //G
  beep(speakerPin, note_E7, 100); //E
  beep(speakerPin, note_C7, 100); //C
  beep(speakerPin, note_D7, 100); //D
  beep(speakerPin, note_B7, 100); //B
  beep(speakerPin, note_F7, 100); //F
  beep(speakerPin, note_C8, 100); //C
}

void closeEncounters() {
  beep(speakerPin, note_Bb5, 300); //B b
  delay(50);
  beep(speakerPin, note_C6, 300); //C
  delay(50);
  beep(speakerPin, note_Ab5, 300); //A b
  delay(50);
  beep(speakerPin, note_Ab4, 300); //A b
  delay(50);
  beep(speakerPin, note_Eb5, 500); //E b
  delay(500);

  beep(speakerPin, note_Bb4, 300); //B b
  delay(100);
  beep(speakerPin, note_C5, 300); //C
  delay(100);
  beep(speakerPin, note_Ab4, 300); //A b
  delay(100);
  beep(speakerPin, note_Ab3, 300); //A b
  delay(100);
  beep(speakerPin, note_Eb4, 500); //E b
  delay(500);

  beep(speakerPin, note_Bb3, 300); //B b
  delay(200);
  beep(speakerPin, note_C4, 300); //C
  delay(200);
  beep(speakerPin, note_Ab3, 300); //A b
  delay(500);
  beep(speakerPin, note_Ab2, 300); //A b
  delay(550);
  beep(speakerPin, note_Eb3, 500); //E b
}

void ariel() {

  beep(speakerPin, note_C6, 300); //C
  delay(50);
  beep(speakerPin, note_D6, 300); //D
  delay(50);
  beep(speakerPin, note_Eb6, 600); //D#
  delay(250);

  beep(speakerPin, note_D6, 300); //D
  delay(50);
  beep(speakerPin, note_Eb6, 300); //D#
  delay(50);
  beep(speakerPin, note_F6, 600); //F
  delay(250);

  beep(speakerPin, note_C6, 300); //C
  delay(50);
  beep(speakerPin, note_D6, 300); //D
  delay(50);
  beep(speakerPin, note_Eb6, 500); //D#
  delay(50);
  beep(speakerPin, note_D6, 300); //D
  delay(50);
  beep(speakerPin, note_Eb6, 300); //D#
  delay(50);
  beep(speakerPin, note_D6, 300); //D
  delay(50);
  beep(speakerPin, note_Eb6, 300); //D#
  delay(50);
  beep(speakerPin, note_F6, 600); //F
  delay(50);

}


void laugh2() {
  beep(speakerPin, note_C6, 200); //C
  beep(speakerPin, note_E6, 200); //E
  beep(speakerPin, note_G6, 200); //G
  beep(speakerPin, note_C7, 200); //C
  beep(speakerPin, note_C6, 200); //C
  delay(50);
  beep(speakerPin, note_C6, 200); //C
  beep(speakerPin, note_E6, 200); //E
  beep(speakerPin, note_G6, 200); //G
  beep(speakerPin, note_C7, 200); //C
  beep(speakerPin, note_C6, 200); //C
  delay(50);
  beep(speakerPin, note_C6, 50); //C
  delay(50);
  beep(speakerPin, note_C6, 50); //C
  delay(50);
  beep(speakerPin, note_C6, 50); //C
  delay(50);
  beep(speakerPin, note_C6, 50); //C
  delay(50);
  beep(speakerPin, note_C6, 50); //C
  delay(50);
  beep(speakerPin, note_C6, 50); //C
  delay(50);
  beep(speakerPin, note_C6, 50); //C



}

void squeak() {
  for (int i = 100; i < 5000; i = i * 1.45) {
    beep(speakerPin, i, 60);
  }
  delay(10);
  for (int i = 100; i < 6000; i = i * 1.5) {
    beep(speakerPin, i, 20);
  }
}

void waka() {
  for (int i = 1000; i < 3000; i = i * 1.05) {
    beep(speakerPin, i, 10);
  }
  delay(100);
  for (int i = 2000; i > 1000; i = i * .95) {
    beep(speakerPin, i, 10);
  }
  for (int i = 1000; i < 3000; i = i * 1.05) {
    beep(speakerPin, i, 10);
  }
  delay(100);
  for (int i = 2000; i > 1000; i = i * .95) {
    beep(speakerPin, i, 10);
  }
  for (int i = 1000; i < 3000; i = i * 1.05) {
    beep(speakerPin, i, 10);
  }
  delay(100);
  for (int i = 2000; i > 1000; i = i * .95) {
    beep(speakerPin, i, 10);
  }
  for (int i = 1000; i < 3000; i = i * 1.05) {
    beep(speakerPin, i, 10);
  }
  delay(100);
  for (int i = 2000; i > 1000; i = i * .95) {
    beep(speakerPin, i, 10);
  }
}

void catcall() {
  for (int i = 1000; i < 5000; i = i * 1.05) {
    beep(speakerPin, i, 10);
  }
  delay(300);

  for (int i = 1000; i < 3000; i = i * 1.03) {
    beep(speakerPin, i, 10);
  }
  for (int i = 3000; i > 1000; i = i * .97) {
    beep(speakerPin, i, 10);
  }
}

void ohhh() {
  for (int i = 1000; i < 2000; i = i * 1.02) {
    beep(speakerPin, i, 10);
  }
  for (int i = 2000; i > 1000; i = i * .98) {
    beep(speakerPin, i, 10);
  }
}

void uhoh() {
  for (int i = 1000; i < 1244; i = i * 1.01) {
    beep(speakerPin, i, 30);
  }
  delay(200);
  for (int i = 1244; i > 1108; i = i * .99) {
    beep(speakerPin, i, 30);
  }
}

void laugh() {
  for (int i = 1000; i < 2000; i = i * 1.10) {
    beep(speakerPin, i, 10);
  }
  delay(50);
  for (int i = 1000; i > 500; i = i * .90) {
    beep(speakerPin, i, 10);
  }
  delay(50);
  for (int i = 1000; i < 2000; i = i * 1.10) {
    beep(speakerPin, i, 10);
  }
  delay(50);
  for (int i = 1000; i > 500; i = i * .90) {
    beep(speakerPin, i, 10);
  }
  delay(50);
  for (int i = 1000; i < 2000; i = i * 1.10) {
    beep(speakerPin, i, 10);
  }
  delay(50);
  for (int i = 1000; i > 500; i = i * .90) {
    beep(speakerPin, i, 10);
  }
  delay(50);
  for (int i = 1000; i < 2000; i = i * 1.10) {
    beep(speakerPin, i, 10);
  }
  delay(50);
  for (int i = 1000; i > 500; i = i * .90) {
    beep(speakerPin, i, 10);
  }
  delay(50);
}

void play(int melody[], int noteDurations[], int anzahlToene) {  //#include "pitches.h"
  for (int thisNote = 0; thisNote < anzahlToene; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(speakerPin, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    noTone(speakerPin); // stop the tone playing:
      SchalterStatus = digitalRead(SchalterPin);
      if (SchalterStatus == HIGH) { break; }
  }
}
