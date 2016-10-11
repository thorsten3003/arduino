

int ledpin = 13 ;
int tasterpin = 8;
int tasterwert = LOW;
int zustand = 0;
int zustandled = LOW;
void setup() {

  pinMode(ledpin, OUTPUT);
  pinMode(tasterpin, INPUT);
  digitalWrite(tasterpin,HIGH);
}

void loop() {

  tasterwert=digitalRead(tasterpin);

  if(tasterwert==HIGH) {   //Schalter losgelassen
    zustand=1;
  }
  else 
    if(tasterwert==LOW & zustand==1) {  //Schalter gedrueckt und war vorher losgelassen
    if(zustandled==LOW) {  // dann wenn LED aus
      digitalWrite(ledpin, HIGH); //schreibe an
      zustandled=HIGH;
    } 
    else {                      // ansonsten
      digitalWrite(ledpin, LOW);   // aus
      zustandled=LOW;
    }
    zustand=0;                  // Tasterzustand auf 0 da gedrueckt
  }
}

