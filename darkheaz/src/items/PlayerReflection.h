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
    Counter timeToLive;

    ShaderBlink *blink;
    Counter counterDamageBlink;

public:
    explicit PlayerReflection(float ttl);

    void onUpdate() override;

    void postUpdate() override;

    void resolveCollision(Collisionable *objectWithCollision) override;

    ~PlayerReflection() override;
};


#endif //BRAKEDA3D_GRAVITATIONALSHIELD_H
