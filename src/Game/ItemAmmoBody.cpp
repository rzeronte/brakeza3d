//
// Created by darkhead on 2/1/20.
//

#include "../../headers/Game/ItemAmmoBody.h"

const std::string &ItemAmmoBody::getWeaponClassname() const {
    return weaponClassname;
}

void ItemAmmoBody::setWeaponClassname(const std::string &weaponClassname) {
    ItemAmmoBody::weaponClassname = weaponClassname;
}

int ItemAmmoBody::getAmmo() const {
    return ammo;
}

void ItemAmmoBody::setAmmo(int ammo) {
    ItemAmmoBody::ammo = ammo;
}
