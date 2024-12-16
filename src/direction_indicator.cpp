#include "direction_indicator.h"

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

uint8_t uparrow0[8]  = {0x4,0xa,0x11,0x0,0x4,0xa,0x11};
uint8_t uparrow1[8]  = {0xa,0x11,0x0,0x4,0xa,0x11,0x0};
uint8_t uparrow2[8]  = {0x11,0x0,0x4,0xa,0x11,0x0,0x4};
uint8_t uparrow3[8]  = {0x0,0x4,0xa,0x11,0x0,0x4,0xa};
uint8_t dnarrow0[8]  = {0x0,0x11,0xa,0x4,0x0,0x11,0xa};
uint8_t dnarrow1[8]  = {0x4,0x0,0x11,0xa,0x4,0x0,0x11};
uint8_t dnarrow2[8]  = {0xa,0x4,0x0,0x11,0xa,0x4,0x0};
uint8_t dnarrow3[8]  = {0x11,0xa,0x4,0x0,0x11,0xa,0x4};

bool DirectionIndicator::draw(void* pObj)
{
    DirectionIndicator* pDir = (DirectionIndicator*)pObj;
    pDir->draw_indicator();
    return true;
}

void DirectionIndicator::begin(LiquidCrystal_I2C* lcd, unsigned int rows, unsigned int column_number, unsigned int interval)
{
  this->lcd = lcd; 
  this->lcd_column = column_number;
  this->lcd_rows = rows;

  lcd->createChar(0, uparrow0);
  lcd->createChar(1, uparrow1);
  lcd->createChar(2, uparrow2);
  lcd->createChar(3, uparrow3);
  lcd->createChar(4, dnarrow0);
  lcd->createChar(5, dnarrow1);
  lcd->createChar(6, dnarrow2);
  lcd->createChar(7, dnarrow3);

  timer = timer_create_default();
  timer.every(interval, DirectionIndicator::draw, this);
}

void DirectionIndicator::set_direction(Direction dir)
{
    this->dir = dir;
}

void DirectionIndicator::tick()
{
    timer.tick();
}

void DirectionIndicator::draw_uparrows(unsigned int char_number)
{
    for (unsigned int row = 0; row < lcd_rows; ++row)
    {
        lcd->setCursor(lcd_column, row);
        lcd->printByte(char_number);
    }
}

void DirectionIndicator::draw_downarrows(unsigned int char_number)
{
    for (unsigned int row = 0; row < lcd_rows; ++row)
    {
        lcd->setCursor(lcd_column, row);
        lcd->printByte(char_number+4);
    }
}

void DirectionIndicator::draw_noarrows()
{
    for (unsigned int row = 0; row < lcd_rows; ++row)
    {
        lcd->setCursor(lcd_column, row);
        lcd->print(" ");
    }
}

void DirectionIndicator::draw_indicator()
{
  switch(dir)
  {
    case Up:
      draw_uparrows(char_number);
      break;
    case Down:
      draw_downarrows(char_number);
      break;
    default:
      draw_noarrows();
  }

  char_number = (char_number + 1) % 4;
}
