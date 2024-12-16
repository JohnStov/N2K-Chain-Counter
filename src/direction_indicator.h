#pragma once

#ifndef DIRECTION_INDICATOR_H
#define DIRECTION_INDICATOR_H

#include <LiquidCrystal_I2C.h>
#include <arduino-timer.h>

enum Direction { Up, Down, Stopped };

class DirectionIndicator {
public:
    void begin(LiquidCrystal_I2C* lcd, unsigned int rows = 4, unsigned int column_number = 19, unsigned int interval = 300);
    void set_direction(Direction dir);
    Direction get_direction() { return dir; }
    void tick();

private:
    void draw_uparrows(unsigned int char_number);
    void draw_downarrows(unsigned int char_number);
    void draw_noarrows();
    void draw_indicator();

    static bool draw(void*);

    LiquidCrystal_I2C* lcd = NULL;
    unsigned int lcd_rows;
    unsigned int lcd_column;
    unsigned int char_number = 0;
    Direction dir = Stopped;
    Timer<> timer;
};

#endif
