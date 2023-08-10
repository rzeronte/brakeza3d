#include "PlayerReflection.h"
#include "../../include/Brakeza3D.h"

PlayerReflection::PlayerReflection(float ttl)
:
    timeToLive(Counter(ttl)),
    blink(new ShaderBlink(false, this, 0.05, ComponentsManager::get()->getComponentGame()->getPalette().getBlinkPlayer()))
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
        auto palette = ComponentsManager::get()->getComponentGame()->getPalette();
        Tools::makeExplosion(this, getPosition(), 5, OCParticlesContext::forExplosion(), palette.getExplosionEnemyFrom(), palette.getExplosionEnemyTo());
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
