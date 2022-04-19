//
// Created by darkhead on 1/1/20.
//

#ifndef BRAKEDA3D_ITEMWEAPONGHOST_H
#define BRAKEDA3D_ITEMWEAPONGHOST_H


#include "../../../include/Physics/Mesh3DGhost.h"
#include "../weapons/Weapon.h"

class ItemWeaponGhost : public Mesh3DGhost {
public:
    ItemWeaponGhost(Weapon *weaponType);

    Weapon *weaponType;

    Weapon *getWeaponType() const;
};


#endif //BRAKEDA3D_ITEMWEAPONGHOST_H
