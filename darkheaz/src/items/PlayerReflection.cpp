#include "PlayerReflection.h"
#include "../weapons/AmmoProjectileBody.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

PlayerReflection::PlayerReflection(float ttl)
:
    hidden(true),
    timeToLive(Counter(ttl))
{
    timeToLive.setEnabled(true);
    zombie = new ShaderZombie(true, EngineSetup::get()->IMAGES_FOLDER + "alien.png", this, this->getOpenClRenderer());
}

void PlayerReflection::onStartSetup()
{
    makeSimpleGhostBody(
        Vertex3D(600, 600, 600),
        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::Player,
        EngineSetup::collisionGroups::Enemy
    );

    removeCollisionObject();
}

void PlayerReflection::onUpdate()
{
    if (!isEnabled()) return;

    Mesh3DGhost::onUpdate();

    timeToLive.update();

    zombie->update();
}

void PlayerReflection::postUpdate()
{
    Mesh3D::postUpdate();

    if (timeToLive.isFinished()) {
        Tools::makeExplosion(this, getPosition(), 5, OCParticlesContext::forExplosion(), Color::white(), Color::yellow());
        setStencilBufferEnabled(false);
        setEnabled(false);
        removeCollisionObject();
        setHidden(true);
    }
}

void PlayerReflection::resolveCollision(Collisionable *objectWithCollision)
{
    Mesh3DGhost::resolveCollision(objectWithCollision);
}

void PlayerReflection::reset()
{
    setEnabled(true);
    setHidden(false);
    timeToLive.setEnabled(true);
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
