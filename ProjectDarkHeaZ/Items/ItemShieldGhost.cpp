//
// Created by eduardo on 20/07/23.
//

#include "ItemShieldGhost.h"
#include "../../include/Brakeza3D.h"
#include "../Common/ShockWave.h"

ItemShieldGhost::ItemShieldGhost(float ttl, float damage, Weapon *weapon)
:
    weapon(weapon),
    damage(damage),
    timeToLive(Counter(ttl)),
    blink(new FXBlink(false, this, 0.05, PaletteColors::getPlayerDamageBlink())),
    counterDamageBlink(Counter(0.45))
{
    timeToLive.setEnabled(true);
    counterDamageBlink.setEnabled(false);
}

void ItemShieldGhost::onUpdate()
{
    Mesh3D::onUpdate();

    if (!isEnabled()) return;

    timeToLive.update();

    counterDamageBlink.update();

    if (timeToLive.currentPercentage() > 80 && !counterDamageBlink.isEnabled()) {
        counterDamageBlink.setEnabled(true);
        blink->setEnabled(true);
        ComponentsManager::get()->getComponentSound()->sound("energyOff", EngineSetup::SoundChannels::SND_GLOBAL, 0);
    }

    blink->update();
    if (timeToLive.isFinished() && !isRemoved()) {
        die();
    }
}

void ItemShieldGhost::resolveCollision(Collisionable *collisionable)
{
    Mesh3DGhost::resolveCollision(collisionable);

    if (isRemoved()) return;

    auto *enemy = dynamic_cast<EnemyGhost*> (collisionable);
    if (enemy != nullptr) {
        die();
        Tools::makeFadeInSprite(getPosition(), ComponentsManager::get()->getComponentGame()->getRandomExplosionSprite()->getAnimation());
    }
}

void ItemShieldGhost::die()
{
    removeCollisionObject();

    setEnabled(false);
    setRemoved(true);
    weapon->decreaseNumberProjectiles();

}

float ItemShieldGhost::getDamage() const {
    return damage;
}
