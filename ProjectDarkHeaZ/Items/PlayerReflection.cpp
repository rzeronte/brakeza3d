#include "PlayerReflection.h"
#include "../../include/Brakeza3D.h"
#include "../Common/ShockWave.h"

PlayerReflection::PlayerReflection(float ttl)
:
    timeToLive(Counter(ttl)),
    blink(new FXBlink(false, this, 0.05, PaletteColors::getPlayerDamageBlink()))
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
        Brakeza3D::get()->addObject3D(new ShockWave(getPosition(), 0.50, 1, ShockWaveParams::standard(), true), Brakeza3D::uniqueObjectLabel("shockWave"));
    }
}

void PlayerReflection::postUpdate()
{
    Mesh3D::postUpdate();
}

PlayerReflection::~PlayerReflection()
{
    if (swarmObject != nullptr) {
        ComponentsManager::get()->getComponentGame()->getSwarm()->removeBoid(swarmObject);
    }
}

void PlayerReflection::setSwarmObject(SwarmObject *swarmObject)
{
    PlayerReflection::swarmObject = swarmObject;
}
