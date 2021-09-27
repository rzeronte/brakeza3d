//
// Created by darkhead on 14/1/20.
//

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

    bool enabled;
    Player *player;

    bool isEnabled() const;

    void setEnabled(bool enable);

    void handleEscape(SDL_Event *event);

    void handleMovingCamera(SDL_Event *, bool &);

    static void handleMouse(SDL_Event *);

    void handleInGameInput(SDL_Event *event, bool &end);

    void handleSniper(SDL_Event *event);

    void handleCrouch(SDL_Event *event) const;

    void handleFire(SDL_Event *event) const;

    void handleWeaponReload(SDL_Event *event) const;

    void handleWeaponSelector();

    void handleZoom(SDL_Event *event);

    void handleMenuKeyboard(bool &end);

    void handleRespawnAfterDead(SDL_Event *event);

    void jump(bool checkOnGround, float YForce, bool soundJump) const;
};


#endif //BRAKEDA3D_COMPONENTGAMEINPUT_H
