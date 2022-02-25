#ifndef BRAKEDA3D_AMMOTYPE_H
#define BRAKEDA3D_AMMOTYPE_H


#include <string>
#include "../Objects/Mesh3DAnimated.h"
#include "ItemWeaponGhost.h"

class AmmoType {
public:
    AmmoType();

    const std::string &getName() const;

    void setName(const std::string &name);

    int getAmount() const;

    void setAmount(int ammo);

    int getReloads() const;

    void setReloads(int reloads);

    int getReloadAmount() const;

    void setReloadAmount(int reloadAmount);

    Mesh3DBody *getModelProjectile();

    Mesh3DBody *getModelBox();

private:
    std::string name;
    std::string classname;

    int amount;
    int reloads;
    int reload_amount;

    Mesh3DBody model_projectile;
    Mesh3DBody model_box;
public:

public:
};


#endif //BRAKEDA3D_AMMOTYPE_H
