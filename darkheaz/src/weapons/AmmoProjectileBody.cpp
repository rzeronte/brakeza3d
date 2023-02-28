
#include "AmmoProjectileBody.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

AmmoProjectileBody::AmmoProjectileBody(
    Vertex3D position,
    Object3D *parent,
    Weapon *weaponType,
    M3 rotation,
    const Vertex3D &sizeCollision,
    const Vertex3D &direction,
    float damage,
    float speed,
    float accuracy,
    float ttl,
    int collisionGroup,
    int collisionMask,
    ParticleEmissor *particleEmissor
) :
    particleEmissor(particleEmissor),
    Projectile3DBody(ttl, direction),
    AmmoProjectile(weaponType->getModelProjectile()->getFlatColor(), damage),
    weaponType(weaponType)
{
    setPosition(position);
    setParent(parent);
    setRender(false);
    setStencilBufferEnabled(true);

    makeProjectileRigidBody(
        0.1,
        sizeCollision,
        direction,
        rotation,
        speed,
        accuracy,
        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
        collisionGroup,
        collisionMask
    );
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

    if (particleEmissor != nullptr) {
        particleEmissor->setActive(false);
    }
}

void AmmoProjectileBody::onUpdate()
{
    Projectile3DBody::onUpdate();

    if (particleEmissor != nullptr) {
        particleEmissor->setPosition(getPosition());
    }

    if (!ComponentsManager::get()->getComponentCamera()->getCamera()->getFrustum()->isVertexInside(getPosition())) {
        this->remove();
        if (particleEmissor != nullptr) {
            particleEmissor->setActiveAdding(false);
        }
    }
}

void AmmoProjectileBody::setParticleEmissor(ParticleEmissor *particleEmissor) {
    AmmoProjectileBody::particleEmissor = particleEmissor;
}
