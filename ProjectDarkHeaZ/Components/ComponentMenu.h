
#ifndef BRAKEDA3D_COMPONENTMENU_H
#define BRAKEDA3D_COMPONENTMENU_H

#include <string>
#include <SDL2/SDL_events.h>
#include "../../include/2D/MenuOption.h"
#include "../../include/Misc/cJSON.h"
#include "../../include/Components/Component.h"
#include "../../include/Objects/Mesh3D.h"
#include "../../include/Objects/Mesh3DAnimated.h"
#include "../../include/Physics/Mesh3DAnimatedGhost.h"
#include "../../include/Physics/SimplePendulum.h"
#include "../../include/Shaders/ShaderImage.h"
#include "../../include/Shaders/ShaderImageMask.h"

class ComponentMenu : public Component {
private:
    ShaderImage *shaderBackgroundImage;
    ShaderImageMask *shaderMenuTitle;

    Mesh3D *planet;

    SimplePendulum *pendulum;

    std::vector<MenuOption> options;
    int currentOption;
    Image *border;
    Image *imageLogoBox;
    bool menuEnabled;
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

    void setMenuEnabled(bool menuEnabled);

    bool isMenuEnabled() const;

    Image *getBorder() const;

    enum MenuActions {
        MNU_NEW_GAME = 0,
        MNU_CREDITS = 1,
        MNU_HELP = 2,
        MNU_EXIT = 3,
    };
    Mesh3D *hexagonStation;
};


#endif //BRAKEDA3D_COMPONENTMENU_H
