
#ifndef BRAKEDA3D_WEAPONSMANAGER_H
#define BRAKEDA3D_WEAPONSMANAGER_H

#include <string>
#include <SDL.h>
#include "../Render/Timer.h"
#include "WeaponAnimation.h"
#include "WeaponType.h"
#include "../Objects/Camera3D.h"

#define WEAPON_MAX_ANIMATIONS 10

class WeaponsManager {
public:
    WeaponsManager();

    int numWeaponsType = 0;
    int currentWeapon = 0;

    float headBobOffsetY = 0;

    // Global Offset When drawing
    float offsetX = 0;
    float offsetY = 0;

    WeaponType *weaponsType[WEAPON_MAX_ANIMATIONS];
    void addWeaponType(std::string);

    WeaponType *getWeaponTypeByLabel(std::string label);
    WeaponType *getWeaponTypeByClassname(std::string label);
    WeaponType *getCurrentWeaponType();

    void onUpdate(Camera3D *cam, SDL_Surface *dst);

    void headBob(Vector3D velocity);
};


#endif //BRAKEDA3D_WEAPONSMANAGER_H
