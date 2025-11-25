
#include "../../include/3D/Projectile.h"


Projectile::Projectile(const Vertex3D direction)
:
    direction(direction)
{
    setCollisionsEnabled(true);
}

const Vertex3D &Projectile::getDirection() const
{
    return direction;
}

void Projectile::setDirection(const Vertex3D &value)
{
    Projectile::direction = value;
}

void Projectile::makeProjectileRigidBody(
    float mass,
    Vertex3D direction,
    float forceImpulse,
    float accuracy,
    btDiscreteDynamicsWorld *world,
    int collisionGroup,
    int collisionMask
)
{
    if (BrakezaSetup::get()->PROJECTILE_SIMPLE_MESH) {
        Mesh3D::makeSimpleRigidBody(mass, world, collisionGroup, collisionMask);
    } else {
        Mesh3D::makeRigidBodyFromTriangleMeshFromConvexHull(mass, world, collisionGroup, collisionMask);
    }
    //Mesh3D::makeSimpleRigidBody(mass, world, collisionGroup, collisionMask);
    //Mesh3D::makeRigidBodyFromTriangleMeshFromConvexHull(mass, world, collisionGroup, collisionMask);

    direction = direction.getScaled(forceImpulse);
    direction.x += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy)) / 100;
    direction.y += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy)) / 100;

    getRigidBody()->applyCentralImpulse(direction.toBullet());

    setDirection(direction);
}

void Projectile::onUpdate()
{
    Mesh3D::onUpdate();
}

void Projectile::resolveCollision(CollisionInfo objectWithCollision)
{
    Collider::resolveCollision(objectWithCollision);
}
