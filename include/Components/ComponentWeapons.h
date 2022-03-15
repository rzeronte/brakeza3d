
#ifndef BRAKEDA3D_COMPONENTWEAPONS_H
#define BRAKEDA3D_COMPONENTWEAPONS_H

#include <string>
#include <SDL2/SDL.h>
#include "../Misc/Timer.h"
#include "../Game/WeaponType.h"
#include "Camera3D.h"
#include "../Game/AmmoType.h"
#include "Component.h"
#include "ComponentCamera.h"




class ComponentWeapons : public Component {
public:
    ComponentWeapons();

    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void onSDLPollEvent(SDL_Event *event, bool &finish);
};


#endif //BRAKEDA3D_COMPONENTWEAPONS_H
