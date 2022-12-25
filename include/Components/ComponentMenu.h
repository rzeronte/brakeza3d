
#ifndef BRAKEDA3D_COMPONENTMENU_H
#define BRAKEDA3D_COMPONENTMENU_H

#include <string>
#include <SDL2/SDL_events.h>
#include "../2D/MenuOption.h"
#include "../Misc/cJSON.h"
#include "Component.h"
#include "../Objects/Mesh3D.h"
#include "../../darkheaz/src/shaders/ShaderBackgroundImage.h"
#include "../Objects/Mesh3DAnimated.h"
#include "../Physics/Mesh3DAnimatedGhost.h"
#include "../Physics/SimplePendulum.h"

#define MAX_MENU_OPTIONS 10

class ComponentMenu : public Component {
public:
    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void setEnabled(bool enabled);

    void onSDLPollEvent(SDL_Event *event, bool &finish);

    // menu background surface
    ShaderImage *shaderBackgroundImage;

    Image *title;
    Mesh3D *spaceship;
    LightPoint3D *light;
    SimplePendulum *pendulum;

    ComponentMenu();

    MenuOption *options[MAX_MENU_OPTIONS];
    int numOptions;

    int currentOption;

    enum MenuActions {
        MNU_NEW_GAME = 0,
        MNU_CREDITS = 1,
        MNU_HELP = 2,
        MNU_EXIT = 3,
    };

    // menu.json
    cJSON *optionsJSON;

    void drawOptions();

    void loadMenuOptions();

    void loadDecorative3DMesh();

    void drawVersion();
};


#endif //BRAKEDA3D_COMPONENTMENU_H
