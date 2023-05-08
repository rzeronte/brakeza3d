//
// Created by eduardo on 3/04/22.
//

#include "../../include/Particles/ParticleEmitterFireworks.h"
#include "../../include/Misc/Tools.h"

ParticleEmitterFireworks::ParticleEmitterFireworks(
    Vertex3D position,
    float ttlEmitter,
    float force,
    float ttl,
    float step,
    Color colorFrom,
    Color colorTo,
    int maxFires,
    int particlesByFire
) :
    ParticleEmitter(this, position, ttlEmitter, force, ttl, step, colorFrom, colorTo, Vertex3D(0, 0, 0)),
    maxFires(maxFires),
    particlesByFire(particlesByFire),
    firesCounter(0)
{
    setPosition(position);
}

void ParticleEmitterFireworks::onUpdate()
{
    setEnabled(false);

    if (!isActive()) {
        return;
    }

    /*if (firesCounter < maxFires) {
        this->getTimeToNetParticleCounter().update();
        if (this->getTimeToNetParticleCounter().isFinished()) {
            this->addFire();
            this->getTimeToNetParticleCounter().setEnabled(true);
        }
    }*/

    lifeCounter.update();
    if (this->lifeCounter.isFinished()) {
        setRemoved(true);
    }
}

void ParticleEmitterFireworks::postUpdate()
{
    ParticleEmitter::postUpdate();
}


