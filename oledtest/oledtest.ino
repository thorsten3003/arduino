//BOARD: Wemos Lolin32


#include <Wire.h>
#include "SSD1306.h" 
 
SSD1306 display (0x3c, 5, 4);

void setup() {
 
  display.init();
  display.drawString(0, 0, "Hello World");
  display.display();
}
 
void loop() {}
