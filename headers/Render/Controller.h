//
// Created by darkhead on 28/4/18.
//

#ifndef SDL2_3D_ENGINE_CONTROLLER_H
#define SDL2_3D_ENGINE_CONTROLLER_H

#include "../Objects/Camera3D.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/Object3D.h"
#include "btBulletDynamicsCommon.h"
#include <SDL.h>

class Controller {

    bool debug = true;
    bool click = false;

    bool MousePressed = false;
    bool MouseMotion = false;

    Uint8* keyboard;

public:
    Controller();

    void handleKeyboard(Camera3D *, bool &, btDiscreteDynamicsWorld* dynamicsWorld );
    void handleMouse(SDL_Event *, Camera3D *);

};


#endif //SDL2_3D_ENGINE_CONTROLLER_H
