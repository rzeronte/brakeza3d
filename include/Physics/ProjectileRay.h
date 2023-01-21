//
// Created by eduardo on 19/01/23.
//

#ifndef BRAKEZA3D_PROJECTILERAY_H
#define BRAKEZA3D_PROJECTILERAY_H


#include "RayCollisionable.h"
#include "../Misc/Counter.h"
#include "../Misc/Color.h"
#include "Projectile.h"

class ProjectileRay: public Projectile, public RayCollisionable {
private:
    Color color;
    float speed;
public:
    ProjectileRay(float ttl, const Vertex3D &direction, const Vertex3D &ray, float speed, const Color &color);

    void onUpdate() override;

    void moveDirection();

    void resolveCollision(Collisionable *collisionable) override;

    float getSpeed() const;

    void setSpeed(float speed);
};


#endif //BRAKEZA3D_PROJECTILERAY_H
