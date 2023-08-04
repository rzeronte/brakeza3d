//
// Created by eduardo on 3/04/22.
//

#ifndef BRAKEDA3D_ITEMENERGYGHOST_H
#define BRAKEDA3D_ITEMENERGYGHOST_H


#include "../../include/Physics/Mesh3DGhost.h"
#include "ItemWithTutorial.h"

class ItemEnergyGhost : public Mesh3DGhost, public ItemWithTutorial {
    float energy;
public:
    ItemEnergyGhost();

    void setEnergy(float energy);

    [[nodiscard]] float getEnergy() const;

    void resolveCollision(Collisionable *collisionable) override;

    void onUpdate() override;
};

#endif //BRAKEDA3D_ITEMENERGYGHOST_H
