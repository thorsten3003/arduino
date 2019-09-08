#include <DFRobot_sim808.h>
#include <SoftwareSerial.h>
#define PIN_TX 10
#define PIN_RX 11
#define PIN_SIM808Button 9

SoftwareSerial mySerial(PIN_TX,PIN_RX);
DFRobot_SIM808 sim808(&mySerial);//Connect RX,TX,PWR,
char buffer[512];

// Funktionen 
// 0 = nicht gefunden,   1 = gefunden
 int stringcontains(String str1, String str2)  {
    int found = 0;
    String substr = "";
    int strdiff = str1.length()-str2.length();
    if (strdiff < 0) {
      return 0;
    }
    for (int i=0; i <= strdiff; i++){
      substr = str1.substring(i, i+str2.length());
      if (substr.equals(str2)){
        ++found;
      }      
    }
    return found;
  }

String sendATcommand(const char *toSend, unsigned long WaitInMilliseconds) {
  String result;
  Serial.print("Sending: ");
  Serial.println(toSend);
  mySerial.println(toSend);
  unsigned long startTime = millis();
  Serial.print("Received: ");
  while (millis() - startTime < WaitInMilliseconds) {
    if (mySerial.available()) {
      char c = mySerial.read();
      Serial.write(c);
      result += c;  // append to the result string
      }
    }
    Serial.println();  // new line after timeout.
    return result;
  }




void setup()
{
  pinMode(PIN_SIM808Button, OUTPUT);
  mySerial.begin(9600);   
  Serial.begin(9600);   
  delay(1000);

   //1. Testen ob Sim808 ansprechberei ist
  Serial.print("Testen ob Sim808 ansprechbereit ist "); 
  while( stringcontains( sendATcommand( "AT", 300 ) , "OK" ) == 0 ) { 
    Serial.print("ERROR - SIM808 Antwortet nicht!"); 
    digitalWrite(PIN_SIM808Button, HIGH); 
    delay(3500);  //3.5 Sekunden PIN_SIM808Button gedrückt halten
    digitalWrite(PIN_SIM808Button, LOW);  
    delay(1500);
    }
    
  //******** Initialize sim808 module *************
  while(!sim808.init()) {
      delay(1000);
      Serial.print("Sim808 init error\r\n");
  }
  delay(3000);  
    
  //*********** Attempt DHCP *******************
  while(!sim808.join(F("internet.t-mobile"))) {
      Serial.println("Sim808 join network error");
      delay(2000);
      //TODO Bei Fehler RESET des Boards?
  }

  //************ Successful DHCP ****************
  Serial.print("IP Address is ");
  Serial.println(sim808.getIPAddress());


/*Verbindung aufbauen Daten übertragen
  if( stringcontains( sendATcommand( "AT+CIPSTART=\"TCP\",\"v92140.1blu.de\",80 " ,300) , "OK" ) > 0 ) { 
    //*********** Send a GET request *****************
    char http_cmd[100];
    sprintf(http_cmd, 'GET /wohnwagen/wwInsert.php?key=4e5rftuzg4e5rftuzg&U1=12.1&U2=12.1&LAT="242.48662"&LON="2-83.29399"&Text="Text"');
    sim808.send(http_cmd, strlen(http_cmd));
  }
  else {
     Serial.println("Connect zu v92140.1blu.de fehlgeschlagen");
  }
*/

if( stringcontains( sendATcommand( "AT+HTTPINIT=?", 300 ) , "OK" ) == 0 ) { 
  Serial.print("HTTPINIT OK")
  sendATcommand( "AT+HTTPPARA=\"CID\",1", 300 )
  sendATcommand( "AT+HTTPPARA=\"URL\",\"v92140.1blu.de\" ", 300 )
  sendATcommand( "AT+HTTPACTION=0 ", 300 )
}
AT+HTTPTERM

//************* Close TCP or UDP connections **********
sim808.close();

//*** Disconnect wireless connection, Close Moving Scene *******
sim808.disconnect();



  
  Serial.print("Ende setup");
  }

void loop()
{
 if (Serial.available()>0)
 mySerial.write(Serial.read());
 if (mySerial.available()>0)
 Serial.write(mySerial.read());
}
