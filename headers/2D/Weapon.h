
#ifndef BRAKEDA3D_WEAPON_H
#define BRAKEDA3D_WEAPON_H

#include <string>
#include <SDL.h>
#include "../Render/Timer.h"
#include "WeaponAnimation.h"
#include "WeaponType.h"
#include "../Objects/Camera3D.h"

#define WEAPON_MAX_ANIMATIONS 10

class Weapon {
public:
    Weapon();

    int numWeaponsType = 0;
    int currentWeapon = 0;

    WeaponType *weaponsType[WEAPON_MAX_ANIMATIONS];
    void addWeaponType(std::string);

    WeaponType *getWeaponTypeByLabel(std::string label);
    WeaponType *getCurrentWeaponType();

    void setAction(Camera3D *cam, bool isFiring);

    void onUpdate(Camera3D *cam, bool isFiring, SDL_Surface *dst);
};


#endif //BRAKEDA3D_WEAPON_H
