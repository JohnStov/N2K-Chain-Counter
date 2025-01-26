#pragma once

#ifndef __MENU_H__
#define __MENU_H__

#include <LiquidCrystal_I2C.h>
#undef max
#undef min
#include <algorithm>
#include <vector>
#include <string>
#include "menu_item.h"

class ITimeReceiver {
public:
    virtual void set_time(int hrs, int mins, int secs) = 0;
};

class TimeMenuItem : public MenuItem, public ITimeReceiver {
public:
    virtual bool can_focus();
    virtual void draw(LiquidCrystal_I2C* lcd);
    virtual bool is_updated();
    virtual void set_time(int hrs, int mins, int secs);

private:
    int hrs = 0;
    int mins = 0;
    int secs = 0;
    bool updated = false;
};

class NumericMenuItem : public TitledMenuItem {
public:
    NumericMenuItem(std::string title, std::string units, int initial_value);
    virtual void draw(LiquidCrystal_I2C* lcd);
    virtual bool can_focus();

private:
    std::string units;
    int value;
};

class TextMenuItem : public TitledMenuItem {
public:
    TextMenuItem(std::string title);
    virtual void draw(LiquidCrystal_I2C* lcd);
    virtual bool can_focus();
};

class Menu {
public:
    Menu(std::vector<MenuItem*> items, LiquidCrystal_I2C* lcd);
    void display();
    MenuItem* get_active_item();

private:
    LiquidCrystal_I2C* lcd;
    std::vector<MenuItem*> items;
    MenuItem* active_item;
};

#endif