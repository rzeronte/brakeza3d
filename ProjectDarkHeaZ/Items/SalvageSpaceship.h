//
// Created by eduardo on 7/08/23.
//

#ifndef BRAKEZA3D_SALVAGESPACESHIP_H
#define BRAKEZA3D_SALVAGESPACESHIP_H


#include "../../include/Physics/Mesh3DGhost.h"
#include "ItemWithTutorial.h"
#include "../../include/Misc/Counter.h"
#include "../../include/FXEffect/FXBlink.h"
#include "../../include/Behaviors/RotatableToTarget.h"

class SalvageSpaceship: public Mesh3DGhost, public ItemWithTutorial, RotatableToTarget {
    bool getting;
    bool finished;
    Counter timeGetting;
    Counter timeToRemove;
    FXBlink *blink;
    Vertex3D velocity;
public:
    SalvageSpaceship();

    void resolveCollision(Collisionable *collisionable) override;

    void onUpdate() override;

    void postUpdate() override;

};


#endif //BRAKEZA3D_SALVAGESPACESHIP_H
