
#include "Collisionable.h"

Collisionable::Collisionable(): collisionsEnabled(true), mass(0) {
}

void Collisionable::integrate() {

}

bool Collisionable::isCollisionsEnabled() {
    return collisionsEnabled;
}

void Collisionable::setCollisionsEnabled(bool value) {
    this->collisionsEnabled = value;
}
