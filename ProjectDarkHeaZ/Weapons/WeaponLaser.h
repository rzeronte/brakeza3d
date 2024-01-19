//
// Created by edu on 19/01/24.
//

#ifndef BRAKEZA3D_WEAPONLASER_H
#define BRAKEZA3D_WEAPONLASER_H


#include "Weapon.h"

class WeaponLaser : public Weapon {
public:
    explicit WeaponLaser(const WeaponAttributes &attributes);

    bool shoot(WeaponShootAttributes attributes) override;

    bool shootLaserProjectile(
        Object3D *parent,
        Vertex3D position,
        Vertex3D offsetPosition,
        Vertex3D direction,
        float intensity,
        bool sound,
        Color color,
        int filterGroup,
        int filterMask
    );
};


#endif //BRAKEZA3D_WEAPONLASER_H
