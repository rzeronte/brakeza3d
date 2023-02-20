#include "../../include/Physics/ProjectileRay.h"
#include "../../darkheaz/src/weapons/AmmoProjectileBody.h"
#include "../../darkheaz/src/items/ItemHealthGhost.h"
#include "../../darkheaz/src/items/ItemWeaponGhost.h"


ProjectileRay::ProjectileRay(
    float ttl,
    float damage,
    const Vertex3D &direction,
    const Vertex3D &ray,
    int filterGroup,
    int filterMask,
    int speed,
    const Color &color,
    bool indestructible
) :
    RayCollisionable(ray, filterGroup, filterMask),
    Projectile(ttl, direction),
    AmmoProjectile(color, damage),
    speed(speed),
    indestructible(indestructible)
{
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

    auto projectile = dynamic_cast<AmmoProjectileBody*> (collisionable);
    if (projectile != nullptr) {
        return;
    }

    auto health = dynamic_cast<ItemHealthGhost*> (collisionable);
    if (health != nullptr) {
        return;
    }

    auto weapon = dynamic_cast<ItemWeaponGhost*> (collisionable);
    if (weapon != nullptr) {
        return;
    }

    auto object = dynamic_cast<Object3D*> (collisionable);
    if (object != nullptr) {
        if (object == getParent()) {
            return;
        }
    }

    if (!indestructible) {
        this->setRemoved(true);
    }
}

int ProjectileRay::getSpeed() const {
    return speed;
}

void ProjectileRay::setSpeed(int value) {
    ProjectileRay::speed = value;
}

void ProjectileRay::integrate() {
    RayCollisionable::integrate();
}

void ProjectileRay::hasHit()
{
}

