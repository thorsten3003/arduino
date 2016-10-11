
#include "Servo.h"
// Servo.ino

Servo meinServo;
int analogPin = 0;

void setup() {
	meinServo.attach(5);

	
	Kopfball(0);
	Kopfball(550);
	Kopfball(870);	
	Kopfball(975);	
	Kopfball(975);	
	Kopfball(955);	

	Kopfball(945);	
}

void loop() {
}

void Kopfball(int ms) {
	delay(ms);
	meinServo.write(90); //runter
	delay(200);
	meinServo.write(140);  //hoch
	}