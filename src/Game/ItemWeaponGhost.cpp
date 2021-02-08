//
// Created by darkhead on 1/1/20.
//

#include "../../headers/Game/ItemWeaponGhost.h"

const std::string &ItemWeaponGhost::getWeaponClassname() const {
    return weaponClassname;
}

void ItemWeaponGhost::setWeaponClassname(const std::string &weaponClassname) {
    ItemWeaponGhost::weaponClassname = weaponClassname;
}
