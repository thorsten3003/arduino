
 
 int luefter1 = 11;

void setup(void) 
{
  Serial.begin(9600);
  

  pinMode(luefter1, OUTPUT); 


  

}
 
 
void loop(void) 
{
    Serial.print("loop");
    
  analogWrite(luefter1, 255); 
  Serial.print("255 ");
  delay(6000);
  


for (int i=1;i<=255;i++) {
  analogWrite(luefter1, i); 
  Serial.print(i);
  Serial.print(" ");
  delay(200);
}

}
