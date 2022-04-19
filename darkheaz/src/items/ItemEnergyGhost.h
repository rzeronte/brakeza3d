//
// Created by eduardo on 3/04/22.
//

#ifndef BRAKEDA3D_ITEMENERGYGHOST_H
#define BRAKEDA3D_ITEMENERGYGHOST_H


#include "../../../include/Physics/Mesh3DGhost.h"

class ItemEnergyGhost : public Mesh3DGhost {
    float energy;
public:
    ItemEnergyGhost();

    void setEnergy(float energy);

    float getEnergy() const;

    void resolveCollision(Collisionable *collisionable) override;
};

#endif //BRAKEDA3D_ITEMENERGYGHOST_H
