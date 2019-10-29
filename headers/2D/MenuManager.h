
#ifndef BRAKEDA3D_MENUMANAGER_H
#define BRAKEDA3D_MENUMANAGER_H

#include <string>
#include "MenuOption.h"
#include "../Misc/cJSON.h"

#define MAX_MENU_OPTIONS 10

class MenuManager {
public:
    // menu background surface
    SDL_Surface *menu_background;

    MenuManager();
    MenuOption *options[MAX_MENU_OPTIONS];
    int numOptions;

    int currentOption;

    // menu.json
    cJSON *optionsJSON;

    void drawOptions(SDL_Surface *dst);
    void getOptionsJSON();

    void addOption(std::string label, std::string imageOn, std::string imageOff);
};


#endif //BRAKEDA3D_MENUMANAGER_H
