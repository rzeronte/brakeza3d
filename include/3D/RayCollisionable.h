//
// Created by eduardo on 18/01/23.
//

#ifndef BRAKEZA3D_RAYCOLLISIONABLE_H
#define BRAKEZA3D_RAYCOLLISIONABLE_H


#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include "../Render/Collider.h"
#include "Object3D.h"

class RayCollisionable: public Object3D {
private:
    Vertex3D ray;
    Object3D *target = nullptr;
    bool hadTarget = false;
    Vertex3D hitPosition;

protected:
    btCollisionWorld::ClosestRayResultCallback *rayCallback;
public:
    void integrate() override;

    explicit RayCollisionable(const Vertex3D &ray, int filterGroup, int filterMask);

    void resolveCollision(CollisionInfo objectWithCollision) override;

    void onUpdate() override;

    [[nodiscard]] const Vertex3D &getHitPosition() const;

    void setHitPosition(const Vertex3D &value);

    virtual void hasHit();

    [[nodiscard]] const Vertex3D &getRay() const;

    void setRay(const Vertex3D &ray);

    void setTarget(Object3D *target);

    [[nodiscard]] Object3D *getTarget() const;

    bool isHadTarget() const;
};


#endif //BRAKEZA3D_RAYCOLLISIONABLE_H
