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
    const Color &c,
    int maxFires,
    int particlesByFire
) :
    ParticleEmitter(this, position, 100, ttlEmitter, force, ttl, step, c),
    maxFires(maxFires),
    particlesByFire(particlesByFire),
    firesCounter(0)
{
    setPosition(position);
}

void ParticleEmitterFireworks::onUpdate()
{
    if (!isActive()) {
        return;
    }

    if (firesCounter < maxFires) {
        this->getTimeToNetParticleCounter().update();
        if (this->getTimeToNetParticleCounter().isFinished()) {
            this->addFire();
            this->getTimeToNetParticleCounter().setEnabled(true);
        }
    }

    lifeCounter.update();
    if (this->lifeCounter.isFinished()) {
        particles.clear();
        setRemoved(true);
    }
}

void ParticleEmitterFireworks::postUpdate()
{
    ParticleEmitter::postUpdate();
    updateFireWorksParticles();
}

void ParticleEmitterFireworks::addFire()
{
    setPosition(getPosition() + Vertex3D(
        (float) Tools::random(-100, 100),
        (float) Tools::random(-100, 100),
        (float) Tools::random(-100, 100)
    ));

    for (int i = 0; i < particlesByFire; i++) {
        setRotation(M3::getMatrixRotationForEulerAngles(
            (float)Tools::random(0, 360),
            (float)Tools::random(0, 360),
            (float)Tools::random(0, 360)
        ));

        particles.emplace_back(this, this->force, Tools::random(0, (int) this->ttl), this->color);
    }

    firesCounter++;
}

void ParticleEmitterFireworks::updateFireWorksParticles()
{
    for (auto &p : getParticles()) {
        p.onUpdate();
    }
}

