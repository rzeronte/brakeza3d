//
// Created by edu on 19/01/24.
//

#ifndef BRAKEZA3D_WEAPONBOMB_H
#define BRAKEZA3D_WEAPONBOMB_H

#include "Weapon.h"

class WeaponBomb : public Weapon {
public:
    explicit WeaponBomb(const WeaponAttributes &attributes);

    bool shoot(WeaponShootAttributes attributes) override;

    void shootBomb(Object3D *parent, Vertex3D position);
};


#endif //BRAKEZA3D_WEAPONBOMB_H
