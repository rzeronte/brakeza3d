//
// Created by eduardo on 3/04/22.
//

#ifndef BRAKEDA3D_GRAVITATIONALSHIELD_H
#define BRAKEDA3D_GRAVITATIONALSHIELD_H


#include "GravitationalGhost.h"
#include "../../../include/Misc/Counter.h"
#include "../../../include/Shaders/ShaderBlink.h"
#include "../shaders/ShaderShockWave.h"
#include "../shaders/ShaderZombie.h"

class PlayerReflection : public Mesh3DGhost {
private:
    float startStamina;
    float stamina;
    ShaderBlink *blink;
    Counter counterDamageBlink;
    bool hidden;
    Counter timeToLive;
    ShaderZombie *zombie;
public:
    PlayerReflection(float stamina, float ttl);

    void onUpdate() override;

    void postUpdate() override;

    [[nodiscard]] float getStartStamina() const;

    void setStartStamina(float startStamina);

    [[nodiscard]] float getStamina() const;

    void setStamina(float stamina);

    void takeDamage(float damageTaken);

    void resolveCollision(Collisionable *objectWithCollision) override;


    void reset();

    [[nodiscard]] bool isHidden() const;

    void setHidden(bool hidden);

    void makeExplosion();

    void loadBlinkShader();
};


#endif //BRAKEDA3D_GRAVITATIONALSHIELD_H
