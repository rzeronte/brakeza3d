
#ifndef BRAKEDA3D_WEAPONTYPE_H
#define BRAKEDA3D_WEAPONTYPE_H

#include "WeaponAnimation.h"

#define WEAPON_MAX_ANIMATIONSS 10

class WeaponType {
public:
    WeaponType(std::string label);

    std::string label;

    int num_animations = 0;
    int current_animation = 0;

    WeaponAnimation *animations[WEAPON_MAX_ANIMATIONSS];

    void addAnimation(std::string, int frames, int offsetX, int offsetY);

    WeaponAnimation *getCurrentWeaponAnimation();
};


#endif //BRAKEDA3D_WEAPONTYPE_H
