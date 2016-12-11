// UltraschallTest1.ino

#define trigger 9  // Arduino Pin an HC-SR04 Trig
#define echo 10     // Arduino Pin an HC-SR04 Echo


void setup() {
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(9600); 
}

long duration=0;
long distance=0;
long distance_old=0;

void loop() {
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

  // Anzeige nur Aendern wenn neue Entfernung
  if(distance!=distance_old){ 
    lcd.clear();
    Serial.print(distance);
    Serial.println(" cm");
    }

  distance_old=distance; // letze Entfernung speichern
  delay(200);
}

