
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include "../../headers/Physics/SpriteDirectional3DBody.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Engine.h"

void SpriteDirectional3DBody::integrate()
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

btRigidBody* SpriteDirectional3DBody::makeRigidBody(float mass, std::vector<Object3D*> &gameObjects, Camera3D *cam, btDiscreteDynamicsWorld *world, bool applyCameraImpulse, int forceImpulse)
{
    this->mass = mass;

    btTransform trans;
    trans.setIdentity();

    Vertex3D pos = *this->getPosition();

    trans.setOrigin(btVector3(pos.x , pos.y, pos.z));

    btVector3 localInertia(0, 0, 0);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);

    btCollisionShape* shape;

    if (this->mass == 0) {
        shape = new btBoxShape(btVector3(1.5, 1.5, 1.5));
    } else {
        shape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
    }

    btRigidBody::btRigidBodyConstructionInfo cInfo(this->mass, myMotionState, shape, localInertia);
    this->m_body = new btRigidBody(cInfo);
    this->m_body->setUserPointer(this);
    this->m_body->setCcdMotionThreshold(0.01f);
    this->m_body->setCcdSweptSphereRadius(0.02f);

    world->addRigidBody(this->m_body);

    gameObjects.push_back(this);

    return this->m_body;
}

btRigidBody* SpriteDirectional3DBody::makeProjectileRigidBody(float mass, std::vector<Object3D*> &gameObjects, Camera3D *cam, btDiscreteDynamicsWorld *world, bool applyCameraImpulse, float forceImpulse, float accuracy)
{
    this->mass = mass;

    btTransform trans;
    trans.setIdentity();

    Vertex3D pos = *this->getPosition();

    Vertex3D dir;
    if (applyCameraImpulse) {
        dir = cam->getRotation().getTranspose() * AxisForward();
        pos = pos + dir.getScaled(10); // un poquito delante del Player
    }

    trans.setOrigin(btVector3(pos.x , pos.y, pos.z));

    btVector3 localInertia(0, 0, 0);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);

    btCollisionShape* shape;

    if (this->mass == 0) {
        shape = new btBoxShape(btVector3(2.5, 2.5, 2.5));
    } else {
        shape = new btBoxShape(btVector3(0.06, 0.06, 0.06));
    }

    btRigidBody::btRigidBodyConstructionInfo cInfo(this->mass, myMotionState, shape, localInertia);

    this->m_body = new btRigidBody(cInfo);
    this->m_body->setUserPointer(this);

    this->m_body->setCcdMotionThreshold(0.01f);
    this->m_body->setCcdSweptSphereRadius(0.02f);

    if (applyCameraImpulse) {
        dir = dir.getScaled(forceImpulse);
        dir.x += Tools::random(-100 + accuracy, 100 - accuracy);
        dir.y += Tools::random(-100 + accuracy, 100 - accuracy);
        dir.z += Tools::random(-100 + accuracy, 100 - accuracy);

        btVector3 impulse(dir.x, dir.y, dir.z);
        this->m_body->applyCentralImpulse(impulse);
    }

    world->addRigidBody(this->m_body, 1, 2);

    gameObjects.push_back(this);

    return this->m_body;
}

btRigidBody* SpriteDirectional3DBody::makeProjectileRigidBodyToPlayer(float mass, std::vector<Object3D*> &gameObjects, Vertex3D dir, btDiscreteDynamicsWorld* world, float forceImpulse)
{
    this->mass = mass;

    btTransform trans;
    trans.setIdentity();

    Vertex3D pos = *this->getPosition();

    pos = pos + dir.getScaled(10); // un poquito delante del NPC

    trans.setOrigin(btVector3(pos.x , pos.y, pos.z));

    btVector3 localInertia(0, 0, 0);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);

    btCollisionShape* shape;

    if (this->mass == 0) {
        shape = new btBoxShape(btVector3(2.5, 2.5, 2.5));
    } else {
        shape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
    }

    btRigidBody::btRigidBodyConstructionInfo cInfo(this->mass, myMotionState, shape, localInertia);
    this->m_body = new btRigidBody(cInfo);
    this->m_body->setUserPointer(this);
    this->m_body->setCcdMotionThreshold(0.01f);
    this->m_body->setCcdSweptSphereRadius(0.02f);

    dir = dir.getScaled(forceImpulse);
    btVector3 impulse(dir.x, dir.y, dir.z);
    this->m_body->applyCentralImpulse(impulse);

    world->addRigidBody(this->m_body);

    gameObjects.push_back(this);

    return this->m_body;
}