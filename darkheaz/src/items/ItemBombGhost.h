//
// Created by eduardo on 4/12/22.
//

#ifndef BRAKEZA3D_ITEMBOMBGHOST_H
#define BRAKEZA3D_ITEMBOMBGHOST_H


#include "../../../include/Physics/Mesh3DGhost.h"
#include "../../../include/Misc/Counter.h"
#include "../../../include/Shaders/ShaderBlink.h"

class ItemBombGhost: public Mesh3DGhost {
    float damage;
    Counter timeToLive;
    float ttl;
public:
    explicit ItemBombGhost(float ttl);

    float getTtl() const;

    void setTtl(float ttl);

    float getDamage() const;

    void setDamage(float damage);

    void onUpdate() override;

    void postUpdate() override;

    void resolveCollision(Collisionable *collisionable) override;

};


#endif //BRAKEZA3D_ITEMBOMBGHOST_H
