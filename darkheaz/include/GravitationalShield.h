//
// Created by eduardo on 3/04/22.
//

#ifndef BRAKEDA3D_GRAVITATIONALSHIELD_H
#define BRAKEDA3D_GRAVITATIONALSHIELD_H


#include "GravitationalGhost.h"
#include "../../include/Misc/Counter.h"
#include "../../include/Shaders/ShaderBlink.h"

class GravitationalShield : public GravitationalGhost {
private:
    float startStamina;
    float stamina;
    Counter timeToLive;
    float ttl;
    ShaderBlink *blink;
    Counter *counterDamageBlink;

public:
    GravitationalShield(float force, float factor, float stamina, float ttl);

    void onUpdate() override;

    void postUpdate() override;

    float getStartStamina() const;

    void setStartStamina(float startStamina);

    float getStamina() const;

    void setStamina(float stamina);

    void takeDamage(float damageTaken);

    void resolveCollision(Collisionable *collisionable) override;

};


#endif //BRAKEDA3D_GRAVITATIONALSHIELD_H
