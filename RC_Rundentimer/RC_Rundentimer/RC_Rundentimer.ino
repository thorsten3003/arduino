#include   <LiquidCrystal.h>
#include <StackList.h> 
StackList <long> stack; 
#define TRUE 1;
#define FALSE 0;

int Rundenzaehler = 0;
float Rundenzeiten[] = {0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0, 0.0 };

//LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int LCDCounter;

//Ultraschall
int pingPin = 6;
int inPin = 7;
long duration, cm;


//LEDs
int LED[] = {1,0,8,9,13}; //gruen gruen gelb rot rot

//Taster
int taster = 10;
int tasterStatus = HIGH;
int vorhertasterStatus = HIGH;
int tastergedrueckt = FALSE;
int tasterAuswahl = 0;

unsigned long jetzt = 0;
unsigned long prev = 0;
long Sekunden = 0;
long Messabstand = 80;
int Mode=1; 

//LDR


void setup() 
  {
  stack.setPrinter (Serial);
  //LCD  
  lcd.begin(20, 4);
  lcd.print("Starte ");
  
  //LEDs
  for(int i = 0; i < 5; i++)
    pinMode(LED[i], OUTPUT);
    
  //Taster
  pinMode(taster, INPUT);
  digitalWrite(taster, HIGH); //interner PullUp-Widerstand

  //LDR
  // ...
  
  // Initialisierung der LEDs und LCD Anzeige
  delay(50);  
  lcd.setCursor(1, 2);
  lcd.print(".");
  delay(100);  
  lcd.setCursor(2, 2);
  lcd.print("."); 
  delay(100);  
  lcd.setCursor(3, 2);
  lcd.print(".");
  delay(100);  
  lcd.setCursor(4, 2);
  lcd.print("."); 
  delay(100);  
  lcd.setCursor(5, 2);
  lcd.print(".");
  delay(100);  
  lcd.setCursor(6, 2);
  lcd.print("."); 
  delay(100);  
  lcd.setCursor(7, 2);
  lcd.print(".");
  delay(100);  
  lcd.setCursor(8, 2);
  lcd.print(".");   
  lcd.setCursor(9, 2);
  lcd.print("."); 
  delay(100);  
  lcd.setCursor(10, 2);
  lcd.print(".");
  delay(100);  
  lcd.setCursor(11, 2);
  lcd.print(".");   
//    delay(1000);  
  digitalWrite(LED[0], LOW);
  digitalWrite(LED[1], LOW);
  digitalWrite(LED[2], LOW);  
  digitalWrite(LED[3], LOW);
  digitalWrite(LED[4], LOW);
    delay(1000); 
    lcd.setCursor(12, 2);
    lcd.print("."); 
  digitalWrite(LED[0], HIGH);
  digitalWrite(LED[4], HIGH);
    delay(300);
    lcd.setCursor(13, 2);
    lcd.print(".");     
  digitalWrite(LED[0], LOW);
  digitalWrite(LED[4], LOW);
  digitalWrite(LED[1], HIGH);  
  digitalWrite(LED[3], HIGH);
    delay(300);
    lcd.setCursor(14, 2);
    lcd.print(".");     
  digitalWrite(LED[1], LOW);
  digitalWrite(LED[3], LOW);
  digitalWrite(LED[2], HIGH);
    delay(300);
    lcd.setCursor(15, 2);
    lcd.print(".");     
  digitalWrite(LED[1], HIGH);
  digitalWrite(LED[3], HIGH);
  digitalWrite(LED[2], LOW);
    delay(400);
    lcd.setCursor(16, 2);
    lcd.print(".");     
  digitalWrite(LED[0], HIGH);
  digitalWrite(LED[4], HIGH);
  digitalWrite(LED[1], LOW);  
  digitalWrite(LED[3], LOW);  
    delay(500);
    lcd.setCursor(17, 2);
    lcd.print("."); 
  digitalWrite(LED[0], LOW);
  digitalWrite(LED[4], LOW);
  digitalWrite(LED[1], HIGH);  
  digitalWrite(LED[3], HIGH);  
    delay(600);
    lcd.setCursor(18, 2);
    lcd.print(".");     
  digitalWrite(LED[1], LOW);     
  digitalWrite(LED[3], LOW);  
  digitalWrite(LED[0], LOW);
  digitalWrite(LED[4], LOW);
  digitalWrite(LED[2], HIGH);
    delay(1000);  
    lcd.setCursor(19, 2);
    lcd.print(".");     
  digitalWrite(LED[2], LOW);
}

