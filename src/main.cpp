//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <LiquidCrystal_I2C.h>
#include <direction_indicator.h>
#include <arduino-timer.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

LiquidCrystal_I2C lcd(0x20, 20, 4, &Wire1);  // set the LCD address to 0x27 for a 16 chars and 2 line display
DirectionIndicator indicator;
auto timer = timer_create_default();

bool switch_direction(void *)
{
  switch (indicator.get_direction())
  {
  case Up:
    indicator.set_direction(Down);
    break;
  
  case Down:
  default:
    indicator.set_direction(Up);
    break;
  }

  return true;
}


void setup()
{
  lcd.init();                      // initialize the lcd 
  indicator.begin(&lcd);

  lcd.backlight();
  lcd.home();
  lcd.print("Hello world...");

  switch_direction(NULL);
  timer.every(20000, switch_direction);

}

void loop()
{
  indicator.tick();
  timer.tick();
}