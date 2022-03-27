#include "../../include/Game/ItemWeaponGhost.h"

ItemWeaponGhost::ItemWeaponGhost(Weapon *weaponType) : weaponType(weaponType) {
}

Weapon *ItemWeaponGhost::getWeaponType() const {
    return weaponType;
}
