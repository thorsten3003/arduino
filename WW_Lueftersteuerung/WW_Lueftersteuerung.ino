#include <OneWire.h>
#include <DallasTemperature.h>
 
OneWire  ds(4); //pin für ds1820

//DeviceAdressen der einzelnen ds1820 Temperatursensoren angeben. (loop anpassen)
DeviceAddress sensor1 = { 0x10, 0xED, 0xDB, 0x5F, 0x2, 0x8, 0x0, 0xCD };
DeviceAddress sensor2 = { 0x10, 0xA6, 0xEC, 0x5F, 0x2, 0x8, 0x0, 0x95 };

char sensor1Name[] = "Oben: ";
char sensor2Name[] = "Unten: ";

int led1V = 5;
int led2V = 6;
int led3V = 7;
int led4V = 8;
int led5V = 9;
 
 float temp_oben = 0.0;
 float temp_unten = 0.0;
 float temp_oben_alt = 0.0;
 float temp_unten_alt = 0.0;
 float tmpDiff=0.0;
 float tmpDiff_alt=0.0;
 
 // Transistor der den Lüfter steuert
 int luefter = 11;
 int luefter2 = 12; 
 int stufeluefter = 0;
 int stufeluefter2 = 0;
 
void setup(void) 
{
  Serial.begin(9600);
  
  pinMode(led1V, OUTPUT);
  pinMode(led2V, OUTPUT);
  pinMode(led3V, OUTPUT);
  pinMode(led4V, OUTPUT);
  pinMode(led5V, OUTPUT);
  
  pinMode(luefter, OUTPUT); 
  pinMode(luefter2, OUTPUT); 

  analogWrite(luefter, 200);
  
  // Lauflicht zur Initialisierung der LEDs
  digitalWrite(led1V, HIGH);
  delay(100);
  digitalWrite(led2V, HIGH);
    delay(100);
  digitalWrite(led3V, HIGH);
    delay(100);
  digitalWrite(led4V, HIGH);
    delay(100);
  digitalWrite(led5V, HIGH);  
    delay(200);
    delay(50000);
  digitalWrite(led5V, LOW);
    delay(100);
  digitalWrite(led4V, LOW);
    delay(100);
  digitalWrite(led3V, LOW);
    delay(100);
  digitalWrite(led2V, LOW);
  
  analogWrite(luefter, 15);

}
 
void writeTimeToScratchpad(byte* address)
{
  //reset the bus
  ds.reset();
  //select our sensor
  ds.select(address);
  //CONVERT T function call (44h) which puts the temperature into the scratchpad
  ds.write(0x44,1);
  //sleep a second for the write to take place
  delay(1000);
}
 
void readTimeFromScratchpad(byte* address, byte* data)
{
  //reset the bus
  ds.reset();
  //select our sensor
  ds.select(address);
  //read the scratchpad (BEh)
  ds.write(0xBE);
  
  for (byte i=0;i<9;i++){
    data[i] = ds.read();
  }
}
 
float getTemperature(byte* address)
{
  int tr;
  byte data[12];
 
  writeTimeToScratchpad(address);
 
  readTimeFromScratchpad(address,data);
 
  //put in temp all the 8 bits of LSB (least significant byte)
  tr = data[0];
 
  //check for negative temperature
  if (data[1] > 0x80)
  {
    tr = !tr + 1; //two's complement adjustment
    tr = tr * -1; //flip value negative.
  }
 
  //COUNT PER Celsius degree (10h)
  int cpc = data[7];
  //COUNT REMAIN (0Ch)
  int cr = data[6];
 
  //drop bit 0
  tr = tr >> 1;
 
  return tr - (float)0.25 + (cpc - cr)/(float)cpc;
}
 
void loop(void) 
{
  temp_oben = getTemperature(sensor1);
  temp_unten = getTemperature(sensor2);

  if(temp_oben >= 0 ) 
  { 
    if(temp_unten >= 0 ) 
    {
        tmpDiff = temp_oben - temp_unten;
        if (tmpDiff_alt - tmpDiff >= -2 && tmpDiff_alt - tmpDiff <= 2.0 )
      {     Serial.print("innen ");   
        if(tmpDiff<0) { tmpDiff=tmpDiff_alt;}
        if(tmpDiff>16) { tmpDiff=tmpDiff_alt;}     
      }
    }
  }  
   
  tmpDiff_alt=tmpDiff;

  
 Serial.print(sensor1Name);
 Serial.print(" : ");
 Serial.print(temp_oben);
 Serial.println(" Celsius, ");
 Serial.print(sensor2Name);
 Serial.print(": ");
 Serial.print(temp_unten);
 Serial.print(" Celsius. = Differenz: ");
 Serial.println(tmpDiff);
                 
// LEDs und Lüftersteuerung                  
  if(tmpDiff < 1.0) {
      digitalWrite(led1V, LOW); 
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
      stufeluefter = 15;
    }     

    
  if(tmpDiff >= 1.0 and tmpDiff <2.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
      stufeluefter = 20;
    }  
    
  if(tmpDiff >= 2.0 and tmpDiff <3.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
      stufeluefter = 30;
    }  
    
  if(tmpDiff > 3.0 and tmpDiff <4.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
      stufeluefter = 40;
    }   
  if(tmpDiff >= 4.0 and tmpDiff <5.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
      stufeluefter = 50;
    }  
    
  if(tmpDiff >= 5.0 and tmpDiff <6.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
      stufeluefter = 50;
    }  
    
  if(tmpDiff > 6.0 and tmpDiff <7.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
      stufeluefter = 60;
    }       

  if(tmpDiff > 7.0 and tmpDiff <8.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
      stufeluefter = 70;
    }   

  if(tmpDiff >= 8.0 and tmpDiff <9.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
      stufeluefter = 80;
    }  
    
  if(tmpDiff >= 9.0 and tmpDiff <10.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
      stufeluefter = 90;
    }  
    
  if(tmpDiff > 10.0 and tmpDiff <11.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
      stufeluefter = 100;
    }    

  if(tmpDiff >= 11.0 and tmpDiff <12.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
      stufeluefter = 150;
    }  
    
  if(tmpDiff >= 12.0 and tmpDiff <13.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
      stufeluefter = 200;
    }  
    
  if(tmpDiff > 13.0 and tmpDiff <14.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
      stufeluefter = 230;
    }            
    
  if(tmpDiff >= 14.0 and tmpDiff <15.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
      stufeluefter = 240;
    }  
    
  if(tmpDiff >= 15.0 and tmpDiff <16.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
      stufeluefter = 255;
    }  
    
  if(tmpDiff > 16.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, HIGH); 
    }       
  
      
  
   
  analogWrite(luefter, stufeluefter);         // By changing values from 0 to 255 you can control motor speed
  analogWrite(luefter2, stufeluefter2);    
  Serial.print("Stufe Lüfter1: ");
  Serial.println(stufeluefter);
  //Serial.print(" -  Stufe Lüfter2: ");
  //Serial.println(stufeluefter2);
  
  delay(5000);
}
