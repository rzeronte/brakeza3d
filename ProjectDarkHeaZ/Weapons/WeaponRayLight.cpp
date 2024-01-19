//
// Created by edu on 19/01/24.
//

#include "WeaponRayLight.h"
#include "../Player.h"
#include "../../include/ComponentsManager.h"

WeaponRayLight::WeaponRayLight(const WeaponAttributes &attributes) : Weapon(attributes)
{}

void WeaponRayLight::onUpdate()
{
    Weapon::onUpdate();

    if (status == WeaponStatus::RELEASED) {
        rayLight->setReach(0);
        rayLight->updateDirection(parent->AxisDown().getNormalize(),parent->AxisUp().getScaled(-1.3f));
        rayLight->update(false);
    } else {
        rayLight->updateDirection(parent->AxisDown().getNormalize(),parent->AxisUp().getScaled(-1.3f));
        rayLight->update(true);
    }
}

bool WeaponRayLight::shootRayLight(float intensity, Color color) {
    if (getAmmoAmount() <= 0) return false;

    if (isStop() && counterStopDuration.isEnabled()) {
        rayLight->setReach(0);
        return false;
    }

    setAmmoAmount(ammoAmount - 1);

    rayLight->setColor(color);
    rayLight->setDamage(getDamage());
    rayLight->setIntensity(intensity);

    if (getStatus() == PRESSED) {
        ComponentsManager::get()->getComponentSound()->sound("projectileRaylight",
                                                             EngineSetup::SoundChannels::SND_GLOBAL, 0);
    }

    if (counterCadence->isFinished()) {
        counterCadence->setEnabled(true);
        setStatus(WeaponStatus::PRESSED);

        ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increase(WeaponTypes::WEAPON_RAYLIGHT);
        return true;
    }

    return false;
}

bool WeaponRayLight::shoot(WeaponShootAttributes attributes)
{
    return shootRayLight(0.00075f, PaletteColors::getPlayerRayLight());
}
