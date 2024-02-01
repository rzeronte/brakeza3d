//
// Created by eduardo on 20/07/23.
//

#ifndef BRAKEZA3D_ITEMSHIELDGHOST_H
#define BRAKEZA3D_ITEMSHIELDGHOST_H


#include "../../include/Misc/Counter.h"
#include "../../include/Physics/Mesh3DGhost.h"
#include "../../include/FXEffect/FXBlink.h"
#include "../Weapons/Weapon.h"

class ItemShieldGhost: public Mesh3DGhost  {
    float damage;
    Counter timeToLive;
    FXBlink *blink;
    Counter counterDamageBlink;
    Weapon *weapon;

    [[nodiscard]] float getDamage() const;

    void onUpdate() override;

    void resolveCollision(Collisionable *collisionable) override;

public:
    explicit ItemShieldGhost(float ttl, float damage, Weapon *weapon);
};


#endif //BRAKEZA3D_ITEMSHIELDGHOST_H
