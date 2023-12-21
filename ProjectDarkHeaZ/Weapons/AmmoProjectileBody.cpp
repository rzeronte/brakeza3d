
#include "AmmoProjectileBody.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

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
    Color color,
    float intensity,
    int collisionGroup,
    int collisionMask,
    ParticleEmitter *particleEmitter
)
:
    Projectile3DBody(direction),
    AmmoProjectile(parent, color, damage, intensity),
    weaponType(weaponType),
    particleEmitter(particleEmitter),
    wasCollision(false),
    ending(Counter(5))
{
    setPosition(position);
    setParent(parent);
    setRender(true);
    setStencilBufferEnabled(false);

    ending.setEnabled(false);

    clone(weaponType->getModelProjectile());

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
    Color from = PaletteColors::getExplosionEnemyFrom();
    Color to = PaletteColors::getExplosionEnemyTo();

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

    auto human = dynamic_cast<ItemHumanGhost*> (collisionable);
    if (human != nullptr) {
        from = Color::red();
        to = Color::red();
    }

    auto object = dynamic_cast<Object3D*> (collisionable);
    if (object != nullptr) {
        if (object == getParent()) {
            return;
        }
    }

    particleEmitter->setStopAdd(true);
    startEndingCounter();

    Tools::makeExplosion(this, getPosition(), 0.75f, OCParticlesContext::forProjectile(), from, to);
    Tools::makeFadeInSprite(getPosition(), ComponentsManager::get()->getComponentGame()->getSpriteSparklesRed()->getAnimation());
}

void AmmoProjectileBody::onUpdate()
{
    Projectile3DBody::onUpdate();

    if (isRemoved()) return;

    ending.update();

    if (ending.isEnabled() && ending.isFinished()) {
        setRemoved(true);
        return;
    }

    if (!ComponentsManager::get()->getComponentCamera()->getCamera()->getFrustum()->isVertexInside(getPosition()) && !isWasCollision()) {
        startEndingCounter();
    }

    particleEmitter->setPosition(getPosition());
    particleEmitter->setRotation(getRotation());

    particleEmitter->onUpdate();
    particleEmitter->drawOnUpdateSecondPass();

}

void AmmoProjectileBody::drawOnUpdateSecondPass()
{
    Object3D::drawOnUpdateSecondPass();
}

void AmmoProjectileBody::startEndingCounter()
{
    wasCollision = true;
    removeCollisionObject();
    ending.setEnabled(true);
}

bool AmmoProjectileBody::isWasCollision() const
{
    return wasCollision;
}

AmmoProjectileBody::~AmmoProjectileBody()
{
    delete particleEmitter;
}
