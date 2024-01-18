
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
#include "../FX/FXOffsetImage.h"

class ComponentMenu : public Component {
private:
    Image glassEffect;
    Image background;
    Image boxTutorial;
    Image border;
    Image imageLogoBox;
    std::string sceneMenu;

    std::vector<MenuOption> options;
    int currentOption;

    bool menuEnabled;
public:
    ComponentMenu();

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void setEnabled(bool enabled) override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    void drawOptions();

    void loadMenuOptions();

    static void drawVersion();

    [[nodiscard]] std::vector<MenuOption> &getOptions();

    [[nodiscard]] int getCurrentOption() const;

    [[nodiscard]] int getNumOptions() const;

    void increaseMenuOption();

    void decreaseMenuOption();

    void setMenuEnabled(bool menuEnabled);

    [[nodiscard]] bool isMenuEnabled() const;

    enum MenuActions {
        MNU_NEW_GAME = 0,
        MNU_CREDITS = 1,
        MNU_HELP = 2,
        MNU_EXIT = 3,
    };

    void LoadScene();
};


#endif //BRAKEDA3D_COMPONENTMENU_H
