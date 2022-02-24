
#include "Collisionable.h"

Collisionable::Collisionable() {
    setCollisionsEnabled(true);
}

void Collisionable::integrate() {

}

bool Collisionable::isCollisionsEnabled() {
    return collisionsEnabled;
}

void Collisionable::setCollisionsEnabled(bool value) {
    this->collisionsEnabled = value;
}
