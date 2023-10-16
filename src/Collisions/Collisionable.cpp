
#include "Collisionable.h"

Collisionable::Collisionable(): collisionsEnabled(true) {
}

void Collisionable::integrate() {

}

bool Collisionable::isCollisionsEnabled() {
    return collisionsEnabled;
}

void Collisionable::setCollisionsEnabled(bool value) {
    this->collisionsEnabled = value;
}
