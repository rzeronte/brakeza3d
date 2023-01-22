//
// Created by eduardo on 18/01/23.
//

#ifndef BRAKEZA3D_RAYCOLLISIONABLE_H
#define BRAKEZA3D_RAYCOLLISIONABLE_H


#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include "../../src/Collisions/Collisionable.h"

class RayCollisionable: public Object3D, public Collisionable {
private:
    btCollisionWorld::ClosestRayResultCallback *rayCallback;
    Vertex3D ray;
    Vertex3D hitPosition;

public:
    void integrate() override;

    explicit RayCollisionable(const Vertex3D &ray);

    void resolveCollision(Collisionable *collisionable) override;

    void onUpdate() override;

    [[nodiscard]] const Vertex3D &getHitPosition() const;

    void setHitPosition(const Vertex3D &value);

    virtual void hasHit();

    [[nodiscard]] btCollisionWorld::ClosestRayResultCallback *getRayCallback() const;

    const Vertex3D &getRay() const;
};


#endif //BRAKEZA3D_RAYCOLLISIONABLE_H
