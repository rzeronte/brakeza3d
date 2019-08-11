
#ifndef BRAKEDA3D_MENU_H
#define BRAKEDA3D_MENU_H

#include <string>
#include "MenuOption.h"
#include "../../src/cJSON.h"

#define MAX_MENU_OPTIONS 10

class Menu {
public:

    Menu();
    MenuOption *options[MAX_MENU_OPTIONS];
    int numOptions;

    int currentOptions;

    // menu.json
    cJSON *optionsJSON;

    void drawOptions(SDL_Surface *dst);
    void getOptionsJSON();

    void addOption(std::string label, std::string imageOn, std::string imageOff);
};


#endif //BRAKEDA3D_MENU_H
