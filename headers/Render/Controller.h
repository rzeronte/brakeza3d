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
#include "../2D/Menu.h"
#include <SDL.h>
#include "../Render/Billboard.h"
#include "../2D/Weapon.h"

class Controller {

    bool debug = true;
    bool click = false;

    bool MousePressed = false;
    bool MouseMotion = false;

    bool firing = false;

    Uint8* keyboard;

public:
    Controller();

    void handleKeyboardContinuous(SDL_Event *, Camera3D *, bool &, btDiscreteDynamicsWorld* dynamicsWorld, std::vector<SpriteDirectional3DBody*> &, Timer *, SpriteDirectional3D *bullet, Menu *menu, Weapon *weapon);
    void handleKeyboard(SDL_Event *, Camera3D *, bool &, btDiscreteDynamicsWorld* dynamicsWorld, std::vector<SpriteDirectional3DBody*> &, Timer *, SpriteDirectional3D *bullet, Menu *menu, Weapon *weapon);

    void handleMouse(SDL_Event *, Camera3D *, btDiscreteDynamicsWorld* dynamicsWorld, std::vector<SpriteDirectional3DBody*> &projectiles, Timer *timer, SpriteDirectional3D *bulletTemplate, Menu *menu, Weapon *weapon);

    bool isFiring();
    void resetFlags();
};


#endif //SDL2_3D_ENGINE_CONTROLLER_H
