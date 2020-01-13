//
// Created by darkhead on 29/12/19.
//

#include "../../headers/Physics/BillboardBody.h"
#include "../../headers/EngineSetup.h"

BillboardBody::BillboardBody()
{

}

void BillboardBody::integrate()
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

btRigidBody* BillboardBody::makeRigidBody(float mass, Vertex3D size, std::vector<Object3D*> &gameObjects, btDiscreteDynamicsWorld *world)
{
    this->mass = mass;

    btTransform trans;
    trans.setIdentity();

    Vertex3D pos = *this->getPosition();

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
    gameObjects.emplace_back(this);

    return this->m_body;
}

void BillboardBody::updateTrianglesCoordinatesAndTexture(Camera3D *cam)
{
    Vertex3D up = cam->getRotation().getTranspose() * EngineSetup::getInstance()->up;
    Vertex3D right = cam->getRotation().getTranspose() * EngineSetup::getInstance()->right;

    this->updateUnconstrainedQuad(this, up, right );
    this->reassignTexture();
}