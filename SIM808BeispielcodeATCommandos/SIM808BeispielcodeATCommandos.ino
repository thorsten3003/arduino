#include <GPRS_Shield_Arduino.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>


#define PIN_WAKE  4
#define PIN_PWR   7
#define PIN_TX    10
#define PIN_RX    11
#define BAUDRATE  9600
SoftwareSerial    mySerial(PIN_TX, PIN_RX);


char latitude[6];
char longitude[6];


unsigned long lastMillis;




 


#define MESSAGE_LENGTH 60
char message[MESSAGE_LENGTH];
int messageIndex = 0;

char phone[14];
char datetime[12];


GPRS gprsTest(PIN_TX, PIN_RX, BAUDRATE);
Adafruit_GPS GPS(&mySerial);

void setup() {
  Serial.begin(9600);
  pinMode(PIN_WAKE, OUTPUT);
  pinMode(PIN_PWR, OUTPUT);


  while (!gprsTest.init()) {
    Serial.print("init error\r\n");
    delay(1000);
  }
  delay(3000);
  Serial.println("Init Success.");

  sendATcommand("AT" , 2000);
  sendATcommand("AT+ECHARGE=1" , 2000);
  sendATcommand("AT+CGPSPWR=1" , 2000);
  
  
Serial.print("SetUp Done Board Now Ready For Use");
  
}



void loop() {
      //is the time for a SYS Check? 
    if (millis() - lastMillis >= 1 * 60 * 1000UL)
    {
      lastMillis = millis(); //get ready for the next iteration
      sendATcommand("AT+CBC" , 2000);
      Serial.println();

     
    }

}




void ReadSMS() {
  messageIndex = gprsTest.isSMSunread();
  if (messageIndex > 0) { //At least, there is one UNREAD SMS
    gprsTest.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);
    //In order not to full SIM Memory, is better to delete it
    gprsTest.deleteSMS(messageIndex);
    Serial.print("From number: ");
    Serial.println(phone);
    Serial.print("Datetime: ");
    Serial.println(datetime);
    Serial.print("Recieved Message: ");
    Serial.println(message);
  }
}

void connectWeb() {
  gprsTest.init();
  // attempt DHCP
  while (false == gprsTest.join(F("cmnet"))) {
    Serial.println("gprs join network error");
    delay(2000);
  }

  // successful DHCP
  Serial.print("IP Address is ");
  Serial.println(gprsTest.getIPAddress());
}





void wake() {
  digitalWrite(PIN_WAKE, LOW);  // Reset the Display via D4
  delay(100);
  digitalWrite(PIN_WAKE, HIGH);  // unReset the Display via D4
  delay (5000); //let the display start up after the reset (This is important)


}

void sleep() {
  sendATcommand("AT+CSCLK=1" , 20000);

}





String sendATcommand(const char *toSend, unsigned long milliseconds) {
  String result;
  Serial.print("Sending: ");
  Serial.println(toSend);
  mySerial.println(toSend);
  unsigned long startTime = millis();
  Serial.print("Received: ");
  while (millis() - startTime < milliseconds) {
    if (mySerial.available()) {
      char c = mySerial.read();
      Serial.write(c);
      result += c;  // append to the result string
    }
  }
Serial.println();  // new line after timeout.
return result;
}