void loop()
{
  //Taster abfragen und gelbe LED
  tasterStatus=digitalRead(taster);
  if(tasterStatus == LOW)  //Taster gedrückt
  {
    digitalWrite(LED[2],HIGH);
    if(tasterStatus != vorhertasterStatus) tastergedrueckt = TRUE;
  }
  else  // Taster nicht gedrückt
   {
    digitalWrite(LED[2],LOW);
    tastergedrueckt = FALSE;
  }
  
   switch (Mode) {
    case 1:
      Ultraschall();
      lcd.clear();
      lcd.setCursor(15, 4);
      lcd.print(cm);
      lcd.print("cm");
      if(tastergedrueckt) Mode = 2;
      break;
      
    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Modus waehlen"); 
      lcd.setCursor(0, 1);
      lcd.print("Run-Lap-Lap-Lap"); 
      lcd.setCursor(0, 2);
      lcd.print("Run-Stop  Run-Stop"); 
      lcd.setCursor(0, 3);
      lcd.print("zurueck"); 
      
      if(tastergedrueckt) {
        tasterAuswahl++;
        jetzt = millis();  //jetzigen Zeitpunkt merken 
      }
      
      if(tasterAuswahl <=3) {
        lcd.setCursor(19, tasterAuswahl);
        lcd.print("*"); 
        
        if(tasterAuswahl > 0) { // wenn Taster min. 1x gedrückt wurde
            if( (millis() - jetzt) > 2000) {   // nach 3 Sekunden Modus wechsel
              if(tasterAuswahl == 1) Mode = 3;
              if(tasterAuswahl == 2) Mode = 3;
              if(tasterAuswahl == 3) Mode = 1;
              //tasterAuswahl = 0;
            }         
         }
      }
      else tasterAuswahl =1; // Cursor wieder nach oben setzen
      
      break;
    
    case 3:  //Run-Lap-Lap-Lap
      Rundenzaehler = 0;
      int i;
      for( i = 0; i >= 7; i++) {
        Rundenzeiten[i] = 0.0;
      }
          
      Ultraschall();
      lcd.clear();
      lcd.setCursor(0, 0);
      if(tasterAuswahl == 1) lcd.print("Run-Lap-Lap-Lap"); 
      if(tasterAuswahl == 2) lcd.print("Run-Stop  Run-Stop"); 
      lcd.setCursor(0, 1);
      lcd.print("Geraet platziern !"); 
      lcd.setCursor(0, 2);
      lcd.print("Knopf druecken zum");
      lcd.setCursor(0, 3);
      lcd.print("START/STOPP");
      lcd.setCursor(15, 4);
      lcd.print(cm);
      lcd.print("cm");
      if(tastergedrueckt){
        if(cm > 20 and cm <= 100) Messabstand = cm - 10;
        if(cm > 100) Messabstand = 100;
        if(cm <= 20) Messabstand = 10;
        if(cm = 0) Messabstand = 100;
        Mode = 4;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Messentfernung in cm");
        lcd.setCursor(0, 1);
        lcd.print(Messabstand);
        delay(1500);
       }
      break;
      
    case 4:  //Run-Lap-Lap-Lap 
      if(Rundenzaehler <= 8) {
        Ultraschall();
       if(tasterAuswahl == 1) ZeitmessungRLLL();
       if(tasterAuswahl == 2) Zeitmessung();
        LCD();
      }
      if(tastergedrueckt) Mode = 3;
      break;
    
    case 5:    //Run-Stop  Run-Stop
      Ultraschall();
      lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Modus 5");
      lcd.setCursor(15, 4);
      lcd.print(cm);
      lcd.print("cm");
      if(tastergedrueckt) Mode = 3;
      break;   

  }
   
  tastergedrueckt = FALSE;
  vorhertasterStatus = tasterStatus;
  delay(50);
}

