#include "../../include/Physics/ProjectileRay.h"
#include "../../darkheaz/src/weapons/AmmoProjectileBody.h"
#include "../../darkheaz/src/items/ItemHealthGhost.h"
#include "../../darkheaz/src/items/ItemWeaponGhost.h"
#include "../../include/ComponentsManager.h"

ProjectileRay::ProjectileRay(
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
    Projectile(direction),
    AmmoProjectile(color, damage),
    speed(speed),
    indestructible(indestructible)
{
}

void ProjectileRay::onUpdate()
{
    RayCollisionable::onUpdate();

    if (!isEnabled()) return;

    if (!indestructible && !ComponentsManager::get()->getComponentCamera()->getCamera()->getFrustum()->isVertexInside(getPosition())) {
        this->setRemoved(true);
        return;
    }

    addToPosition(getDirection().getScaled((float) speed));
}

void ProjectileRay::resolveCollision(Collisionable *objectWithCollision)
{
    RayCollisionable::resolveCollision(objectWithCollision);

    auto projectile = dynamic_cast<AmmoProjectileBody*> (objectWithCollision);
    if (projectile != nullptr) {
        return;
    }

    auto health = dynamic_cast<ItemHealthGhost*> (objectWithCollision);
    if (health != nullptr) {
        return;
    }

    auto weapon = dynamic_cast<ItemWeaponGhost*> (objectWithCollision);
    if (weapon != nullptr) {
        return;
    }

    auto object = dynamic_cast<Object3D*> (objectWithCollision);
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

void ProjectileRay::integrate()
{
    RayCollisionable::integrate();
}

