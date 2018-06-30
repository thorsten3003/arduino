#include "pitches.h"

int speakerPin = 11; // speaker connected to digital pin 11
int LDRPin = 0;
int LDRWert = 0;
boolean istAktiv = 0;
boolean debug = 1;

void setup() {
      Serial.begin(9600);
}

void loop() {
  LDRWert= analogRead(LDRPin); 
  if(debug) Serial.println(LDRWert);
    if(LDRWert<200)
    {
      if(debug) Serial.println("STROMAUSFALL");
      if(istAktiv == 1) 
      {
        for (int i=0; i <= 9; i++){
          tone(speakerPin, NOTE_G1, 500);
          delay(1000);
       }
       delay(10000);
      }
    }
    else
    {
      istAktiv = 1;
    }

}




