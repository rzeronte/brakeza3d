
#ifndef BRAKEDA3D_COMPONENTMENU_H
#define BRAKEDA3D_COMPONENTMENU_H

#include <string>
#include <SDL2/SDL_events.h>
#include "../2D/MenuOption.h"
#include "../Misc/cJSON.h"
#include "Component.h"

#define MAX_MENU_OPTIONS 10

class ComponentMenu : public Component {
public:
    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void onSDLPollEvent(SDL_Event *event, bool &finish);

    // menu background surface
    SDL_Surface *menu_background;

    ComponentMenu();

    MenuOption *options[MAX_MENU_OPTIONS];
    int numOptions;

    int currentOption;

    enum MenuActions {
        MNU_NEW_GAME = 0,
        MNU_CREDITS = 1,
        MNU_OPTIONS = 2,
        MNU_EXIT = 3,
    };

    // menu.json
    cJSON *optionsJSON;

    void drawOptions(SDL_Surface *dst);

    void loadMenuOptions();
};


#endif //BRAKEDA3D_COMPONENTMENU_H
