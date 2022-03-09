//
// Created by eduardo on 9/3/22.
//

#ifndef BRAKEDA3D_AMMOPROJECTILEGHOST_H
#define BRAKEDA3D_AMMOPROJECTILEGHOST_H


#include "../Physics/Projectile3DBody.h"
#include "WeaponType.h"

class AmmoProjectileGhost: public Projectile3DBody {

    WeaponType *weaponType;

public:
    WeaponType *getWeaponType() const;

    void setWeaponType(WeaponType *weaponType);
};


#endif //BRAKEDA3D_AMMOPROJECTILEGHOST_H
