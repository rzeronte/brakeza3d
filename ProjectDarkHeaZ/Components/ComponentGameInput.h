#ifndef BRAKEDA3D_COMPONENTGAMEINPUT_H
#define BRAKEDA3D_COMPONENTGAMEINPUT_H


#include "../../include/Components/Component.h"

class ComponentGameInput : public Component {
private:
    float controllerAxisThreshold;
    bool lockRightStick;
    bool wasSpacePressed = false;

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

    static void handleWeaponSelector(SDL_Event *event);

    void handleZoom(SDL_Event *event);

    void handleMenuKeyboard(SDL_Event *event, bool &end);

    void handleKeyboardMovingPlayer();

    void handleFindClosestObject3D(SDL_Event *event);

    void handleGamePadMovingPlayer();

    void handleDashMovement(SDL_Event *pEvent);

    static void handleEnergyShield(SDL_Event *event);

    void handleMakeReflection(SDL_Event *event);

    void handlePressKeyGameStates(SDL_Event *event);

    static void handleBomb(SDL_Event *pEvent);

    [[nodiscard]] float getControllerAxisThreshold() const;

    void updateWeaponStatus(SDL_Event *event);

    static void handleShield(SDL_Event *event);
};


#endif //BRAKEDA3D_COMPONENTGAMEINPUT_H
