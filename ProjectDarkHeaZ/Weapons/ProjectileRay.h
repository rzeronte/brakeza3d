//
// Created by eduardo on 19/01/23.
//

#ifndef BRAKEZA3D_PROJECTILERAY_H
#define BRAKEZA3D_PROJECTILERAY_H


#include "../../include/Physics/RayCollisionable.h"
#include "../../include/Misc/Counter.h"
#include "../../include/Misc/Color.h"
#include "../../include/Physics/Projectile.h"
#include "AmmoProjectile.h"

class ProjectileRay: public RayCollisionable, public Projectile, public AmmoProjectile {
private:
    Color color;
    float speed;
    bool indestructible;
public:
    ProjectileRay(
        Object3D *parent,
        Vertex3D position,
        float damage,
        const Vertex3D &direction,
        const Vertex3D &ray,
        int filterGroup,
        int filterMask,
        float speed,
        const Color &color,
        float intensity,
        bool indestructible
    );

    void onUpdate() override;

    void resolveCollision(Collisionable *objectWithCollision) override;

    [[nodiscard]] float getSpeed() const;

    void setSpeed(float value);

    void integrate() override;
};


#endif //BRAKEZA3D_PROJECTILERAY_H
