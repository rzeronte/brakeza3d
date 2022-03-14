//
// Created by eduardo on 9/3/22.
//

#include "../../include/Game/AmmoProjectileGhost.h"
#include "../../include/ComponentsManager.h"

WeaponType *AmmoProjectileGhost::getWeaponType() const {
    return weaponType;
}

void AmmoProjectileGhost::setWeaponType(WeaponType *weaponType) {
    AmmoProjectileGhost::weaponType = weaponType;
}

void AmmoProjectileGhost::resolveCollision(Collisionable *collisionable) {
    auto projectile = dynamic_cast<AmmoProjectileGhost*> (collisionable);
    if (projectile != nullptr) {
        if (projectile->getParent() == getParent()) {
            return;
        }
    }

    this->remove();
}

void AmmoProjectileGhost::onUpdate() {
    Projectile3DBody::onUpdate();
    updateBoundingBox();
    if (!ComponentsManager::get()->getComponentCamera()->getCamera()->frustum->isAABBInFrustum(&this->aabb)) {
        this->remove();
    }
}
