//
// Created by eduardo on 3/04/22.
//

#ifndef BRAKEDA3D_GRAVITATIONALSHIELD_H
#define BRAKEDA3D_GRAVITATIONALSHIELD_H


#include "GravitationalGhost.h"
#include "../../../include/Misc/Counter.h"
#include "../../../include/Shaders/ShaderBlink.h"
#include "../shaders/ShaderShockWave.h"

class PlayerReflection : public Mesh3DGhost {
private:
    float startStamina;
    float stamina;
    float ttl;
    ShaderBlink *blink;
    Counter *counterDamageBlink;
    bool hidden;

public:
    PlayerReflection(float stamina, float ttl);

    virtual ~PlayerReflection();

    void onUpdate() override;

    void postUpdate() override;

    float getStartStamina() const;

    void setStartStamina(float startStamina);

    float getStamina() const;

    void setStamina(float stamina);

    void takeDamage(float damageTaken);

    void resolveCollision(Collisionable *collisionable) override;

    Counter timeToLive;

    void reset();

    bool isHidden() const;

    void setHidden(bool hidden);

    void loadBlinkShader();

    void makeExplosion();
};


#endif //BRAKEDA3D_GRAVITATIONALSHIELD_H