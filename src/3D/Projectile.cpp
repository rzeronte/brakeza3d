
#include "../../include/3D/Projectile.h"

#include "../../include/Misc/Logging.h"


Projectile::Projectile(const Vertex3D &direction)
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
    direction = value;
}

void Projectile::makeProjectileRigidBody(
    float mass,
    Vertex3D dir,
    float impulse,
    float accuracy,
    btDiscreteDynamicsWorld *world,
    CollisionShape shape,
    int collisionGroup,
    int collisionMask
)
{
    if (shape == SIMPLE_SHAPE) {
        Mesh3D::MakeSimpleRigidBody(mass, world, collisionGroup, collisionMask);
    } else if (shape == TRIANGLE_MESH_SHAPE) {
        makeRigidBodyFromTriangleMeshFromConvexHull(mass, world, collisionGroup, collisionMask);
    } else {
        LOG_ERROR("[Projectile] Projectile cannot be capsule shape!");
    }

    dir = dir.getScaled(impulse);
    dir.x += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy)) / 100;
    dir.y += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy)) / 100;

    getRigidBody()->applyCentralImpulse(dir.toBullet());

    setDirection(dir);
}

void Projectile::onUpdate()
{
    Mesh3D::onUpdate();
}

void Projectile::ResolveCollision(CollisionInfo objectWithCollision)
{
    Collider::ResolveCollision(objectWithCollision);
}
