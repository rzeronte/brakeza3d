#include "../../include/Physics/Body.h"
#include "../../include/Misc/Tools.h"
#include "../../include/ComponentsManager.h"

Body::Body() {
    this->body = nullptr;
    this->collisionObject = nullptr;
    this->shape = nullptr;
    this->motionState = nullptr;
    setMass(0);
}

void Body::setMass(float m) {
    mass = m;
}

btCollisionObject *Body::getCollisionObject() const {
    return collisionObject;
}

btRigidBody *Body::getRigidBody() const {
    return body;
}

void Body::removeCollisionObject() const {
    ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeCollisionObject(getRigidBody());
}

