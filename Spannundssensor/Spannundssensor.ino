/*
DC Voltmeter Using a Voltage Divider
Based on Code Created By
T.K.Hareendran
max 25V
http://henrysbench.capnfatz.com/henrys-bench/arduino-voltage-measurements/arduino-25v-voltage-sensor-module-user-manual/
*/

#define SENSOR A1
const float R1 = 30000.0f; // Der Widerstand R1 hat eine größe von 30 kOhm
const float R2 = 7500.0f; //  Der Widerstand R2 hat eine größe von 7,5 kOhm
const float MAX_VIN = 3.39f;
void setup(){
   Serial.begin(9600);
   pinMode(SENSOR, INPUT);
}
void loop(){
  // Serial.println(analogRead(SENSOR));
  float vout = (analogRead(SENSOR) * MAX_VIN) / 1024.0f;
  float vin = vout / (R2/(R1+R2)); 
   
  Serial.print("Eingangsspannung ");
  Serial.print(vin,2);
  Serial.println("V");
  delay(5000);
}
