#include "../../include/Physics/Body.h"
#include "../../include/Misc/Tools.h"

Body::Body() {
    this->body = nullptr;
    this->collisionObject = nullptr;
    this->shape = nullptr;
    this->motionState = nullptr;
    setBoxShapeSize(Vertex3D(1, 1, 1));
    setMass(0);
}

void Body::setBoxShapeSize(Vertex3D size) {
    this->boxShapeSize = size;
}

Vertex3D Body::getBoxShapeSize() const {
    return this->boxShapeSize;
}

void Body::applyImpulse(Vertex3D impulse) const {

    this->body->clearForces();

    if (!Tools::isValidVector(impulse)) {
        return;
    }

    this->body->activate(true);

    btVector3 i(impulse.x, impulse.y, impulse.z);
    this->body->applyCentralImpulse(i);
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

