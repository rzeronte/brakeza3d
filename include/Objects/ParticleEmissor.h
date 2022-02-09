//
// Created by eduardo on 8/2/22.
//

#ifndef BRAKEDA3D_PARTICLEEMISSOR_H
#define BRAKEDA3D_PARTICLEEMISSOR_H


#include <vector>
#include "Object3D.h"
#include "Particle.h"
#include "../Misc/Timer.h"
#include "../Misc/Counter.h"
#include "../Misc/Color.h"

class ParticleEmissor : public Object3D {

public:
    ParticleEmissor(bool active, float force, float ttl, float step, Color c);

    const std::vector<Particle *> &getParticles() const;

    const Counter &getCounter() const;
    void clear();
    void addParticle(Particle *p);
    void onUpdate() override;
    bool isActive() const;

    float force;
    float ttl;
    float step;
    Color color;
    Counter timer;
private:
    std::vector<Particle*> particles;
    bool active;
};


#endif //BRAKEDA3D_PARTICLEEMISSOR_H
