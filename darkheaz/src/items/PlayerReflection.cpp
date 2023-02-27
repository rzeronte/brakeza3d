#include "PlayerReflection.h"
#include "../weapons/AmmoProjectileBody.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

PlayerReflection::PlayerReflection(
    float stamina,
    float ttl
):
    startStamina(stamina),
    counterDamageBlink(Counter(1))
{
    timeToLive.setStep(ttl);
    timeToLive.setEnabled(true);

    counterDamageBlink.setEnabled(false);

}

void PlayerReflection::onUpdate()
{
    if (!isEnabled()) return;

    Mesh3DGhost::onUpdate();

    timeToLive.update();
}


void PlayerReflection::postUpdate()
{
    Object3D::postUpdate();

    if (counterDamageBlink.isEnabled()) {
        counterDamageBlink.update();
        blink->update();
        if (counterDamageBlink.isFinished()) {
            blink->setEnabled(false);
        }
    }
    if (timeToLive.isFinished()) {
        makeExplosion();
        setStencilBufferEnabled(false);
        setEnabled(false);
        removeCollisionObject();
        setHidden(true);
    }
}

float PlayerReflection::getStartStamina() const {
    return startStamina;
}

void PlayerReflection::setStartStamina(float value) {
    PlayerReflection::startStamina = value;
}

float PlayerReflection::getStamina() const {
    return stamina;
}

void PlayerReflection::setStamina(float value) {
    PlayerReflection::stamina = value;
}

void PlayerReflection::takeDamage(float damageTaken)
{
    this->stamina -= damageTaken;

    if (this->stamina <= 0) {
        makeExplosion();
        setEnabled(false);
        removeCollisionObject();
        setHidden(true);
        setStencilBufferEnabled(false);
    }
}

void PlayerReflection::resolveCollision(Collisionable *collisionable)
{
    Mesh3DGhost::resolveCollision(collisionable);
    auto *projectile = dynamic_cast<AmmoProjectileBody*> (collisionable);
    if (projectile != nullptr) {
        blink->setEnabled(true);
        counterDamageBlink.setEnabled(true);

        auto fireworks = new ParticleEmissorFireworks(getPosition(), 5, 1000, 1, 0.02, Color::green(), 1, 4);
        Brakeza3D::get()->addObject3D(fireworks, ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());


        takeDamage(projectile->getWeaponType()->getDamage());
    }
}


void PlayerReflection::reset()
{
    setEnabled(true);
    setHidden(false);
    timeToLive.setEnabled(true);
    setStamina(getStartStamina());
    setStencilBufferEnabled(true);

    if (!isHidden()) {
        removeCollisionObject();
    }

    Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld()->addCollisionObject(
        ghostObject,
        EngineSetup::collisionGroups::Player,
        EngineSetup::collisionGroups::Enemy | EngineSetup::collisionGroups::Projectile
    );
}

bool PlayerReflection::isHidden() const
{
    return hidden;
}

void PlayerReflection::setHidden(bool value)
{
    PlayerReflection::hidden = value;
}

void PlayerReflection::loadBlinkShader()
{
    blink = new ShaderBlink(this, Color::red());
    blink->setStep(0.05);
    blink->setEnabled(true);

    makeSimpleGhostBody(
        Vertex3D(600, 600, 600),
        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Player,
        EngineSetup::collisionGroups::Enemy | EngineSetup::collisionGroups::Projectile
    );
    removeCollisionObject();
    setHidden(true);
}

void PlayerReflection::makeExplosion()
{
    auto sprite = new Sprite3D();

    Vertex3D origin = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition();

    Vector3D direction(origin, getPosition());
    sprite->setPosition(origin + direction.getComponent().getNormalize().getScaled(150));

    sprite->linkTextureAnimation(ComponentsManager::get()->getComponentGame()->getExplosionSpriteTemplate());
    sprite->setAnimation(0);
    sprite->setAutoRemoveAfterAnimation(true);

    Brakeza3D::get()->addObject3D(sprite, "enemy_explosion_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
}
