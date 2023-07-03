#include "LivingObject.h"

LivingObject::LivingObject(float startStamina, float stamina, Mesh3D *parent)
:
    stamina(INITIAL_STAMINA),
    startStamina(INITIAL_STAMINA)
{
    blink = new ShaderBlink(true, parent, 0.05, Color::red());
}

float LivingObject::getStartStamina() const {
    return startStamina;
}

float LivingObject::getStamina() const {
    return stamina;
}

ShaderBlink *LivingObject::getBlink() const {
    return blink;
}

const Counter &LivingObject::getCounterDamageBlink() const {
    return counterDamageBlink;
}

void LivingObject::takeDamage(float damageTaken)
{
    stamina -= damageTaken;
}

