//
// Created by eduardo on 3/04/22.
//

#ifndef BRAKEDA3D_GRAVITATIONALSHIELD_H
#define BRAKEDA3D_GRAVITATIONALSHIELD_H

#include "GravitationalGhost.h"
#include "../../include/Misc/Counter.h"
#include "../../include/FXEffect/FXBlink.h"
#include "../../include/Objects/Swarm.h"

class PlayerReflection : public Mesh3D {
private:
    Counter timeToLive;

    FXBlink *blink;
    Counter counterDamageBlink;
    SwarmObject *swarmObject;
public:
    explicit PlayerReflection(float ttl);

    void onUpdate() override;

    void postUpdate() override;

    ~PlayerReflection() override;

    void setSwarmObject(SwarmObject *swarmObject);
};


#endif //BRAKEDA3D_GRAVITATIONALSHIELD_H
