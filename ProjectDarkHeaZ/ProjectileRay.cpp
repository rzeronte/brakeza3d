#include "ProjectileRay.h"
#include "../include/ComponentsManager.h"

ProjectileRay::ProjectileRay(
    Object3D *parent,
    Vertex3D position,
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
    AmmoProjectile(this, color, damage),
    speed(speed),
    indestructible(indestructible)
{
    setParent(parent);
    setPosition(position);
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

    Tools::makeExplosion(this, getPosition(), 1, OCParticlesContext::forProjectile(), Color::white(), Color::yellow());
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
