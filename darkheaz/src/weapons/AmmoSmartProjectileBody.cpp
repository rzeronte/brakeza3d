//
// Created by eduardo on 25/3/22.
//

#include "AmmoSmartProjectileBody.h"
#include "../../../include/ComponentsManager.h"

AmmoSmartProjectileBody::AmmoSmartProjectileBody(
    float damage,
    float ttl,
    const Vertex3D &direction,
    Object3D *target
) : AmmoProjectileBody(damage, ttl, direction), target(target)
{

}


Object3D *AmmoSmartProjectileBody::getTarget() const {
    return target;
}

void AmmoSmartProjectileBody::setTarget(Object3D *o) {
    AmmoSmartProjectileBody::target = o;
}

void AmmoSmartProjectileBody::onUpdate()
{
    AmmoProjectileBody::onUpdate();

    if (target == nullptr) return;

    Vertex3D to = target->getPosition();
    Vector3D direction(getPosition(), to);

    float speed = powf(7000 / getPosition().distance(to), 3) * 0.075f;

    btVector3 btDirection;
    direction.getComponent().getScaled(speed).saveToBtVector3(&btDirection);
    getRigidBody()->applyCentralForce(btDirection);

    direction.getComponent().saveToBtVector3(&btDirection);

    getRigidBody()->applyTorque(btDirection);
}
