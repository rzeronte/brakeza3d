#include "PlayerReflection.h"
#include "../../include/Brakeza3D.h"

PlayerReflection::PlayerReflection(float ttl)
:
    timeToLive(Counter(ttl)),
    blink(new ShaderBlink(false, this, 0.05, Color(255, 102, 0)))
{
    timeToLive.setEnabled(true);
}

void PlayerReflection::onUpdate()
{
    Mesh3D::onUpdate();

    if (!isEnabled()) return;

    timeToLive.update();

    counterDamageBlink.update();

    if (timeToLive.currentPercentage() > 75) {
        counterDamageBlink.setEnabled(true);
        blink->setEnabled(true);
    }

    blink->update();
    if (timeToLive.isFinished()) {
        setRemoved(true);
        setEnabled(false);
        Tools::makeExplosion(this, getPosition(), 5, OCParticlesContext::forExplosion(), Color::white(), Color::yellow());
        Brakeza3D::get()->addObject3D(new ShockWave(getPosition(), 0.50, 50, 1, true), Brakeza3D::uniqueObjectLabel("shockWave"));
    }
}

void PlayerReflection::postUpdate()
{
    Mesh3D::postUpdate();
}

PlayerReflection::~PlayerReflection()
{
}
