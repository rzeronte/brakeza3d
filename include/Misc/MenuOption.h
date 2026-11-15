/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */

#ifndef BRAKEDA3D_MENUOPTION_H
#define BRAKEDA3D_MENUOPTION_H


#include <string>

class MenuOption {

private:
    std::string label;
    std::string alt;
    int action;

public:

    MenuOption(std::string label, std::string  alt, int action);

    std::string &getLabel();

    void setLabel(const std::string &label);

    [[nodiscard]] int getAction() const;

    void setAction(int action);

    std::string &getAlt();

    void setAlt(const std::string &alt);
};


#endif //BRAKEDA3D_MENUOPTION_H
