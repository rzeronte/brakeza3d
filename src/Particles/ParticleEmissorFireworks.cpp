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
        int particlesByFire) : ParticleEmissor(this, ttlEmitter, force, ttl, step, c),
                               maxFires(maxFires),
                               particlesByFire(particlesByFire)
{
    setPosition(position);
    this->firesCounter = 0;
}

void ParticleEmissorFireworks::onUpdate()
{
    Object3D::onUpdate();

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
        setRemoved(true);
        return;
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
        Tools::random(-100, 100),
        Tools::random(-100, 100),
        Tools::random(-100, 100)
    ));

    for (int i = 0; i < particlesByFire; i++) {
        setRotation(M3::getMatrixRotationForEulerAngles(
            Tools::random(0, 360),
            Tools::random(0, 360),
            Tools::random(0, 360)
        ));

        particles.emplace_back(new Particle(this, this->force, Tools::random(0, this->ttl), this->color));
    }

    firesCounter++;
}

void ParticleEmissorFireworks::updateFireWorksParticles()
{
    for (auto p : getParticles()) {
        if (p->isRemoved()) continue;

        p->onUpdate();
    }
}

