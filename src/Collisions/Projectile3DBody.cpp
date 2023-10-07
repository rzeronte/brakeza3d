
#include "../../include/Physics/Projectile3DBody.h"

Projectile3DBody::Projectile3DBody(
    const Vertex3D &direction
) : Projectile(direction)
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
    direction.x += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy));
    direction.y += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy));

    btVector3 impulse;
    direction.saveToBtVector3(&impulse);
    getRigidBody()->applyCentralImpulse(impulse);

    setDirection(direction);
}

void Projectile3DBody::onUpdate()
{
    Mesh3D::onUpdate();
}

void Projectile3DBody::resolveCollision(Collisionable *objectWithCollision) {
    Mesh3DBody::resolveCollision(objectWithCollision);
}
