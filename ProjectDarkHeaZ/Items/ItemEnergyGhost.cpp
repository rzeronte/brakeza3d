//
// Created by eduardo on 3/04/22.
//

#include "ItemEnergyGhost.h"
#include "../../include/ComponentsManager.h"

ItemEnergyGhost::ItemEnergyGhost() : energy(25)
{
}

void ItemEnergyGhost::setEnergy(float value) {
    this->energy = value;
}

float ItemEnergyGhost::getEnergy() const {
    return this->energy;
}

void ItemEnergyGhost::onUpdate()
{
    Mesh3D::onUpdate();

    this->magnetizableTo(ComponentsManager::get()->getComponentGame()->getPlayer());
}

void ItemEnergyGhost::resolveCollision(Collisionable *collisionable) {
    Mesh3DGhost::resolveCollision(collisionable);
}