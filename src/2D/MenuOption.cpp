
#include "../../headers/2D/MenuOption.h"

MenuOption::MenuOption(const std::string label, int action) {
    this->label = label;
    this->action = action;
}

std::string &MenuOption::getLabel() {
    return label;
}

void MenuOption::setLabel(const std::string &label) {
    MenuOption::label = label;
}

int MenuOption::getAction() const {
    return action;
}

void MenuOption::setAction(int action) {
    MenuOption::action = action;
}

std::string &MenuOption::getAlt() {
    return alt;
}

void MenuOption::setAlt(const std::string &alt) {
    MenuOption::alt = alt;
}
