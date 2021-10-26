#include "../../include/Game/ItemWeaponGhost.h"

const std::string &ItemWeaponGhost::getWeaponClassname() const {
    return weaponClassname;
}

void ItemWeaponGhost::setWeaponClassname(const std::string &weaponClassname) {
    ItemWeaponGhost::weaponClassname = weaponClassname;
}
