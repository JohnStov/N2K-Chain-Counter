#include "menu.h"

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
