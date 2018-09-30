//
// Created by darkhead on 28/4/18.
//

#ifndef SDL2_3D_ENGINE_CONTROLLER_H
#define SDL2_3D_ENGINE_CONTROLLER_H

#include "Camera.h"
#include "Mesh.h"
#include "Object3D.h"
#include <SDL.h>

class Controller {

    Uint32 event_type;

    bool debug = true;
    bool click = false;

    SDL_Keycode key_pressed;

    bool MousePressed = false;
    bool Mousefirst  = true;
    bool MouseMotion = false;

    float MouseRelX;
    float MouseRelY;


public:
    Controller();

    void handleKeyboard(SDL_Event *, Camera *, bool &);
    void handleMouse(SDL_Event *, Camera *);

    void handleKeyboardObject3D(SDL_Event *event, Object3D *object);
    void updateKeyboardRead(SDL_Event *event);
};


#endif //SDL2_3D_ENGINE_CONTROLLER_H
