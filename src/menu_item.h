#pragma once

#ifndef __MENU_ITEM_H__
#define __MENU_ITEM_H__

#include <LiquidCrystal_I2C.h>
#undef min
#undef max
#include <string>

class MenuItem {
public:
    virtual void draw(LiquidCrystal_I2C* lcd) = 0;
    virtual bool can_focus() = 0;

    virtual bool is_updated();
};

class TitledMenuItem : public MenuItem {
protected:
    TitledMenuItem(std::string title);

    const std::string get_title();

private:
    std::string title;
};

#endif