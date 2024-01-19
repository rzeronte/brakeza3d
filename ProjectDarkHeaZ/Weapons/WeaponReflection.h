//
// Created by edu on 19/01/24.
//

#ifndef BRAKEZA3D_WEAPONREFLECTION_H
#define BRAKEZA3D_WEAPONREFLECTION_H


#include "Weapon.h"

class WeaponReflection : public Weapon {
public:

    explicit WeaponReflection(const WeaponAttributes &attributes);

    bool shoot(WeaponShootAttributes attributes) override;

    void shootHologram(Object3D *parent, Vertex3D position);
};


#endif //BRAKEZA3D_WEAPONREFLECTION_H
