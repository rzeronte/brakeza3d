//
// Created by eduardo on 18/01/23.
//

#ifndef BRAKEZA3D_RAYCOLLISIONABLE_H
#define BRAKEZA3D_RAYCOLLISIONABLE_H


#include "../../src/Collisions/Collisionable.h"

class RayCollisionable: public Object3D, public Collisionable {
private:
    Vertex3D ray;
    Vertex3D hitPosition;

public:
    void integrate() override;

    explicit RayCollisionable(const Vertex3D &ray);

    void resolveCollision(Collisionable *collisionable) override;

    void onUpdate() override;

    [[nodiscard]] const Vector3D &getRay() const;

    void setRay(const Vector3D &ray);

    const Vertex3D &getHitPosition() const;

    void setHitPosition(const Vertex3D &hitPosition);
};


#endif //BRAKEZA3D_RAYCOLLISIONABLE_H