void ZeitmessungRLLL(){
    if( millis() - prev >=1500) digitalWrite(LED[2], LOW);  //ZieldurchfahrtLED nach 1s wieder ausschalten
  
    if(cm <= Messabstand) {
      jetzt = millis(); 
      if( (jetzt - prev) >= 1500 ) { 
            digitalWrite(LED[2], HIGH);  //ZieldurchfahrtLED einschalten
      if(Rundenzaehler == 0) { //erste Durchfahrt nur Zeit merken (Startdurchfahrt)
            prev = jetzt;
        }else { //Messung ab 2. Durchfahrt 
            Sekunden = (jetzt - prev) / 10;
            prev = jetzt; //Zeit als previous Wert merken
            Rundenzeiten[Rundenzaehler] = (float) Sekunden * 0.01;
        }
        Rundenzaehler++; 
      }
    }
  }
  
void Zeitmessung() {
  if( (Rundenzaehler % 2) > 0 ) { 
    
    }
  if(cm <= Messabstand) {
    
    }
  }
  
void LCD() {
  //Statistiken und LEDs
  float avg = 0;
  float gesamt = 0;
  float top = 999.99;
  float flop= 0.0;
  int i;
  for( i = 1; i <= Rundenzaehler-1 ; i++) {
    gesamt = gesamt + Rundenzeiten[i];
    if(Rundenzeiten[i] < top) top = Rundenzeiten[i];
    if(Rundenzeiten[i] > flop) flop = Rundenzeiten[i];
   }
  avg = gesamt / (Rundenzaehler-1);
  
  if(Rundenzaehler >=2) {
    if(Rundenzeiten[Rundenzaehler-1] <= avg ) {
      digitalWrite(LED[1], HIGH);
      digitalWrite(LED[3], LOW);
      } else {
        digitalWrite(LED[1], LOW);
        digitalWrite(LED[3], HIGH);
      }
    if(Rundenzeiten[Rundenzaehler-1] <= top ) {
      digitalWrite(LED[0], HIGH);
      } else {
        digitalWrite(LED[0], LOW);
      }  
    if(Rundenzeiten[Rundenzaehler-1] >= flop ) {
      digitalWrite(LED[4], HIGH);
      } else {
        digitalWrite(LED[4], LOW);
      }    
      
   } 
    
  //Statistiken Ende
  
    LCDCounter++;
    if(LCDCounter >= 8 ) {  //Nur jedes X-te mal das Display aktualisieren
      lcd.clear();  
      lcd.setCursor(15, 3);
      if(Rundenzaehler >=1 ) lcd.print( (millis() -prev) /1000);
      
      lcd.setCursor(0, 0);
      lcd.print( Rundenzeiten[1] );
      lcd.setCursor(0, 1);
      lcd.print( Rundenzeiten[2] );
      lcd.setCursor(0, 2);
      lcd.print( Rundenzeiten[3] );
      lcd.setCursor(0, 3);
      lcd.print( Rundenzeiten[4] );
      lcd.setCursor(6, 0);
      lcd.print( Rundenzeiten[5] );
      lcd.setCursor(6, 1);
      lcd.print( Rundenzeiten[6] );
      lcd.setCursor(6, 2);
      lcd.print( Rundenzeiten[7] );
      lcd.setCursor(6, 3);
      lcd.print( Rundenzeiten[8] );
      lcd.setCursor(11, 0);
      lcd.print( "Top:" );
      lcd.setCursor(15, 0);
      lcd.print( top);  
      lcd.setCursor(11, 1);
      lcd.print( "Avg:");
      lcd.setCursor(15, 1);
      lcd.print( avg);     
      lcd.setCursor(11, 2);
      lcd.print( "Rnd:");
      lcd.setCursor(16, 2);
      if(Rundenzaehler >=1 and Rundenzaehler <=8 ) lcd.print(Rundenzaehler);
   }   
      
}

void Test() {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("jetzt");
      lcd.setCursor(10, 0);
      lcd.print(jetzt);
      lcd.setCursor(0, 1);
      lcd.print("prev");
      lcd.setCursor(10, 1);
      lcd.print(prev);
       lcd.setCursor(0, 2);
      lcd.print("Sekunden");
      lcd.setCursor(10, 2);
      lcd.print(Sekunden);
     // if (!stack.isEmpty()) lcd.print( stack.peek ()); 

      lcd.setCursor(15, 4);
      lcd.print(cm);
      lcd.print("cm");
  }
  
void Ultraschall() {
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds. // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  // The inpin is used to read the signal from the PING))): a HIGH // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(inPin, INPUT);
  duration = pulseIn(inPin, HIGH);
  cm= duration / 29 / 2;
  }

