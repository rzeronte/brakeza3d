//
// Created by eduardo on 25/3/22.
//

#include "../../include/Game/AmmoSmartProjectileBody.h"
#include "../../include/ComponentsManager.h"

Object3D *AmmoSmartProjectileBody::getTarget() const {
    return target;
}

void AmmoSmartProjectileBody::setTarget(Object3D *target) {
    AmmoSmartProjectileBody::target = target;
}

void AmmoSmartProjectileBody::onUpdate() {
    AmmoProjectileBody::onUpdate();

    auto closestObject= ComponentsManager::get()->getComponentGame()->getClosesObject3DFromPosition(getPosition(), true, false);
    if (closestObject == nullptr) {
        return;
    }

    Vector3D direction(getPosition(), closestObject->getPosition());

    btVector3 btDirection;
    direction.getComponent().getNormalize().getScaled(3000).saveToBtVector3(&btDirection);
    getRigidBody()->setLinearVelocity(btDirection);
}
