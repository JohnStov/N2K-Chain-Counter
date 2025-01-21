#include <LiquidCrystal_I2C.h>
#include "menu.h"

LiquidCrystal_I2C lcd(0x20, 20, 4, &Wire1);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Menu menu({
  new NumericMenuItem("Gypsy circ", "cm", 32),
  new NumericMenuItem("Rode length", "m", 60),
  new TextMenuItem("Item 3"),
  new TextMenuItem("Item 4")
}, &lcd);

void setup() {
  lcd.init();                      // initialize the lcd 
  lcd.backlight();

  Serial.write("Starting...\r\n");
  lcd.setCursor(0,0);
  lcd.print("Hello, World");

  delay(1000);
  menu.display();
}

void loop() {
}
