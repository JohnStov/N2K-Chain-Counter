#include "menu_item.h"

bool MenuItem::is_updated() { return false; };

TitledMenuItem::TitledMenuItem(std::string title) {
    this->title = title;
}

const std::string TitledMenuItem::get_title() {
    return title;
}
