//
// Created by eduardo on 3/04/22.
//

#include "../../include/Particles/ParticleEmissorFireworks.h"
#include "../../include/Misc/Tools.h"

ParticleEmissorFireworks::ParticleEmissorFireworks(
    Vertex3D position,
    float ttlEmitter,
    float force,
    float ttl,
    float step,
    const Color &c,
    int maxFires,
    int particlesByFire
) :
    ParticleEmissor(this, position, ttlEmitter, force, ttl, step, c),
    maxFires(maxFires),
    particlesByFire(particlesByFire)
{
    setPosition(position);
    this->firesCounter = 0;
}

void ParticleEmissorFireworks::onUpdate()
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

void ParticleEmissorFireworks::postUpdate()
{
    ParticleEmissor::postUpdate();
    updateFireWorksParticles();
}

void ParticleEmissorFireworks::addFire()
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

void ParticleEmissorFireworks::updateFireWorksParticles()
{
    for (auto &p : getParticles()) {
        p.onUpdate();
    }
}

