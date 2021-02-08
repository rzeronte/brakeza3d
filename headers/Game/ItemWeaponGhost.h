//
// Created by darkhead on 1/1/20.
//

#ifndef BRAKEDA3D_ITEMWEAPONGHOST_H
#define BRAKEDA3D_ITEMWEAPONGHOST_H


#include "../Physics/Mesh3DGhost.h"

class ItemWeaponGhost: public Mesh3DGhost {

    std::string weaponClassname;  // Related with weapon by classname

public:
    const std::string &getWeaponClassname() const;
    void  setWeaponClassname(const std::string &weaponClassname);
};


#endif //BRAKEDA3D_ITEMWEAPONGHOST_H
