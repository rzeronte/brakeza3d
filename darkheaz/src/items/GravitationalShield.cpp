//
// Created by eduardo on 3/04/22.
//

#include "GravitationalShield.h"
#include "../weapons/AmmoProjectileBody.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

GravitationalShield::GravitationalShield(
        float force,
        float factor,
        float stamina,
        float ttl
) : GravitationalGhost(force, factor), stamina(stamina), ttl(ttl)
{
    this->startStamina = stamina;
    timeToLive.setStep(ttl);
    timeToLive.setEnabled(true);

    blink = new ShaderBlink(this, Color::green());
    blink->setStep(0.05);
    blink->setEnabled(false);

    counterDamageBlink = new Counter(1);
    counterDamageBlink->setEnabled(false);

    shockWave = new ShaderShockWave(60, 0.55f, 1);

    makeSimpleGhostBody(
        Vertex3D(600, 600, 600),
        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Player,
        EngineSetup::collisionGroups::Enemy | EngineSetup::collisionGroups::Projectile
    );

    removeCollisionObject();
    setHidden(true);
}

void GravitationalShield::onUpdate()
{
    if (!isEnabled()) return;

    GravitationalGhost::onUpdate();

    timeToLive.update();

    if (timeToLive.isFinished()) {
        setEnabled(false);
        removeCollisionObject();
        setHidden(true);
    }
}


void GravitationalShield::postUpdate()
{
    Object3D::postUpdate();

    shockWave->onUpdate(getPosition());

    if (counterDamageBlink->isEnabled()) {
        counterDamageBlink->update();
        blink->update();
        if (counterDamageBlink->isFinished()) {
            blink->setEnabled(false);
        }
    }
}

float GravitationalShield::getStartStamina() const {
    return startStamina;
}

void GravitationalShield::setStartStamina(float startStamina) {
    GravitationalShield::startStamina = startStamina;
}

float GravitationalShield::getStamina() const {
    return stamina;
}

void GravitationalShield::setStamina(float stamina) {
    GravitationalShield::stamina = stamina;
}

void GravitationalShield::takeDamage(float damageTaken)
{
    this->stamina -= damageTaken;

    if (this->stamina <= 0) {
        setEnabled(false);
        removeCollisionObject();
        setHidden(true);
    }
}

void GravitationalShield::resolveCollision(Collisionable *collisionable)
{
    Mesh3DGhost::resolveCollision(collisionable);
    auto *projectile = dynamic_cast<AmmoProjectileBody*> (collisionable);
    if (projectile != nullptr) {
        blink->setEnabled(true);
        counterDamageBlink->setEnabled(true);

        auto fireworks = new ParticleEmissorFireworks(true, 1000, 1, 0.02, Color::green(), 1, 4);
        fireworks->setPosition(projectile->getPosition());
        fireworks->setRotationFrame(0, 4, 5);
        Brakeza3D::get()->addObject3D(fireworks, ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());


        takeDamage(projectile->getWeaponType()->getDamage());
    }
}

GravitationalShield::~GravitationalShield()
{
    delete blink;
    delete shockWave;
    delete counterDamageBlink;
}

void GravitationalShield::reset()
{
    setEnabled(true);
    setHidden(false);
    timeToLive.setEnabled(true);
    setStamina(getStartStamina());

    if (!isHidden()) {
        removeCollisionObject();
    }

    Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld()->addCollisionObject(
        ghostObject,
        EngineSetup::collisionGroups::Player,
        EngineSetup::collisionGroups::Enemy | EngineSetup::collisionGroups::Projectile
    );
}

bool GravitationalShield::isHidden() const
{
    return hidden;
}

void GravitationalShield::setHidden(bool hidden)
{
    GravitationalShield::hidden = hidden;
}

void GravitationalShield::loadBlinkShader()
{
    blink = new ShaderBlink(this, Color::red());
    blink->setStep(0.05);
    blink->setEnabled(true);
}
