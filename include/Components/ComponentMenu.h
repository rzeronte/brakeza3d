
#ifndef BRAKEDA3D_COMPONENTMENU_H
#define BRAKEDA3D_COMPONENTMENU_H

#include <string>
#include <SDL2/SDL_events.h>
#include "../2D/MenuOption.h"
#include "../Misc/cJSON.h"
#include "Component.h"
#include "../Objects/Mesh3D.h"
#include "../../darkheaz/src/shaders/ShaderImage.h"
#include "../Objects/Mesh3DAnimated.h"
#include "../Physics/Mesh3DAnimatedGhost.h"
#include "../Physics/SimplePendulum.h"
#include "../../darkheaz/src/shaders/ShaderImageMask.h"

class ComponentMenu : public Component {
private:
    ShaderImage *shaderBackgroundImage;
    ShaderImageMask *shaderMenuTitle;

    Mesh3D *planet;

    LightPoint3D *light;
    SimplePendulum *pendulum;

    std::vector<MenuOption> options;
    int currentOption;
    Image *border;
public:
    ComponentMenu();

    virtual ~ComponentMenu();

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void setEnabled(bool enabled) override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    void drawOptions();

    void loadMenuOptions();

    void loadDecorative3DMesh();

    void drawVersion();

    [[nodiscard]] std::vector<MenuOption> &getOptions();

    [[nodiscard]] int getCurrentOption() const;

    [[nodiscard]] int getNumOptions() const;

    void increaseMenuOption();

    void decreaseMenuOption();

    enum MenuActions {
        MNU_NEW_GAME = 0,
        MNU_CREDITS = 1,
        MNU_HELP = 2,
        MNU_EXIT = 3,
    };
};


#endif //BRAKEDA3D_COMPONENTMENU_H
