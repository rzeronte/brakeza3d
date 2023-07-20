//
// Created by eduardo on 20/07/23.
//

#ifndef BRAKEZA3D_ITEMSHIELDGHOST_H
#define BRAKEZA3D_ITEMSHIELDGHOST_H


#include "../../../include/Misc/Counter.h"
#include "../shaders/ShaderBlink.h"
#include "../../../include/Physics/Mesh3DGhost.h"

class ItemShieldGhost: public Mesh3DGhost  {
    float damage;
    Counter timeToLive;
    ShaderBlink *blink;
    Counter counterDamageBlink;

    explicit ItemShieldGhost(float ttl, float damage);

    [[nodiscard]] float getDamage() const;

    void onUpdate() override;

    void resolveCollision(Collisionable *collisionable) override;
};


#endif //BRAKEZA3D_ITEMSHIELDGHOST_H
