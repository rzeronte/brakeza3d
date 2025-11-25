
#include <utility>

#include "../../include/Misc/MenuOption.h"

MenuOption::MenuOption(std::string label, std::string  alt, int action)
:
    label(std::move(label)),
    alt(std::move(alt)),
    action(action)
{
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
