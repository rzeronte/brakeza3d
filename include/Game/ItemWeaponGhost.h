//
// Created by darkhead on 1/1/20.
//

#ifndef BRAKEDA3D_ITEMWEAPONGHOST_H
#define BRAKEDA3D_ITEMWEAPONGHOST_H


#include "../Physics/Mesh3DGhost.h"
#include "WeaponType.h"

class ItemWeaponGhost : public Mesh3DGhost {
public:
    ItemWeaponGhost(WeaponType *weaponType);

    WeaponType *weaponType;

    WeaponType *getWeaponType() const;
};


#endif //BRAKEDA3D_ITEMWEAPONGHOST_H
