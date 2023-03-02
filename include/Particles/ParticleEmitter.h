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

class ParticleEmitter : public Object3D {
private:
    Counter timeToNextParticleCounter;
    bool active;
    bool activeAdding;
    float rotFrameX;
    float rotFrameY;
    float rotFrameZ;
protected:
    std::vector<Particle> particles;
    Counter lifeCounter;
    float force;
    float ttl;
    Color color;
public:
    ParticleEmitter(Object3D *parent, Vertex3D position, float ttlEmitter, float force, float ttl, float step, Color c);

    void setRotationFrame(float, float, float);

    void onUpdate() override;

    void postUpdate() override;

    void updateParticles();

    void setActive(bool value);

    void setActiveAdding(bool value);

    [[nodiscard]] bool isActiveAdding() const;

    [[nodiscard]] std::vector<Particle> &getParticles();

    [[nodiscard]] Counter &getTimeToNetParticleCounter();

    [[nodiscard]] bool isActive() const;
};


#endif //BRAKEDA3D_PARTICLEEMISSOR_H
