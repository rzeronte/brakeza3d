//
// Created by eduardo on 22/01/23.
//

#ifndef BRAKEZA3D_AMMOPROJECTILE_H
#define BRAKEZA3D_AMMOPROJECTILE_H

class AmmoProjectile {
    float damage;
public:
    explicit AmmoProjectile(float damage) : damage(damage) {

    }

    [[nodiscard]] float getDamage() const {
        return damage;
    }

    void setDamage(float value) {
        AmmoProjectile::damage = value;
    }
};


#endif //BRAKEZA3D_AMMOPROJECTILE_H
