//
// Created by eduardo on 20/07/23.
//

#include "ItemShieldGhost.h"
#include "../../include/Brakeza3D.h"
#include "../Common/ShockWave.h"

ItemShieldGhost::ItemShieldGhost(float ttl, float damage)
:
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
    if (timeToLive.isFinished()) {
        setRemoved(true);
        setEnabled(false);
        removeCollisionObject();
        Tools::makeExplosion(this, getPosition(), 5, OCParticlesContext::forExplosion(), PaletteColors::getExplosionEnemyFrom(), PaletteColors::getExplosionEnemyTo());
        Brakeza3D::get()->addObject3D(new ShockWave(getPosition(), 0.50, 1, ShockWaveParams::standard(), true), Brakeza3D::uniqueObjectLabel("shockWave"));
    }
}

void ItemShieldGhost::resolveCollision(Collisionable *collisionable)
{
    Mesh3DGhost::resolveCollision(collisionable);

    auto *enemy = dynamic_cast<EnemyGhost*> (collisionable);
    if (enemy != nullptr) {
        removeCollisionObject();

        setEnabled(false);
        setRemoved(true);

        ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increaseHit(WEAPON_BOMB);
        enemy->takeDamage(getDamage());
        Tools::makeExplosion(this, getPosition(), 5, OCParticlesContext::forExplosion(), PaletteColors::getExplosionEnemyFrom(), PaletteColors::getExplosionEnemyTo());
    }
}

float ItemShieldGhost::getDamage() const {
    return damage;
}
