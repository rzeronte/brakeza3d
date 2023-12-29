#include "ItemBombGhost.h"
#include "../../include/Brakeza3D.h"
#include "../Common/ShockWave.h"

ItemBombGhost::ItemBombGhost(float ttl, float damage)
:
    damage(damage),
    timeToLive(Counter(ttl)),
    blink(new FXBlink(false, this, 0.05, PaletteColors::getPlayerDamageBlink())),
    counterDamageBlink(Counter(0.45))
{
    timeToLive.setEnabled(true);
    counterDamageBlink.setEnabled(false);
}

void ItemBombGhost::onUpdate()
{
    Mesh3D::onUpdate();

    if (!isEnabled()) return;

    timeToLive.update();

    counterDamageBlink.update();

    if (timeToLive.currentPercentage() > 75 && !counterDamageBlink.isEnabled()) {
        counterDamageBlink.setEnabled(true);
        blink->setEnabled(true);
        ComponentsManager::get()->getComponentSound()->sound("countdownItem", EngineSetup::SoundChannels::SND_GLOBAL, 0);
    }

    blink->update();
    if (timeToLive.isFinished()) {
        setRemoved(true);
        setEnabled(false);
        removeCollisionObject();
        Tools::makeExplosion(this, getPosition(), 5, OCParticlesContext::forExplosion(), PaletteColors::getExplosionEnemyFrom(), PaletteColors::getExplosionEnemyTo());
        Tools::makeFadeInSprite(getPosition(), ComponentsManager::get()->getComponentGame()->getExplosionSprite()->getAnimation());
        Brakeza3D::get()->addObject3D(new ShockWave(getPosition(), 0.50, 1, true), Brakeza3D::uniqueObjectLabel("shockWave"));
    }
}

void ItemBombGhost::resolveCollision(Collisionable *collisionable)
{
    Mesh3DGhost::resolveCollision(collisionable);

    auto *enemy = dynamic_cast<EnemyGhost*> (collisionable);
    if (enemy != nullptr) {
        removeCollisionObject();

        setEnabled(false);
        setRemoved(true);

        ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increaseHit(WEAPON_BOMB);

        enemy->takeDamage(getDamage());
        enemy->stuck(5);
    }
}

float ItemBombGhost::getDamage() const {
    return damage;
}

void ItemBombGhost::setDamage(float damage) {
    ItemBombGhost::damage = damage;
}
