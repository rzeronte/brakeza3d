//
// Created by eduardo on 9/3/22.
//

#ifndef BRAKEDA3D_AMMOPROJECTILEBODY_H
#define BRAKEDA3D_AMMOPROJECTILEBODY_H


#include "../../../include/Physics/Projectile3DBody.h"
#include "Weapon.h"
#include "../shaders/ShaderTrailObject.h"
#include "AmmoProjectile.h"

class AmmoProjectileBody: public Projectile3DBody, public AmmoProjectile {
    Weapon *weaponType;
    ParticleEmissor *particleEmissor;
public:
    AmmoProjectileBody(Weapon *weaponType, float damage, float ttl, const Vertex3D &direction);

    [[nodiscard]] Weapon *getWeaponType() const;

    void resolveCollision(Collisionable *collisionable) override;

    void onUpdate() override;

    ~AmmoProjectileBody() override;
};


#endif //BRAKEDA3D_AMMOPROJECTILEBODY_H
