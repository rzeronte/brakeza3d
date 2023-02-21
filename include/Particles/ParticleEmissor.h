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
private:
    Counter timeToNextParticleCounter;
protected:
    std::vector<Particle> particles;
    Counter lifeCounter;
public:
    ParticleEmissor(Object3D *parent, float ttlEmitter, float force, float ttl, float step, Color c);

    [[nodiscard]] Counter &getTimeToNetParticleCounter();
    void setRotationFrame(float, float, float);
    [[nodiscard]] bool isActive() const;
    void onUpdate() override;

    void postUpdate() override;

    float force;
    float ttl;
    float step;
    Color color;
    bool active;
    bool activeAdding;
    float rotFrameX;
    float rotFrameY;
    float rotFrameZ;

    void updateParticles();

    void setActive(bool value);

    [[nodiscard]] bool isActiveAdding() const;

    void setActiveAdding(bool value);

    [[nodiscard]] std::vector<Particle> &getParticles();
};


#endif //BRAKEDA3D_PARTICLEEMISSOR_H
