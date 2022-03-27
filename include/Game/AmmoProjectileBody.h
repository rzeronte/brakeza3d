//
// Created by eduardo on 9/3/22.
//

#ifndef BRAKEDA3D_AMMOPROJECTILEBODY_H
#define BRAKEDA3D_AMMOPROJECTILEBODY_H


#include "../Physics/Projectile3DBody.h"
#include "Weapon.h"

class AmmoProjectileBody: public Projectile3DBody {

    Weapon *weaponType;

public:
    Weapon *getWeaponType() const;

    void setWeaponType(Weapon *weaponType);

    void resolveCollision(Collisionable *collisionable) override;

    void onUpdate() override;
};


#endif //BRAKEDA3D_AMMOPROJECTILEBODY_H
