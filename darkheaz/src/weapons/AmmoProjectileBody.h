//
// Created by eduardo on 9/3/22.
//

#ifndef BRAKEDA3D_AMMOPROJECTILEBODY_H
#define BRAKEDA3D_AMMOPROJECTILEBODY_H


#include "../../../include/Physics/Projectile3DBody.h"
#include "Weapon.h"
#include "../shaders/ShaderTrailObject.h"

class AmmoProjectileBody: public Projectile3DBody {
    Weapon *weaponType;
public:
    AmmoProjectileBody(float ttl, const Vertex3D &direction);

    [[nodiscard]] Weapon *getWeaponType() const;

    void setWeaponType(Weapon *weapon);

    void resolveCollision(Collisionable *collisionable) override;

    void onUpdate() override;
};


#endif //BRAKEDA3D_AMMOPROJECTILEBODY_H
