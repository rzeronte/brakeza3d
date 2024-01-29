//
// Created by edu on 18/01/24.
//

#include "ExplotableObject.h"

bool ExplotableObject::isExplode() const {
    return explode;
}

void ExplotableObject::setExplode(bool explode) {
    ExplotableObject::explode = explode;
}

float ExplotableObject::getExplodeDamage() const {
    return explodeDamage;
}

void ExplotableObject::setExplodeDamage(float explodeDamage) {
    ExplotableObject::explodeDamage = explodeDamage;
}
