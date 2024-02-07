//
// Created by edu on 19/01/24.
//

#ifndef BRAKEZA3D_WEAPONSHIELD_H
#define BRAKEZA3D_WEAPONSHIELD_H


#include "Weapon.h"
#include "../Items/ItemShieldGhost.h"

#define MAX_SIMULTANEOUS_BOMBS 2

class WeaponShield : public Weapon {
    ItemShieldGhost *last;
public:
    explicit WeaponShield(const WeaponAttributes &attributes);

    bool shoot(WeaponShootAttributes attributes) override;

    [[maybe_unused]] void shootShield(Object3D *parent, Vertex3D position);

    void onUpdate() override;
};


#endif //BRAKEZA3D_WEAPONSHIELD_H
