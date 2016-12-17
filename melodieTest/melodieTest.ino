#include "pitches.h"

int speakerPin = 9; // speaker connected to digital pin 9
// notes in the melody, note durations: 4 = quarter note, 8 = eighth note, etc.:


void setup() {
  int toene[] = { NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4 };
  int dauer[] = { 4, 8, 8, 4, 4, 4, 4, 4 };
  int anzahlToene=8;
//  play(toene, dauer, anzahlToene);

delay(2000);

  int r2toene[] = { NOTE_A3, NOTE_G3, NOTE_E3, NOTE_C3, NOTE_D3, NOTE_B3, NOTE_F3, NOTE_C3, NOTE_A3, NOTE_G3, NOTE_E3, NOTE_C3, NOTE_D3, NOTE_B3, NOTE_F3, NOTE_C3 };
  int r2dauer[] = { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };
  int r2anzahlToene=16;
  play(r2toene, r2dauer, r2anzahlToene);

}


  void play(int melody[], int noteDurations[], int anzahlToene) {
    for (int thisNote = 0; thisNote < anzahlToene; thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(speakerPin, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);

      noTone(speakerPin); // stop the tone playing:
    }
  }
  
void loop() {
  // no need to repeat the melody.
}
