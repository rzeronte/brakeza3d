//
// Created by darkhead on 3/1/20.
//

#ifndef BRAKEDA3D_AMMOTYPE_H
#define BRAKEDA3D_AMMOTYPE_H


#include <string>
#include "../Objects/Mesh3DAnimated.h"
#include "ItemWeaponBody.h"

class AmmoType {
public:
    AmmoType();

    const std::string &getName() const;
    void  setName(const std::string &name);

    const std::string &getClassname() const;
    void  setClassname(const std::string &classname);

    int  getAmount() const;
    void setAmount(int ammo);

    Mesh3DAnimated* getModelProjectile();
    Mesh3DBody*     getModelBox();

private:
    std::string name;
    std::string classname;
    int amount;

    Mesh3DAnimated model_projectile;
    Mesh3DBody     model_box;
public:

public:
};


#endif //BRAKEDA3D_AMMOTYPE_H
