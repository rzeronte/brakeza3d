//
// Created by eduardo on 3/04/22.
//

#include "include/ItemEnergyGhost.h"
ItemEnergyGhost::ItemEnergyGhost() : energy(25)
{

}

void ItemEnergyGhost::setEnergy(float energy) {
    this->energy = energy;
}

float ItemEnergyGhost::getEnergy() const {
    return this->energy;
}

void ItemEnergyGhost::resolveCollision(Collisionable *collisionable) {
    Mesh3DGhost::resolveCollision(collisionable);
}

