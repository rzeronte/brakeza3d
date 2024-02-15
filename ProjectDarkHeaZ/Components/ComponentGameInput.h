#ifndef BRAKEDA3D_COMPONENTGAMEINPUT_H
#define BRAKEDA3D_COMPONENTGAMEINPUT_H


#include "../../include/Components/Component.h"

struct KeyboardMapping {
    SDL_Scancode left;
    SDL_Scancode up;
    SDL_Scancode down;
    SDL_Scancode right;
    SDL_Scancode fire;
    SDL_Scancode nextWeapon;
    SDL_Scancode previousWeapon;
    SDL_Scancode intro;
    SDL_Scancode selectTarget;
    SDL_Scancode dash;
    SDL_Scancode bomb;
    SDL_Scancode shield;
    SDL_Scancode reflection;
    SDL_Scancode energyShield;
    SDL_Scancode weapon1;
    SDL_Scancode weapon2;
    SDL_Scancode weapon3;
    SDL_Scancode esc;
    SDL_Scancode menuLeft;
    SDL_Scancode menuUp;
    SDL_Scancode menuDown;
    SDL_Scancode menuRight;
};

class ComponentGameInput : public Component {
private:
    float controllerAxisThreshold;
    bool lockRightStick;
    bool wasSpacePressed = false;
    KeyboardMapping mapping;
public:
    explicit ComponentGameInput();

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    void handleEscape(SDL_Event *event);

    void handleInGameInput(SDL_Event *event, bool &end);

    void handleFire() const;

    void handleWeaponSelector(SDL_Event *event);

    void handleZoom(SDL_Event *event);

    void handleMenuKeyboard(SDL_Event *event, bool &end) const;

    void handleKeyboardMovingPlayer() const;

    void handleFindClosestObject3D(SDL_Event *event);

    static void handleGamePadMovingPlayer();

    void handleDashMovement(SDL_Event *pEvent) const;

    void handleEnergyShield(SDL_Event *event) const;

    void handleMakeReflection(SDL_Event *event) const;

    void handlePressKeyGameStates(SDL_Event *event);

    void handleBomb(SDL_Event *pEvent) const;

    [[nodiscard]] float getControllerAxisThreshold() const;

    void updateWeaponStatus(SDL_Event *event);

    void handleShield(SDL_Event *event) const;

    void setMapping(const KeyboardMapping &mapping);

    bool isFiring() const;
};


#endif //BRAKEDA3D_COMPONENTGAMEINPUT_H
