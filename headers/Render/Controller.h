//
// Created by darkhead on 28/4/18.
//

#ifndef SDL2_3D_ENGINE_CONTROLLER_H
#define SDL2_3D_ENGINE_CONTROLLER_H

#include "../Objects/Camera3D.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/Object3D.h"
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

    void handleKeyboard(SDL_Event *, Camera3D *, bool &);
    void handleMouse(SDL_Event *, Camera3D *);

    void updateKeyboardRead(SDL_Event *event);
};


#endif //SDL2_3D_ENGINE_CONTROLLER_H
