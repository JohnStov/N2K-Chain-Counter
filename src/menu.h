#pragma once

#ifndef __MENU_H__
#define __MENU_H__

#include <LiquidCrystal_I2C.h>
#undef max
#undef min
#include <algorithm>
#include <vector>
#include <string>

class MenuItem {
public:
    virtual void draw(LiquidCrystal_I2C* lcd) { updated = false; };
    virtual bool can_focus() = 0;

    void set_updated() { updated = true; }
    bool is_updated() { return updated; }

private:
    bool updated = true;
};

class TimeMenuItem : public MenuItem {
public:
    virtual bool can_focus() { return false; }
    virtual void draw(LiquidCrystal_I2C* lcd) {
        char output[10];
        sprintf(output, "%02d:%02d:%02d", hrs, mins, secs);
        lcd->print(output);
        MenuItem::draw(lcd);
    }

    void set_time(int hrs, int mins, int secs) {
        this->hrs = hrs; 
        this->mins = mins; 
        this->secs = secs; 
        set_updated();
    }

private:
    int hrs = 0;;
    int mins = 0;;
    int secs = 0;
};

class TitledMenuItem : public MenuItem {
protected:
    TitledMenuItem(std::string title) {
        this->title = title;
    }

    std::string get_title() {
        return title;
    }

private:
    std::string title;
};

class NumericMenuItem : public TitledMenuItem {
public:
    NumericMenuItem(std::string title, std::string units, int initial_value) 
        : TitledMenuItem(title) {
        this->units = units;
        this->value = initial_value;
    }

    virtual void draw(LiquidCrystal_I2C* lcd) {
        char buf[20];
        std::snprintf(buf, 20, "%s: %d %s", get_title().c_str(), value, units.c_str());
        lcd->print(buf);
        MenuItem::draw(lcd);
    }

    virtual bool can_focus() { return true; } 

private:
    std::string units;
    int value;
};

class TextMenuItem : public TitledMenuItem {
public:
    TextMenuItem(std::string title)
        : TitledMenuItem(title) {
    }

    virtual void draw(LiquidCrystal_I2C* lcd) {
        lcd->print(get_title().c_str());
        MenuItem::draw(lcd);
    }

    virtual bool can_focus() { return false; } 
};

class Menu {
public:
    Menu(std::vector<MenuItem*> items, LiquidCrystal_I2C* lcd) {
        this->items = items;
        this->lcd = lcd;
        this->active_item = items[0];
    }

    void display();
    MenuItem* get_active_item() { return active_item; };

private:
    LiquidCrystal_I2C* lcd;
    std::vector<MenuItem*> items;
    MenuItem* active_item;
    bool changed = true;
};

#endif