//
// Created by eduardo on 3/04/22.
//

#ifndef BRAKEDA3D_PARTICLEEMISSORFIREWORKS_H
#define BRAKEDA3D_PARTICLEEMISSORFIREWORKS_H


#include "ParticleEmitter.h"

class ParticleEmitterFireworks: public ParticleEmitter {
    int maxFires;
    int particlesByFire;
    int firesCounter;

public:
    ParticleEmitterFireworks(
        Vertex3D position,
        float ttlEmitter,
        float force,
        float ttl,
        float step,
        Color colorFrom,
        Color colorTo,
        int maxFires,
        int particlesByFire
    );

    void onUpdate() override;
    void postUpdate() override;
};


#endif //BRAKEDA3D_PARTICLEEMISSORFIREWORKS_H
