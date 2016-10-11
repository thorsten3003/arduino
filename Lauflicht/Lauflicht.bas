int ledPin[] = {2,3,4,5,6,7}; //ledPin0=2, ledPin1=3, ... 
int ledPin13 = 13;
boolean led13 = false;

// the setup routine runs once when you press reset:
void setup() {     
  pinMode(ledPin13, OUTPUT); 
  
  for(int i=0;i<=5;i++){
    pinMode(ledPin[i], OUTPUT);  
  } 
 
}

// the loop routine runs over and over again forever:
void loop() {
  lauflos();
}

void lauflos() {
  
  if(led13==true){
    digitalWrite(ledPin13, LOW);
    led13=false;
  }
  else{
    digitalWrite(led13, HIGH);
    led13=true;
  }
  
  for(int i=0;i<=5;i++){
    digitalWrite(ledPin[i], HIGH);
    delay(200);   
    digitalWrite(ledPin[i], LOW);
  }
  for(int i=4;i>=1;i--){
    digitalWrite(ledPin[i], HIGH);
    delay(200);   
    digitalWrite(ledPin[i], LOW);
  }
}