#include "ItemBombGhost.h"
#include "../../../include/Brakeza3D.h"

ItemBombGhost::ItemBombGhost(float ttl)
{
    this->ttl = ttl;
    timeToLive.setStep(ttl);
    timeToLive.setEnabled(true);
}

void ItemBombGhost::onUpdate()
{
    Mesh3D::onUpdate();

    if (!isEnabled()) return;

    timeToLive.update();

    if (timeToLive.isFinished()) {
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
        enemy->takeDamage(getDamage());
        setEnabled(false);
        removeCollisionObject();
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
