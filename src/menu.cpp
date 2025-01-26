#include "menu.h"

bool TimeMenuItem::can_focus() { return false; }

void TimeMenuItem::draw(LiquidCrystal_I2C* lcd) {
    char output[10];
    sprintf(output, "%02d:%02d:%02d", hrs, mins, secs);
    lcd->print(output);
}

bool TimeMenuItem::is_updated() {
    bool result = updated;
    updated = false;
    return result;
}

void TimeMenuItem::set_time(int hrs, int mins, int secs) {
    this->hrs = hrs; 
    this->mins = mins; 
    this->secs = secs;
    updated = true; 
}

NumericMenuItem::NumericMenuItem(std::string title, std::string units, int initial_value) 
    : TitledMenuItem(title) {
    this->units = units;
    this->value = initial_value;
}

void NumericMenuItem::draw(LiquidCrystal_I2C* lcd) {
    char buf[20];
    std::snprintf(buf, 20, "%s: %d %s", get_title().c_str(), value, units.c_str());
    lcd->print(buf);
}

bool NumericMenuItem::can_focus() { return true; }

TextMenuItem::TextMenuItem(std::string title)
    : TitledMenuItem(title) {
}

void TextMenuItem::draw(LiquidCrystal_I2C* lcd) {
    lcd->print(get_title().c_str());
}

bool TextMenuItem::can_focus() { return false; } 

Menu::Menu(std::vector<MenuItem*> items, LiquidCrystal_I2C* lcd) {
    this->items = items;
    this->lcd = lcd;
    this->active_item = items[0];
}

void Menu::display() {
    static bool first_draw = true;
    for (size_t i = 0; i < items.size(); ++i) {
        if (first_draw || items[i]->is_updated()) {
            lcd->setCursor(1, i);
            items[i]->draw(lcd);
        }
    }
    first_draw = false;
}

MenuItem* Menu::get_active_item() { return active_item; };

