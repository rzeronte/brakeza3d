//
// Created by eduardo on 9/2/22.
//

#ifndef BRAKEDA3D_PARTICLEEMISSORGRAVITY_H
#define BRAKEDA3D_PARTICLEEMISSORGRAVITY_H


#include "ParticleEmissor.h"

class ParticleEmissorGravity : public ParticleEmissor {
public:
    ParticleEmissorGravity(bool active, float force, float ttl, float step, const Color &c);

    void onUpdate() override;
};


#endif //BRAKEDA3D_PARTICLEEMISSORGRAVITY_H
