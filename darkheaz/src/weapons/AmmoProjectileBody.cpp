
#include "AmmoProjectileBody.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

AmmoProjectileBody::AmmoProjectileBody(
        Weapon *weaponType,
        float damage,
        float ttl,
        const Vertex3D &direction) : Projectile3DBody(ttl, direction), AmmoProjectile(weaponType->getModelProjectile()->getFlatColor(), damage), weaponType(weaponType)
{
    particleEmissor = new ParticleEmissor(this, 5, 1000, 1, 0.0005, weaponType->getModelProjectile()->getFlatColor());
    particleEmissor->setRotationFrame(0, 25, 33);

    Brakeza3D::get()->addObject3D(particleEmissor, "emissor" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
}

Weapon *AmmoProjectileBody::getWeaponType() const {
    return weaponType;
}

void AmmoProjectileBody::resolveCollision(Collisionable *collisionable)
{
    auto projectile = dynamic_cast<AmmoProjectileBody*> (collisionable);
    if (projectile != nullptr) {
        return;
    }

    auto health = dynamic_cast<ItemHealthGhost*> (collisionable);
    if (health != nullptr) {
        return;
    }

    auto ray = dynamic_cast<ProjectileRay*> (collisionable);
    if (ray != nullptr) {
        return;
    }

    auto object = dynamic_cast<Object3D*> (collisionable);
    if (object != nullptr) {
        if (object == getParent()) {
            return;
        }
    }

    this->remove();
    getParticleEmissor()->setActive(false);
}

void AmmoProjectileBody::onUpdate()
{
    Projectile3DBody::onUpdate();

    particleEmissor->setPosition(getPosition());
    updateBoundingBox();

    if (!ComponentsManager::get()->getComponentCamera()->getCamera()->frustum->isAABBInFrustum(&this->aabb)) {
        this->remove();
        particleEmissor->setActiveAdding(false);
    }
}

ParticleEmissor *AmmoProjectileBody::getParticleEmissor() {
    return particleEmissor;
}
