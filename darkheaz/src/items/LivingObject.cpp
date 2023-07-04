#include "LivingObject.h"

LivingObject::LivingObject(Mesh3D *parent)
:
    stamina(INITIAL_STAMINA),
    startStamina(INITIAL_STAMINA),
    counterDamageBlink(Counter(0.45)),
    blink(nullptr)

{
}

float LivingObject::getStartStamina() const {
    return startStamina;
}

float LivingObject::getStamina() const {
    return stamina;
}

void LivingObject::setStamina(float stamina) {
    LivingObject::stamina = stamina;
}

void LivingObject::setStartStamina(float startStamina) {
    LivingObject::startStamina = startStamina;
}

