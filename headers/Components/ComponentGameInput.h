//
// Created by darkhead on 14/1/20.
//

#ifndef BRAKEDA3D_COMPONENTGAMEINPUT_H
#define BRAKEDA3D_COMPONENTGAMEINPUT_H


#include "Component.h"
#include "../../src/Game/Player.h"

class ComponentGameInput : public Component{
public:
    ComponentGameInput( Player *player );

    void onStart();
    void preUpdate();
    void onUpdate();
    void postUpdate();
    void onEnd();
    void onSDLPollEvent(SDL_Event *event, bool &finish);

    Player *player;

    void handleMovingCamera(SDL_Event *, bool &);
    void handleMouse(SDL_Event *);

    void handleInGameInput(SDL_Event *event, bool &end);
    void handleSniper(SDL_Event *event);
    void handleCrouch(SDL_Event *event);
    void handleFire(SDL_Event *event);
    void handleWeaponSelector();
    void handleZoom(SDL_Event *event);

    void handleMenuKeyboard(bool &end);

    void jump(bool checkOnGround, float YForce, bool soundJump);
};


#endif //BRAKEDA3D_COMPONENTGAMEINPUT_H
