#include "menu.h"

void Menu::display() {
    lcd->clear();
    for (size_t i = 0; i < items.size(); ++i) {
        lcd->setCursor(0, i);
        lcd->print(items[i]->get_text().c_str());
    }
}
