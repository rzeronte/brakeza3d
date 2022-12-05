//
// Created by eduardo on 4/12/22.
//

#ifndef BRAKEZA3D_ITEMBOMBGHOST_H
#define BRAKEZA3D_ITEMBOMBGHOST_H


#include "../../../include/Physics/Mesh3DGhost.h"
#include "../../../include/Misc/Counter.h"

class ItemBombGhost: public Mesh3DGhost {
    float damage;
    Counter timeToLive;
    float ttl;
public:
    float getTtl() const;

    void setTtl(float ttl);

public:
    float getDamage() const;

    void setDamage(float damage);

public:
    ItemBombGhost(float ttl);

    void onUpdate() override;

    void postUpdate() override;

    void resolveCollision(Collisionable *collisionable) override;

};


#endif //BRAKEZA3D_ITEMBOMBGHOST_H
