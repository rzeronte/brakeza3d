#include "../../include/Physics/Body.h"
#include "../../include/Misc/Tools.h"
#include "../../include/ComponentsManager.h"

Body::Body()
    :
    typeShape(BodyTypeShape::BODY_SIMPLE_SHAPE),
    mass(0),
    body(nullptr)
{
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
    if (body != nullptr) {
        this->removeCollisionObject();
    }
    delete body;
}

BodyTypeShape Body::getTypeShape() const {
    return typeShape;
}

void Body::setTypeShape(BodyTypeShape typeShape) {
    Body::typeShape = typeShape;
}

