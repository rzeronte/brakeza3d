#include "../../include/Physics/Body.h"
#include "../../include/Misc/Tools.h"
#include "../../include/ComponentsManager.h"

Body::Body()
{
    this->body = nullptr;
    setMass(0);
}

void Body::setMass(float m) {
    mass = m;
}

btRigidBody *Body::getRigidBody() const {
    return body;
}

void Body::removeCollisionObject() const {
    ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeCollisionObject(getRigidBody());
}

Body::~Body()
{
}

