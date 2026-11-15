/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */

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
