#include "menu.h"

void Menu::display() {
    //if (changed)
    {
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i]->is_updated()) {
                lcd->setCursor(0, i);
                items[i]->draw(lcd);
            }
        }
    }
    changed = false;
}
