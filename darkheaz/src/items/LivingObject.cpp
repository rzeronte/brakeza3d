#include "LivingObject.h"

LivingObject::LivingObject(Mesh3D *parent)
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

void LivingObject::takeDamage(float damageTaken)
{
    stamina -= damageTaken;
}

