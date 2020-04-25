
#include "../../headers/Physics/Sprite3DBody.h"
#include "../../headers/Physics/Body.h"
#include "../../headers/Misc/Tools.h"
#include <vector>

void Sprite3DBody::integrate()
{
    if (this->mass == 0) {
        //return;
    }

    // Sync position
    btTransform t ;
    m_body->getMotionState()->getWorldTransform(t);
    btVector3 pos = t.getOrigin();

    Vertex3D worldPosition = Vertex3D(pos.getX() , pos.getY() , pos.getZ());
    this->setPosition(worldPosition);

    // Sync rotation
    btQuaternion  quat = t.getRotation();
    float angle = quat.getAngle();
    btVector3 axis = quat.getAxis();
}

btRigidBody* Sprite3DBody::makeRigidBody(float mass, Vertex3D size, btDiscreteDynamicsWorld *world)
{
    this->mass = mass;

    btTransform trans;
    trans.setIdentity();

    Vertex3D pos = this->getPosition();

    trans.setOrigin(btVector3(pos.x , pos.y, pos.z));

    btVector3 localInertia(0, 0, 0);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);

    btVector3 btSize; size.saveToBtVector3(&btSize);
    btCollisionShape* shape = new btBoxShape(btSize);

    btRigidBody::btRigidBodyConstructionInfo cInfo(this->mass, myMotionState, shape, localInertia);
    this->m_body = new btRigidBody(cInfo);
    this->m_body->setUserPointer(this);
    this->m_body->setCcdMotionThreshold(0.001f);
    this->m_body->setCcdSweptSphereRadius(0.02f);

    world->addRigidBody(this->m_body);

    return this->m_body;
}