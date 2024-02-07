//
// Created by edu on 19/01/24.
//

#include "WeaponRayLight.h"
#include "../Player.h"
#include "../../include/ComponentsManager.h"

WeaponRayLight::WeaponRayLight(const WeaponAttributes &attributes)
:
        isPlayer(true),
        Weapon(attributes)
{
    particles = new ParticleEmitter(
        ParticleEmitterState::DEFAULT,
        nullptr,
        parent->getPosition(),
        9999,
        Color::red(),
        Color::yellow(),
        OCParticlesContext::forPlayerEngine(),
        ComponentsManager::get()->getComponentGame()->getImages()->getTextureByLabel("particle02")
    );
}

void WeaponRayLight::onUpdate()
{
    Weapon::onUpdate();

    particles->onUpdate();

    if (!isEnabled()) return;

    rayLight->updateDirection(parent->AxisDown().getNormalize(),parent->AxisUp().getScaled(-1.3f));

    if (isPlayer) {
        if (status == WeaponStatus::RELEASED) {
            rayLight->setReach(0);
            rayLight->update(false);
        }

        if (status == WeaponStatus::PRESSED || status == WeaponStatus::SUSTAINED) {
            rayLight->update(true);
        }
    } else {
        auto gameState = ComponentsManager::get()->getComponentGame()->getGameState();

        if (isStop() && counterStopDuration.isEnabled() || gameState != EngineSetup::GAMING) {
            rayLight->setReach(0);
            rayLight->update(false);
        } else {
            rayLight->update(true);
        }
    }

    auto result = rayLight->getResult();
    if (result.wasHit) {
        particles->setPosition(result.position);
    } else {
        particles->setPosition(result.position);

    }
}

bool WeaponRayLight::shootRayLight(float intensity, Color color)
{
    if (getAmmoAmount() <= 0) return false;

    if (isStop() && counterStopDuration.isEnabled()) {
        rayLight->setReach(0);
        return false;
    }

    setAmmoAmount(ammoAmount - 1);

    rayLight->setColor(color);
    rayLight->setDamage(getDamage());
    rayLight->setIntensity(intensity);

    if (counterCadence->isFinished()) {
        counterCadence->setEnabled(true);

        ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increase(WeaponTypes::WEAPON_RAYLIGHT);
        return true;
    }

    return false;
}

bool WeaponRayLight::shoot(WeaponShootAttributes attributes)
{
    rayLight->setCollisionMask(attributes.filterGroup, attributes.filterMask);
    return shootRayLight(0.00025f, PaletteColors::getPlayerRayLight());
}

void WeaponRayLight::setIsForPlayer(bool avoidCheckWeaponStatus) {
    WeaponRayLight::isPlayer = avoidCheckWeaponStatus;
}

void WeaponRayLight::setEnabled(bool value) {
    Weapon::setEnabled(value);
    particles->setStopAdd(!value);
}
