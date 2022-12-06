#include "ItemHealthGhost.h"
#include "../../../include/ComponentsManager.h"

void ItemHealthGhost::setAid(float aid) {
    ItemHealthGhost::aid = aid;
}

ItemHealthGhost::ItemHealthGhost() : aid(25) {

}

void ItemHealthGhost::onUpdate()
{
    Mesh3D::onUpdate();

    this->magnetizableTo(ComponentsManager::get()->getComponentGame()->getPlayer());
}

float ItemHealthGhost::getAid() const {
    return aid;
}

void ItemHealthGhost::resolveCollision(Collisionable *collisionable) {
    Mesh3DGhost::resolveCollision(collisionable);
}