//
// Created by darkhead on 28/4/18.
//

#ifndef SDL2_3D_ENGINE_CONTROLLER_H
#define SDL2_3D_ENGINE_CONTROLLER_H

#include "../Objects/Camera3D.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/Object3D.h"
#include "btBulletDynamicsCommon.h"
#include "../Physics/SpriteDirectional3DBody.h"
#include "../2D/MenuManager.h"
#include <SDL.h>
#include "../Render/Billboard.h"
#include "../2D/WeaponsManager.h"

class InputController {

    bool debug = true;
    bool click = false;

    bool MousePressed = false;
    bool MouseMotion = false;

    Uint8* keyboard;

public:
    InputController();

    void handleKeyboardContinuous(SDL_Event *, bool &);
    void handleKeyboard(SDL_Event *e, bool &);

    void handleMouse(SDL_Event *);
};


#endif //SDL2_3D_ENGINE_CONTROLLER_H
