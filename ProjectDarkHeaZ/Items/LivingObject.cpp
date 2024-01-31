#include "LivingObject.h"

LivingObject::LivingObject(Mesh3D *parent)
:
    startStamina(0),
    stamina(0),
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

float LivingObject::currentStaminaPercentage() const
{
    return getStamina() * 100 / getStartStamina();
}

LivingObject::~LivingObject() {
    delete blink;
}

