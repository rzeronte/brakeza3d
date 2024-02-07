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
#include "../../include/Physics/Projectile3DBody.h"

class ProjectileRay: public Projectile3DBody, public AmmoProjectile  {
private:
    Color color;
    float speed;
    float size;
    bool indestructible;
    Object3D *target;
    bool hadTarget = false;

public:
    ProjectileRay(
        Object3D *parent,
        Vertex3D position,
        float damage,
        const Vertex3D &direction,
        float size,
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

    void setTarget(Object3D *target);

    Object3D *getTarget() const;

    bool isHadTarget() const;

    Vertex3D getRay();
};


#endif //BRAKEZA3D_PROJECTILERAY_H
