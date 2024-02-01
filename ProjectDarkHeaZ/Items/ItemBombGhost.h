
#ifndef BRAKEZA3D_ITEMBOMBGHOST_H
#define BRAKEZA3D_ITEMBOMBGHOST_H


#include "../../include/Physics/Mesh3DGhost.h"
#include "../../include/Misc/Counter.h"
#include "../../include/FXEffect/FXBlink.h"
#include "../Weapons/Weapon.h"

class ItemBombGhost: public Mesh3DGhost {
    float damage;
    Counter timeToLive;
    FXBlink *blink;
    Counter counterDamageBlink;
    Weapon *weapon;
public:
    explicit ItemBombGhost(float ttl, float damage, Weapon *weapon);

    [[nodiscard]] float getDamage() const;

    void onUpdate() override;

    void resolveCollision(Collisionable *collisionable) override;
};


#endif //BRAKEZA3D_ITEMBOMBGHOST_H
