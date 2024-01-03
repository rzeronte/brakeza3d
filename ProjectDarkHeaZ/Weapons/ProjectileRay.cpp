#include "ProjectileRay.h"
#include "../../include/ComponentsManager.h"

ProjectileRay::ProjectileRay(
    Object3D *parent,
    Vertex3D position,
    float damage,
    const Vertex3D &direction,
    const Vertex3D &ray,
    int filterGroup,
    int filterMask,
    float speed,
    const Color &color,
    float intensity,
    bool indestructible
) :
    RayCollisionable(ray, filterGroup, filterMask),
    Projectile(direction),
    AmmoProjectile(this, color, damage, intensity),
    speed(speed),
    indestructible(indestructible)
{
    setParent(parent);
    setPosition(position);
    setTransparent(true);
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

    Tools::makeFadeInSprite(getPosition() + Vertex3D(0, 0, -2), ComponentsManager::get()->getComponentGame()->getSpriteSparklesGreen()->getAnimation());
}

float ProjectileRay::getSpeed() const {
    return speed;
}

void ProjectileRay::setSpeed(float value) {
    ProjectileRay::speed = value;
}

void ProjectileRay::integrate()
{
    RayCollisionable::integrate();
}

