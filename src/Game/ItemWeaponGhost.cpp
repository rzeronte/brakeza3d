#include "../../include/Game/ItemWeaponGhost.h"

ItemWeaponGhost::ItemWeaponGhost(WeaponType *weaponType) : weaponType(weaponType) {
}

WeaponType *ItemWeaponGhost::getWeaponType() const {
    return weaponType;
}
