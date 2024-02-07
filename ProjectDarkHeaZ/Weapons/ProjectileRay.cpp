#include "ProjectileRay.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ProjectileRay::ProjectileRay(
    Object3D *parent,
    Vertex3D position,
    float damage,
    const Vertex3D &direction,
    float size,
    int filterGroup,
    int filterMask,
    float speed,
    const Color &color,
    float intensity,
    bool indestructible
) :
    Projectile3DBody(direction),
    AmmoProjectile(parent, color, damage, intensity),
    speed(speed),
    size(size),
    indestructible(indestructible)
{
    setParent(parent);
    setPosition(position);
    setTransparent(true);

    makeProjectileRigidBody(
        0.1,
        Vertex3D(0.5, 0.5, 0.5),
        direction,
        rotation,
        speed,
        90,
        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
        filterGroup,
        filterMask
    );
}

void ProjectileRay::onUpdate()
{
    if (!isEnabled()) return;

    if (!indestructible && !Frustum::isVertexInside(getPosition())) {
        this->setRemoved(true);
        return;
    }

    addToPosition(getDirection().getScaled((float) speed));
}

void ProjectileRay::resolveCollision(Collisionable *objectWithCollision)
{
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

void ProjectileRay::setTarget(Object3D *target) {
    ProjectileRay::target = target;
    if (target != nullptr) {
        hadTarget = true;
    }
}

Object3D *ProjectileRay::getTarget() const {
    return target;
}

bool ProjectileRay::isHadTarget() const {
    return hadTarget;
}

Vertex3D ProjectileRay::getRay()
{
    return getPosition() + getDirection().getNormalize().getScaled(size);
}