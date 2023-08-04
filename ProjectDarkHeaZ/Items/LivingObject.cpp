#include "LivingObject.h"

LivingObject::LivingObject(Mesh3D *parent)
:
    startStamina(INITIAL_STAMINA),
    stamina(INITIAL_STAMINA),
    blink(nullptr),
    counterDamageBlink(Counter(0.45))
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

float LivingObject::currentStaminaPercentage()
{
    return getStamina() * 100 / getStartStamina();
}

