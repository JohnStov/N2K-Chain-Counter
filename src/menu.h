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
    virtual std::string get_text() = 0;
protected:
    MenuItem(std::string title) {
        this->title = title;
    }

    std::string title;
};

class NumericMenuItem : public MenuItem {
public:
    NumericMenuItem(std::string title, std::string units, int initial_value) 
        : MenuItem(title) {
        this->units = units;
        this->value = initial_value;
    }

    virtual std::string get_text() {
        auto buf = new char[ 20 ];
        std::snprintf(buf, 20, "%s: %d %s", title.c_str(), value, units.c_str());
        auto result = std::string(buf);
        delete buf;
        return result;
    }

private:
    std::string units;
    int value;
};

class TextMenuItem : public MenuItem {
public:
    TextMenuItem(std::string title)
        : MenuItem(title) {
    }

    virtual std::string get_text() {
        return title;
    }
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
};

#endif