//
// Created by eduardo on 9/3/22.
//

#include "../../include/Game/AmmoProjectileBody.h"
#include "../../include/ComponentsManager.h"

Weapon *AmmoProjectileBody::getWeaponType() const {
    return weaponType;
}

void AmmoProjectileBody::setWeaponType(Weapon *weaponType) {
    AmmoProjectileBody::weaponType = weaponType;
}

void AmmoProjectileBody::resolveCollision(Collisionable *collisionable) {
    auto projectile = dynamic_cast<AmmoProjectileBody*> (collisionable);
    if (projectile != nullptr) {
        return;
    }

    auto object = dynamic_cast<Object3D*> (collisionable);
    if (object != nullptr) {
        if (object == getParent()) {
            return;
        }
    }

    this->remove();
}

void AmmoProjectileBody::onUpdate() {
    Projectile3DBody::onUpdate();
    updateBoundingBox();
    if (!ComponentsManager::get()->getComponentCamera()->getCamera()->frustum->isAABBInFrustum(&this->aabb)) {
        this->remove();
    }
}
