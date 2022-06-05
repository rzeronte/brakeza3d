//
// Created by eduardo on 3/04/22.
//

#ifndef BRAKEDA3D_PARTICLEEMISSORFIREWORKS_H
#define BRAKEDA3D_PARTICLEEMISSORFIREWORKS_H


#include "ParticleEmissor.h"

class ParticleEmissorFireworks: public ParticleEmissor {
    int maxFires;
    int particlesByFire;
    int firesCounter;

public:
    ParticleEmissorFireworks(bool active, float force, float ttl, float step, const Color &c, int maxFires, int particlesByFire);

    void onUpdate() override;

    void addFire();

    void updateFireWorksParticles();

    void postUpdate() override;
};


#endif //BRAKEDA3D_PARTICLEEMISSORFIREWORKS_H
