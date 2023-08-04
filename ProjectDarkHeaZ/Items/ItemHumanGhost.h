//
// Created by eduardo on 8/07/23.
//

#ifndef BRAKEZA3D_ITEMHUMANGHOST_H
#define BRAKEZA3D_ITEMHUMANGHOST_H


#include "../../include/Physics/Mesh3DGhost.h"
#include "ItemWithTutorial.h"
#include "LivingObject.h"

class ItemHumanGhost : public Mesh3DGhost, public ItemWithTutorial, public LivingObject
{
public:
    explicit ItemHumanGhost();

    void onStart();

private:
    void resolveCollision(Collisionable *collisionable) override;

    void onUpdate() override;

    void takeDamage(float d);
};


#endif //BRAKEZA3D_ITEMHUMANGHOST_H
