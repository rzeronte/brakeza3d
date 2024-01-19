//
// Created by edu on 19/01/24.
//

#ifndef BRAKEZA3D_WEAPONPROJECTILES_H
#define BRAKEZA3D_WEAPONPROJECTILES_H

#include "Weapon.h"

class WeaponProjectiles: public Weapon {
public:
    explicit WeaponProjectiles(const WeaponAttributes &attributes);

    bool shoot(WeaponShootAttributes attributes) override;

    bool shootProjectile(
        Object3D *parent,
        Vertex3D position,
        Vertex3D offsetPosition,
        Vertex3D direction,
        M3 rotation,
        Color color,
        float intensity,
        int filterGroup,
        int filterMask,
        bool sound,
        bool allowMirror,
        Color particlesFrom,
        Color particlesTo
    );
};


#endif //BRAKEZA3D_WEAPONPROJECTILES_H
