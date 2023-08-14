//
// Created by eduardo on 9/3/22.
//

#ifndef BRAKEDA3D_AMMOPROJECTILEBODY_H
#define BRAKEDA3D_AMMOPROJECTILEBODY_H


#include "../../include/Physics/Projectile3DBody.h"
#include "Weapon.h"
#include "AmmoProjectile.h"

class AmmoProjectileBody: public Projectile3DBody, public AmmoProjectile {
    Weapon *weaponType;
    ParticleEmitter *particleEmitter;
    bool wasCollision;
    Counter ending;
    Vertex3D lightOffset;
public:
    AmmoProjectileBody(
        Vertex3D position,
        Object3D *parent,
        Weapon *weaponType,
        M3 rotation,
        const Vertex3D &sizeCollision,
        const Vertex3D &direction,
        float damage,
        float speed,
        float accuracy,
        Color color,
        float intensity,
        int collisionGroup,
        int collisionMask,
        ParticleEmitter *particleEmitter
    );

    [[nodiscard]] Weapon *getWeaponType() const;

    void resolveCollision(Collisionable *collisionable) override;

    void onUpdate() override;

    void startEndingCounter();

    [[nodiscard]] bool isWasCollision() const;

    void drawOnUpdateSecondPass() override;

    ~AmmoProjectileBody() override;
};


#endif //BRAKEDA3D_AMMOPROJECTILEBODY_H
