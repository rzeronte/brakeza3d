#ifndef BRAKEDA3D_COMPONENTGAMEINPUT_H
#define BRAKEDA3D_COMPONENTGAMEINPUT_H


#include "Component.h"
#include "../../src/Game/Player.h"

class ComponentGameInput : public Component {
public:
    ComponentGameInput(Player *player);

    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void onSDLPollEvent(SDL_Event *event, bool &finish);

    bool enabled{};
    Player *player;

    bool isEnabled() const;

    void setEnabled(bool enable);

    void handleEscape(SDL_Event *event);

    void handleKeyboardMovingCamera(SDL_Event *event, bool &end);

    static void handleMouse(SDL_Event *);

    void handleInGameInput(SDL_Event *event, bool &end);

    void handleSniper(SDL_Event *event);

    void handleFire(SDL_Event *event) const;

    void handleWeaponReload(SDL_Event *event) const;

    void handleWeaponSelector();

    void handleZoom(SDL_Event *event);

    void handleMenuKeyboard(bool &end);

    void jump(bool soundJump) const;

    void handleKeyboardMovingPlayer();
};


#endif //BRAKEDA3D_COMPONENTGAMEINPUT_H