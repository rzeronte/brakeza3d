#ifndef BRAKEDA3D_ITEMWEAPONGHOST_H
#define BRAKEDA3D_ITEMWEAPONGHOST_H

#include "../../include/Physics/Mesh3DGhost.h"
#include "../Weapons/Weapon.h"
#include "ItemWithTutorial.h"

class ItemWeaponGhost : public Mesh3DGhost, public ItemWithTutorial  {
public:
    explicit ItemWeaponGhost(Weapon *weaponType);

    Weapon *weaponType;

    [[nodiscard]] Weapon *getWeaponType() const;

    void onUpdate() override;

    void onDrawHostBuffer() override;

    void postUpdate() override;
};


#endif //BRAKEDA3D_ITEMWEAPONGHOST_H
