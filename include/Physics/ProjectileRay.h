//
// Created by eduardo on 19/01/23.
//

#ifndef BRAKEZA3D_PROJECTILERAY_H
#define BRAKEZA3D_PROJECTILERAY_H


#include "RayCollisionable.h"
#include "../Misc/Counter.h"
#include "../Misc/Color.h"
#include "Projectile.h"
#include "../../darkheaz/src/weapons/AmmoProjectile.h"

class ProjectileRay: public RayCollisionable, public Projectile, public AmmoProjectile {
private:
    Color color;
    int speed;
    bool indestructible;
public:
    ProjectileRay(
        float damage,
        const Vertex3D &direction,
        const Vertex3D &ray,
        int filterGroup,
        int filterMask,
        int speed,
        const Color &color,
        bool indestructible
    );

    void onUpdate() override;

    void resolveCollision(Collisionable *objectWithCollision) override;

    [[nodiscard]] int getSpeed() const;

    void setSpeed(int value);

    void integrate() override;
};


#endif //BRAKEZA3D_PROJECTILERAY_H
