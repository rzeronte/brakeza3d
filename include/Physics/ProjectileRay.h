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

class ProjectileRay: public Projectile, public RayCollisionable, public AmmoProjectile {
private:
    Color color;
    int speed;
public:
    ProjectileRay(float ttl, float damage, const Vertex3D &direction, const Vertex3D &ray, int speed, const Color &color);

    void onUpdate() override;

    void moveDirection();

    void resolveCollision(Collisionable *collisionable) override;

    [[nodiscard]] int getSpeed() const;

    void setSpeed(int value);

    void integrate() override;

    void hasHit() override;

    [[nodiscard]] const Color &getColor() const;

    void setColor(const Color &value);
};


#endif //BRAKEZA3D_PROJECTILERAY_H
