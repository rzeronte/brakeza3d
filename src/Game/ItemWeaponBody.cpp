//
// Created by darkhead on 1/1/20.
//

#include "../../headers/Game/ItemWeaponBody.h"

const std::string &ItemWeaponBody::getWeaponClassname() const {
    return weaponClassname;
}

void ItemWeaponBody::setWeaponClassname(const std::string &weaponClassname) {
    ItemWeaponBody::weaponClassname = weaponClassname;
}
