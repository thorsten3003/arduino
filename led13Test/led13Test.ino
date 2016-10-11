// led13Test.ino

int LedPin=13;

void setup() {
	pinMode(13, OUTPUT);
}

void loop() {
	digitalWrite(LedPin, HIGH);
	delay(500);
	digitalWrite(LedPin, LOW);
	delay(500);
}

