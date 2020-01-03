//
// Created by darkhead on 2/1/20.
//

#ifndef BRAKEDA3D_ITEMAMMOBODY_H
#define BRAKEDA3D_ITEMAMMOBODY_H


#include "../Physics/BillboardBody.h"

class ItemAmmoBody : public BillboardBody {
    std::string weaponClassname;  // Related with weapon by classname
public:
    int ammo = 50;
    const std::string &getWeaponClassname() const;

    void setWeaponClassname(const std::string &weaponClassname);

    int getAmmo() const;

    void setAmmo(int ammo);
};


#endif //BRAKEDA3D_ITEMAMMOBODY_H
