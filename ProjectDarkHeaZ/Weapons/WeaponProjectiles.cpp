//
// Created by edu on 19/01/24.
//

#include "WeaponProjectiles.h"
#include "../../include/Brakeza3D.h"
WeaponProjectiles::WeaponProjectiles(const WeaponAttributes &attributes) : Weapon(attributes) {

}

bool WeaponProjectiles::shootProjectile(
    Object3D *parent,
    Vertex3D position,
    Vertex3D offsetPosition,
    Vertex3D direction,
    M3 rotation,
    Color color,
    float intensity,
    int filterGroup,
    int filterMask,
    bool sound,
    bool allowMirror,
    Color particlesFrom,
    Color particlesTo
) {
    if (getAmmoAmount() <= 0) return false;

    if (isStop() && counterStopDuration.isEnabled() ) {
        return false;
    }

    auto storeManager = ComponentsManager::get()->getComponentGame()->getStoreManager();

    if (counterCadence->isFinished()) {
        float t = cadenceTime;

        if (storeManager->isItemEnabled(EngineSetup::StoreItems::ITEM_FAST_SHOOT_CADENCE)){
            t = t - (t * 0.5f);
        }

        this->counterCadence->setStep(t);
        this->counterCadence->setEnabled(true);

        setAmmoAmount(getAmmoAmount() - 1);

        if (sound) {
            ComponentsManager::get()->getComponentSound()->sound("projectileStandard", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }

        ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increase(WeaponTypes::WEAPON_PROJECTILE);

        Brakeza3D::get()->addObject3D(new AmmoProjectileBody(
            position - offsetPosition,
            parent,
            this,
            rotation,
            Vertex3D(0.15, 0.15, 0.15),
            direction,
            getDamage(),
            (float) getSpeed(),
            getAccuracy(),
            color,
            intensity,
            filterGroup,
            filterMask,
            new ParticleEmitter(
                ParticleEmitterState::DEFAULT,
                nullptr,
                position,
                0,
                particlesFrom,
                particlesTo,
                OCParticlesContext::forProjectile(),
                ComponentsManager::get()->getComponentGame()->getImages()->getTextureByLabel("particle01")
            )
        ), Brakeza3D::uniqueObjectLabel("weaponProjectile"));

        if (storeManager->isItemEnabled(EngineSetup::StoreItems::ITEM_MIRROR_SHOOT) && allowMirror) {
            Brakeza3D::get()->addObject3D(new AmmoProjectileBody(
                    position + offsetPosition,
                    parent,
                    this,
                    rotation,
                    Vertex3D(50, 50, 50),
                    direction.getInverse(),
                    getDamage(),
                    (float) getSpeed(),
                    getAccuracy(),
                    color,
                    intensity,
                    filterGroup,
                    filterMask,
                    new ParticleEmitter(
                        ParticleEmitterState::DEFAULT,
                        nullptr,
                        position,
                        0,
                        particlesFrom,
                        particlesTo,
                        OCParticlesContext::forProjectile(),
                        ComponentsManager::get()->getComponentGame()->getImages()->getTextureByLabel("particle01")
                    )
            ), Brakeza3D::uniqueObjectLabel("weaponProjectile"));
        }

        return true;
    }

    return false;
}

bool WeaponProjectiles::shoot(WeaponShootAttributes attributes)
{
    return shootProjectile(
        parent,
        parent->getPosition(),
        parent->AxisUp().getScaled(attributes.startOffset),
        attributes.direction,
        parent->getRotation(),
        PaletteColors::getPlayerProjectile(),
        0.0f,
        attributes.filterGroup,
        attributes.filterMask,
        attributes.sound,
        true,
        PaletteColors::getParticlesPlayerProjectileFrom(),
        PaletteColors::getParticlesPlayerProjectileTo()
    );
}

