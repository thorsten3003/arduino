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
 
 float temp1 = 0.0;
 float temp2 = 0.0;
 float tmpDiff=0.0;
 
 // Transistor der den Lüfter steuert
 int luefter = 11;
 int luefter2 = 12; 
 
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
  digitalWrite(led5V, LOW);
    delay(100);
  digitalWrite(led4V, LOW);
    delay(100);
  digitalWrite(led3V, LOW);
    delay(100);
  digitalWrite(led2V, LOW);

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
  
  temp1 = getTemperature(sensor1);
  temp2 = getTemperature(sensor2);
  tmpDiff = temp1 - temp2;
  boolean runterpusten =false; 
  
                  Serial.print(sensor1Name);
                  Serial.print(temp1);
                  Serial.println(" Celsius");
                  Serial.print(sensor2Name);
                  Serial.print(temp2);
                  Serial.println(" Celsius");
                  Serial.print("Differenz: ");
                  Serial.print(tmpDiff);
                  Serial.println();
                  Serial.println();
                 
// LEDs und Lüftersteuerung                  
  if(tmpDiff < 1.0 {
      digitalWrite(led1V, LOW); 
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
    }     
    
  if(tmpDiff >= 1.0 and tmpDiff <2.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
    }  
    
  if(tmpDiff >= 2.0 and tmpDiff <3.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
    }  
    
  if(tmpDiff > 3.0 and tmpDiff <4.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
    }   
  if(tmpDiff >= 4.0 and tmpDiff <5.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
    }  
    
  if(tmpDiff >= 5.0 and tmpDiff <6.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
    }  
    
  if(tmpDiff > 6.0 and tmpDiff <7.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
    }       

  if(tmpDiff > 7.0 and tmpDiff <8.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, LOW); 
    }   

  if(tmpDiff >= 8.0 and tmpDiff <9.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
    }  
    
  if(tmpDiff >= 9.0 and tmpDiff <10.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
    }  
    
  if(tmpDiff > 10.0 and tmpDiff <11.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
    }    

  if(tmpDiff >= 11.0 and tmpDiff <12.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
    }  
    
  if(tmpDiff >= 12.0 and tmpDiff <13.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
    }  
    
  if(tmpDiff > 13.0 and tmpDiff <14.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
    }            
    
  if(tmpDiff >= 14.0 and tmpDiff <15.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
    }  
    
  if(tmpDiff >= 15.0 and tmpDiff <16.0) {
      digitalWrite(led1V, HIGH);
      digitalWrite(led2V, HIGH);
      digitalWrite(led3V, HIGH);
      digitalWrite(led4V, HIGH);
      digitalWrite(led5V, LOW); 
    }  
    
  if(tmpDiff > 16.0) {
      digitalWrite(led1V, LOW);
      digitalWrite(led2V, LOW);
      digitalWrite(led3V, LOW);
      digitalWrite(led4V, LOW);
      digitalWrite(led5V, HIGH); 
    }       
  
      
    if (tmpDiff >=1.5) {
      stufeluefter =  map(tmpDiff, 1.5, 16, 60, 200);
      stufeluefter2 = map(tmpDiff, 3, 16, 60, 255);
    }
    else
   
      stufeluefter = 0;
      stufeluefter2 =0;
   }

      analogWrite(luefter, stufeluefter);         // By changing values from 0 to 255 you can control motor speed
      analogWrite(luefter2, stufeluefter2);
     
    

  delay(15000);
}
