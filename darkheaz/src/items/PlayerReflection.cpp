#include "PlayerReflection.h"
#include "../weapons/AmmoProjectileBody.h"

PlayerReflection::PlayerReflection(float ttl)
:
    timeToLive(Counter(ttl))
{
    timeToLive.setEnabled(true);
    blink = new ShaderBlink(false, this, 0.05, Color::red());
}

void PlayerReflection::onUpdate()
{
    if (!isEnabled()) return;

    Mesh3DGhost::onUpdate();

    timeToLive.update();

    if (timeToLive.isFinished()) {
        Tools::makeExplosion(this, getPosition(), 5, OCParticlesContext::forExplosion(), Color::white(), Color::yellow());
        removeCollisionObject();
        setRemoved(true);
        return;
    }

    blink->update();

    if (timeToLive.currentPercentage() > 75) {
        counterDamageBlink.setEnabled(true);
        blink->setEnabled(true);
    }
}

void PlayerReflection::postUpdate()
{
    Mesh3D::postUpdate();
}

void PlayerReflection::resolveCollision(Collisionable *objectWithCollision)
{
    Mesh3DGhost::resolveCollision(objectWithCollision);
}

PlayerReflection::~PlayerReflection()
{
}
