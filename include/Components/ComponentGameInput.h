#ifndef BRAKEDA3D_COMPONENTGAMEINPUT_H
#define BRAKEDA3D_COMPONENTGAMEINPUT_H


#include "Component.h"
#include "../../darkheaz/Player.h"

class ComponentGameInput : public Component {
public:
    ComponentGameInput(Player *player);

    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void onSDLPollEvent(SDL_Event *event, bool &finish);

    bool enabled;
    Player *player;
    int currentWeaponIndex;

    bool isEnabled() const;

    void setEnabled(bool enable);

    void handleEscape(SDL_Event *event);

    void handleInGameInput(SDL_Event *event, bool &end);

    void handleFire() const;

    void handleWeaponSelector(SDL_Event *event);

    void handleZoom(SDL_Event *event);

    void handleMenuKeyboard(SDL_Event *event, bool &end);

    void handleKeyboardMovingPlayer();

    void handleFindClosestObject3D(SDL_Event *event);

    void handleGamePadMovingPlayer();

    void handleDashMovement(SDL_Event *pEvent);

    void handleEnergyShield(SDL_Event *event);

    void updateWeaponStatus(SDL_Event *event);

    void handleMakeGravitationalShields(SDL_Event *event);

    void handlePressKeyGameStates(SDL_Event *event);
};


#endif //BRAKEDA3D_COMPONENTGAMEINPUT_H
