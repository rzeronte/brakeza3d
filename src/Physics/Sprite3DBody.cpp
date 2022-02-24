
#include "../../include/Physics/Sprite3DBody.h"

void Sprite3DBody::integrate() {
    if (this->mass == 0) {
        //return;
    }

    // Sync position
    btTransform t;
    body->getMotionState()->getWorldTransform(t);
    btVector3 pos = t.getOrigin();

    Vertex3D worldPosition = Vertex3D(pos.getX(), pos.getY(), pos.getZ());
    this->setPosition(worldPosition);

    // Sync rotation
    //btQuaternion quat = t.getRotation();
    //float angle = quat.getAngle();
    //btVector3 axis = quat.getAxis();
}

btRigidBody *Sprite3DBody::makeRigidBody(float mass, Vertex3D size, btDiscreteDynamicsWorld *world) {
    this->mass = mass;

    btTransform trans;
    trans.setIdentity();

    Vertex3D pos = this->getPosition();

    trans.setOrigin(btVector3(pos.x, pos.y, pos.z));

    btVector3 localInertia(0, 0, 0);

    auto *myMotionState = new btDefaultMotionState(trans);

    btVector3 btSize;
    size.saveToBtVector3(&btSize);
    btCollisionShape *shape = new btBoxShape(btSize);

    btRigidBody::btRigidBodyConstructionInfo cInfo(this->mass, myMotionState, shape, localInertia);
    this->body = new btRigidBody(cInfo);
    this->body->setUserPointer(dynamic_cast<Body *> (this));
    this->body->setCcdMotionThreshold(0.001f);
    this->body->setCcdSweptSphereRadius(0.02f);

    world->addRigidBody(this->body);

    return this->body;
}