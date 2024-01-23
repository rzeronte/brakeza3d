//
// Created by edu on 19/01/24.
//

#ifndef BRAKEZA3D_WEAPONRAYLIGHT_H
#define BRAKEZA3D_WEAPONRAYLIGHT_H


#include "Weapon.h"

class WeaponRayLight : public Weapon {
public:
    bool isPlayer;
    ParticleEmitter *particles;

    explicit WeaponRayLight(const WeaponAttributes &attributes);

private:
    bool shoot(WeaponShootAttributes attributes) override;

public:
    void onUpdate() override;

    bool shootRayLight(float intensity, Color color);

    void setIsForPlayer(bool avoidCheckWeaponStatus);

    void setEnabled(bool enabled) override;
};


#endif //BRAKEZA3D_WEAPONRAYLIGHT_H
