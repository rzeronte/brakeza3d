//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTINPUT_H
#define BRAKEDA3D_COMPONENTINPUT_H


#include <SDL.h>
#include "Component.h"
#include "ComponentCamera.h"

class ComponentInput : public Component {
public:
    ComponentInput();

    void onStart();
    void preUpdate();
    void onUpdate();
    void postUpdate();
    void onEnd();
    void onSDLPollEvent(SDL_Event *event, bool &finish);

    bool debug = true;
    bool click = false;

    bool MousePressed = false;
    bool MouseMotion = false;

    bool leftButton  = false;
    bool rightButton = false;

    void handleWindowEvents(SDL_Event *e, bool &);
    void updateKeyboardMapping();
    void updateMouseStates(SDL_Event *e);
    void handleMovingCamera();
    void handleMouse(SDL_Event *);

    Uint8* keyboard;
};


#endif //BRAKEDA3D_COMPONENTINPUT_H
