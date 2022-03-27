#include "../../include/Game/ItemHealthGhost.h"

void ItemHealthGhost::setAid(float aid) {
    ItemHealthGhost::aid = aid;
}

ItemHealthGhost::ItemHealthGhost() : aid(25) {

}

float ItemHealthGhost::getAid() const {
    return aid;
}

void ItemHealthGhost::resolveCollision(Collisionable *collisionable) {
    Mesh3DGhost::resolveCollision(collisionable);
}
