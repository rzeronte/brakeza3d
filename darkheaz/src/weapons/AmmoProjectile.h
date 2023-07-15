//
// Created by eduardo on 22/01/23.
//

#ifndef BRAKEZA3D_AMMOPROJECTILE_H
#define BRAKEZA3D_AMMOPROJECTILE_H

#include "../../../include/Particles/ParticleEmitter.h"

class AmmoProjectile {
    float damage;
    Color color;
    Object3D *owner;
public:
    explicit AmmoProjectile(Object3D *owner, Color color, float damage) : damage(damage), color(color), owner(owner) {
    }

    [[nodiscard]] float getDamage() const {
        return damage;
    }

    void setDamage(float value) {
        AmmoProjectile::damage = value;
    }

    [[nodiscard]] const Color &getColor() const {
        return color;
    }

    [[nodiscard]] Object3D *getOwner() const {
        return owner;
    }
};


#endif //BRAKEZA3D_AMMOPROJECTILE_H
