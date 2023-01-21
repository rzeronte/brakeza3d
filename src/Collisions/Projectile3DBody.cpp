
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
        Vertex3D projectileDirection,
        M3 rotation,
        float forceImpulse,
        float accuracy,
        btDiscreteDynamicsWorld *world,
        int collisionGroup,
        int collisionMask
) {
    updateBoundingBox();
    Mesh3DBody::makeSimpleRigidBody(mass, getPosition(), projectileDirection, rotation, aabb.size(), world, collisionGroup, collisionMask);

    projectileDirection = projectileDirection.getScaled(forceImpulse);
    projectileDirection.x += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy));
    projectileDirection.y += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy));
    projectileDirection.z += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy));

    btVector3 impulse;
    projectileDirection.saveToBtVector3(&impulse);
    getRigidBody()->applyCentralImpulse(impulse);
    getRigidBody()->setGravity(btVector3(0, 0, 0));

    this->direction = projectileDirection;
}

void Projectile3DBody::onUpdate() {
    Mesh3D::onUpdate();

    if (isRemoved()) {
        return;
    }

    if (this->getTTL() != 0) {
        if (this->getTimeToLive()->isFinished()) {
            this->getTimeToLive()->setEnabled(true);
            ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeCollisionObject(getRigidBody());
            this->removed = true;
        }

        this->getTimeToLive()->update();
    }
}

void Projectile3DBody::resolveCollision(Collisionable *collisionable) {
    Mesh3DBody::resolveCollision(collisionable);
}
