#include "../../include/Physics/ProjectileRay.h"


ProjectileRay::ProjectileRay(
    float ttl,
    const Vertex3D &direction,
    const Vertex3D &ray,
    float speed,
    const Color &color
) : Projectile(ttl, direction), RayCollisionable(ray), color(color), speed(speed) {

}

void ProjectileRay::onUpdate()
{
    RayCollisionable::onUpdate();

    moveDirection();
}

void ProjectileRay::moveDirection()
{
    const Vertex3D to = getPosition() + direction.getScaled(speed);

    setPosition(to);
}

void ProjectileRay::resolveCollision(Collisionable *collisionable)
{
    RayCollisionable::resolveCollision(collisionable);
    this->setRemoved(true);
}

float ProjectileRay::getSpeed() const {
    return speed;
}

void ProjectileRay::setSpeed(float speed) {
    ProjectileRay::speed = speed;
}
