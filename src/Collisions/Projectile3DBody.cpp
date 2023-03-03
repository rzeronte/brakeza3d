
#include "../../include/Physics/Projectile3DBody.h"
#include "../../include/ComponentsManager.h"

Projectile3DBody::Projectile3DBody(
    float ttl,
    const Vertex3D &direction
) : Projectile(ttl, direction)
{
}

void Projectile3DBody::makeProjectileRigidBody(
    float mass,
    Vertex3D size,
    Vertex3D direction,
    M3 rotation,
    float forceImpulse,
    float accuracy,
    btDiscreteDynamicsWorld *world,
    int collisionGroup,
    int collisionMask
) {
    Mesh3DBody::makeSimpleRigidBody(mass, getPosition(), rotation, size, world, collisionGroup, collisionMask);

    direction = direction.getScaled(forceImpulse);

    btVector3 impulse;
    direction.saveToBtVector3(&impulse);
    getRigidBody()->applyCentralImpulse(impulse);

    setDirection(direction);
}

void Projectile3DBody::onUpdate()
{
    Mesh3D::onUpdate();

    if (isRemoved()) {
        return;
    }

    /*if (this->getTTL() != 0) {
        if (this->getTimeToLive().isFinished()) {
            this->getTimeToLive().setEnabled(true);
            ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeCollisionObject(getRigidBody());
            setRemoved(true);
        }

        this->getTimeToLive().update();
    }*/
}

void Projectile3DBody::resolveCollision(Collisionable *objectWithCollision) {
    Mesh3DBody::resolveCollision(objectWithCollision);
}
