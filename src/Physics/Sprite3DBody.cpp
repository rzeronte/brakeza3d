
#include "../../headers/Physics/Sprite3DBody.h"
#include "../../headers/Physics/Body.h"
#include "../../headers/Render/Tools.h"
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

btRigidBody* Sprite3DBody::makeRigidBody(float mass, std::vector<Object3D*> &gameObjects, Camera3D *cam, btDiscreteDynamicsWorld *world, Object3D *originForces)
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
        shape = new btBoxShape(btVector3(2.5, 2.5, 2.5));
    } else {
        shape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
    }

    btRigidBody::btRigidBodyConstructionInfo cInfo(this->mass, myMotionState, shape, localInertia);
    this->m_body = new btRigidBody(cInfo);
    this->m_body->setUserPointer(this);
    this->m_body->setCcdMotionThreshold(0.001f);
    this->m_body->setCcdSweptSphereRadius(0.02f);


    Vertex3D up = EngineSetup::getInstance()->up.getInverse();

    double r1 = ((double) rand() / (RAND_MAX)) + 1;
    double r2 = ((double) rand() / (RAND_MAX)) + 1;
    double r3 = ((double) rand() / (RAND_MAX)) + 1;
    Vertex3D randomNoise = Vertex3D(r1, 0, r3);
    randomNoise.consoleInfo("randomNose", false);

    int rand = Tools::random(0, 100);

    Vertex3D dir;
    if (rand > 50) {
        dir = up + randomNoise;
    } else {
        dir = up - randomNoise;
    }
    dir = dir.getNormalize().getScaled(100);

    dir.consoleInfo("gib:", false);

    btVector3 impulse(dir.x, dir.y, dir.z);
    this->m_body->applyCentralImpulse(impulse);

    world->addRigidBody(this->m_body);
    gameObjects.push_back(this);

    return this->m_body;
}