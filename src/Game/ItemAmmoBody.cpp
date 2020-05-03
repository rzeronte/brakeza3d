//
// Created by darkhead on 2/1/20.
//

#include "../../headers/Game/ItemAmmoBody.h"

int ItemAmmoBody::getAmount() const {
    return amount;
}

void ItemAmmoBody::setAmount(int amount) {
    ItemAmmoBody::amount = amount;
}

const std::string &ItemAmmoBody::getAmmoTypeClassname() const {
    return ammoTypeClassname;
}

void ItemAmmoBody::setAmmoTypeClassname(const std::string &ammoTypeClassname) {
    ItemAmmoBody::ammoTypeClassname = ammoTypeClassname;
}

void ItemAmmoBody::onUpdate()
{
    this->setRotation ( this->getRotation() * M3::getMatrixRotationForEulerAngles(0, 3, 0));
    Mesh3D::onUpdate();
}


btRigidBody* ItemAmmoBody::makeSimpleRigidBody(float mass, Vertex3D pos, Vertex3D dimensions, btDiscreteDynamicsWorld* world)
{
    this->mass = mass;

    btTransform trans;
    trans.setIdentity();

    trans.setOrigin(btVector3(pos.x , pos.y, pos.z));
    btVector3 localInertia(0, 0, 0);
    btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
    btCollisionShape* shape = new btBoxShape(btVector3( dimensions.x, dimensions.y, dimensions.z) );

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
    this->m_body = new btRigidBody(cInfo);

    this->m_body->setUserPointer(this);
    this->m_body->setCcdMotionThreshold(0.01f);
    this->m_body->setCcdSweptSphereRadius(0.02f);

    world->addRigidBody(this->m_body);

    return this->m_body;
}
