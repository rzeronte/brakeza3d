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

public:

    bool debug = true;
    bool click = false;

    bool MousePressed = false;
    bool MouseMotion = false;

    bool leftButton  = false;
    bool rightButton = false;

    InputController();

    virtual void handleKeyboardContinuous(SDL_Event *, bool &);
    virtual void handleKeyboard(SDL_Event *e, bool &);
    virtual void handleMouse(SDL_Event *);
    void updateKeyboardMapping();
    void updateMouseStates(SDL_Event *event);

    Uint8* keyboard;
};


#endif //SDL2_3D_ENGINE_CONTROLLER_H
