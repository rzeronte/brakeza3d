
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

    enum MenuActions {
        MNU_NEW_GAME = 0,
        MNU_CREDITS  = 1,
        MNU_OPTIONS  = 2,
        MNU_EXIT     = 3,
    };

    // menu.json
    cJSON *optionsJSON;

    void drawOptions(SDL_Surface *dst);
    void getOptionsJSON();
};


#endif //BRAKEDA3D_MENUMANAGER_H
