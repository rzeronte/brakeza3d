//
// Created by eduardo on 25/3/22.
//

#include "include/AmmoSmartProjectileBody.h"
#include "../include/ComponentsManager.h"

Object3D *AmmoSmartProjectileBody::getTarget() const {
    return target;
}

void AmmoSmartProjectileBody::setTarget(Object3D *target) {
    AmmoSmartProjectileBody::target = target;
}

void AmmoSmartProjectileBody::onUpdate() {
    AmmoProjectileBody::onUpdate();

    Vertex3D to;
    if (target == nullptr) {
        auto closestObject= ComponentsManager::get()->getComponentGame()->getClosesObject3DFromPosition(getPosition(), true, false);
        if (closestObject == nullptr) {
            return;
        }
        to = closestObject->getPosition();
    } else {
        to = target->getPosition();
    }

    Vector3D direction(getPosition(), to);

    btVector3 btDirection;
    direction.getComponent().getNormalize().getScaled(3000).saveToBtVector3(&btDirection);
    getRigidBody()->setLinearVelocity(btDirection);
}
