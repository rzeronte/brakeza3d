
#include "../../include/Game/Projectile3DBody.h"

Projectile3DBody::Projectile3DBody() : fromEnemy(false), damage(0), damageRadius(0) {

}

bool Projectile3DBody::isFromEnemy() const {
    return fromEnemy;
}

void Projectile3DBody::setFromEnemy(bool isFromEnemy) {
    fromEnemy = isFromEnemy;
}

float Projectile3DBody::getDamage() const {
    return damage;
}

void Projectile3DBody::setDamage(float newDamage) {
    Projectile3DBody::damage = newDamage;
}

float Projectile3DBody::getDamageRadius() const {
    return damageRadius;
}

void Projectile3DBody::setDamageRadius(float newDamageRadius) {
    Projectile3DBody::damageRadius = newDamageRadius;
}


btRigidBody *
Projectile3DBody::makeProjectileRigidBody(float mass, Vertex3D size, Camera3D *cam, btDiscreteDynamicsWorld *world,
                                          bool applyCameraImpulse, float forceImpulse, float accuracy) {
    this->mass = mass;

    btTransform trans;
    trans.setIdentity();

    Vertex3D pos = this->getPosition();

    Vertex3D dir;
    dir = cam->getRotation().getTranspose() * AxisForward();

    trans.setOrigin(btVector3(pos.x, pos.y, pos.z));

    btVector3 localInertia(0, 0, 0);

    auto *myMotionState = new btDefaultMotionState(trans);

    btVector3 btSize;
    size.saveToBtVector3(&btSize);
    btCollisionShape *shape = new btBoxShape(btSize);

    btRigidBody::btRigidBodyConstructionInfo cInfo(this->mass, myMotionState, shape, localInertia);

    this->m_body = new btRigidBody(cInfo);
    this->m_body->setUserPointer(this);
    this->m_body->setCcdMotionThreshold(0.01f);
    this->m_body->setCcdSweptSphereRadius(0.02f);

    if (applyCameraImpulse) {
        dir = dir.getScaled(forceImpulse);
        dir.x += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy));
        dir.y += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy));
        dir.z += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy));

        btVector3 impulse(dir.x, dir.y, dir.z);
        this->m_body->applyCentralImpulse(impulse);
    }

    world->addRigidBody(this->m_body, 1, 2);

    return this->m_body;
}
