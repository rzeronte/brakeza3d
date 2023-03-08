#include "ItemBombGhost.h"
#include "../../../include/Brakeza3D.h"

ItemBombGhost::ItemBombGhost(float ttl, float damage): ttl(ttl), damage(damage)
{
    timeToLive.setStep(ttl);
    timeToLive.setEnabled(true);
}

void ItemBombGhost::onUpdate()
{
    Mesh3D::onUpdate();

    if (!isEnabled()) return;

    timeToLive.update();

    if (timeToLive.isFinished()) {
        setRemoved(true);
        setEnabled(false);
        removeCollisionObject();
    }
}

void ItemBombGhost::postUpdate() {
    Object3D::postUpdate();
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

float ItemBombGhost::getTtl() const {
    return ttl;
}

void ItemBombGhost::setTtl(float ttl) {
    ItemBombGhost::ttl = ttl;
}
