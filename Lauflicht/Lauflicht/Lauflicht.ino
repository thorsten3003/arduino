int ledPin[] = {2,3,4,5,6,7}; //ledPin0=2, ledPin1=3, ... 
int ledPin13 = 13;
boolean led13 = false;
int geschwindigkeit=100;

// the setup routine runs once when you press reset:
void setup() {     
  pinMode(ledPin13, OUTPUT); 
  
  for(int i=0;i<=5;i++){
    pinMode(ledPin[i], OUTPUT);  
  } 
 
}

// the loop routine runs over and over again forever:
void loop() {
  
  if(led13==false){
    digitalWrite(ledPin13, HIGH);
    led13=true;
  }else{
    digitalWrite(ledPin13, LOW); 
    led13=false;
  }
  
   
  
  lauflos();
  
}

void lauflos() {
  
 for(int i=0;i<=5;i++){
    digitalWrite(ledPin[i], HIGH);
    delay(geschwindigkeit);   
    digitalWrite(ledPin[i], LOW);
  }
  for(int i=4;i>=1;i--){
    digitalWrite(ledPin[i], HIGH);
    delay(geschwindigkeit);   
    digitalWrite(ledPin[i], LOW);
  }
}
