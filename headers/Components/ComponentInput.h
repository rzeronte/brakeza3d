//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTINPUT_H
#define BRAKEDA3D_COMPONENTINPUT_H


#include <SDL2/SDL.h>
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

    bool enabled;

    bool debug = true;
    bool click = false;

    bool MousePressed = false;
    bool MouseMotion = false;

    bool leftButton  = false;
    bool rightButton = false;

    bool isEnabled() const;
    void setEnabled(bool enabled);

    void handleWindowEvents(SDL_Event *e, bool &);
    void updateKeyboardMapping();
    void updateMouseStates(SDL_Event *e);
    void handleKeyboardMovingCamera();
    void handleMouse(SDL_Event *);

    Uint8* keyboard;
};


#endif //BRAKEDA3D_COMPONENTINPUT_H
