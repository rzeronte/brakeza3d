//
// Created by darkhead on 28/4/18.
//

#ifndef SDL2_3D_ENGINE_CONTROLLER_H
#define SDL2_3D_ENGINE_CONTROLLER_H

#include "../Components/Camera3D.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/Object3D.h"
#include "btBulletDynamicsCommon.h"
#include "../Physics/SpriteDirectional3DBody.h"
#include "../Components/ComponentMenu.h"
#include <SDL.h>
#include "../Render/Billboard.h"
#include "../Components/ComponentWeapons.h"

class InputController {

public:

    bool debug = true;
    bool click = false;

    bool MousePressed = false;
    bool MouseMotion = false;

    bool leftButton  = false;
    bool rightButton = false;

    InputController();

    void handleWindowEvents(SDL_Event *e, bool &);
    void updateKeyboardMapping();
    void updateMouseStates(SDL_Event *event);

    virtual void handleMovingCamera(SDL_Event *e, bool &);
    virtual void handleMouse(SDL_Event *);

    Uint8* keyboard;
};


#endif //SDL2_3D_ENGINE_CONTROLLER_H
