//
// Created by edu on 19/01/24.
//

#include "WeaponLaser.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

WeaponLaser::WeaponLaser(const WeaponAttributes &attributes) : Weapon(attributes)
{

}

bool WeaponLaser::shootLaserProjectile(
    Object3D *parent,
    Vertex3D position,
    Vertex3D offsetPosition,
    Vertex3D direction,
    float intensity,
    bool sound,
    Color color,
    int filterGroup,
    int filterMask
)
{
    if (getAmmoAmount() <= 0) return false;

    if (isStop() && counterStopDuration.isEnabled()) {
        return false;
    }

    if (counterCadence->isFinished()) {
        float t = cadenceTime;

        auto storeManager = ComponentsManager::get()->getComponentGame()->getStoreManager();
        if (storeManager->isItemEnabled(EngineSetup::StoreItems::ITEM_FAST_SHOOT_CADENCE)){
            t = t - (t * 0.25f);
        }

        this->counterCadence->setStep(t);
        this->counterCadence->setEnabled(true);

        auto test  = Vertex3D();
        test.x += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy));
        test.y += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy));
        test.z = 0;
        test = test.getScaled(0.005);

        auto *projectile = new ProjectileRay(
            parent,
            position - offsetPosition,
            getDamage(),
            direction + test,
            1,
            filterGroup,
            filterMask,
            getSpeed(),
            color,
            intensity,
            false,
            ComponentsManager::get()->getComponentGame()->getSpriteSparklesGreen()->getAnimation()
        );

        setAmmoAmount(getAmmoAmount() - 1);

        ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increase(WEAPON_LASER);

        Brakeza3D::get()->addObject3D(projectile, Brakeza3D::uniqueObjectLabel("weaponProjectile"));

        if (sound) {
            ComponentsManager::get()->getComponentSound()->sound("projectileLaser", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }

        return true;
    }

    return false;
}

bool WeaponLaser::shoot(WeaponShootAttributes attributes)
{
    return shootLaserProjectile(
        parent,
        parent->getPosition(),
        parent->AxisUp().getScaled(attributes.startOffset),
        attributes.direction,
        0.00075f,
        attributes.sound,
        getColor(),
        attributes.filterGroup,
        attributes.filterMask
    );
}
