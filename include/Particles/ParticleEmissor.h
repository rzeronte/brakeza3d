//
// Created by eduardo on 8/2/22.
//

#ifndef BRAKEDA3D_PARTICLEEMISSOR_H
#define BRAKEDA3D_PARTICLEEMISSOR_H


#include <vector>
#include "../Objects/Object3D.h"
#include "Particle.h"
#include "../Misc/Timer.h"
#include "../Misc/Counter.h"
#include "../Misc/Color.h"

class ParticleEmissor : public Object3D {

public:
    ParticleEmissor(bool active, float force, float ttl, float step, Color c);

    const Counter &getCounter() const;
    void clear();
    void addParticle(Particle *p);
    void setRotationFrame(float, float, float);
    bool isActive() const;
    void onUpdate() override;

    float force;
    float ttl;
    float step;
    Color color;
    Counter counter;
    std::vector<Particle*> particles;
    bool active;
    float rotFrameX;
    float rotFrameY;
    float rotFrameZ;
};


#endif //BRAKEDA3D_PARTICLEEMISSOR_H
