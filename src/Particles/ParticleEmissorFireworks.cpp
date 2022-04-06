//
// Created by eduardo on 3/04/22.
//

#include "../../include/Particles/ParticleEmissorFireworks.h"
#include "../../include/Misc/Tools.h"

ParticleEmissorFireworks::ParticleEmissorFireworks(
        bool active,
        float force,
        float ttl,
        float step,
        const Color &c,
        int maxFires,
        int particlesByFire) : ParticleEmissor(active, force, ttl, step, c),
                               maxFires(maxFires),
                               particlesByFire(particlesByFire)
{
    this->firesCounter = 0;
}

void ParticleEmissorFireworks::onUpdate() {
    Object3D::onUpdate();

    if (!active) {
        return;
    }


    if (firesCounter >= maxFires) {
        if ((int)particles.size() == 0) {
            setRemoved(true);
        }
        return;
    }

    this->counter.update();
    if (this->counter.isFinished()) {
        this->addFire();
        this->counter.setEnabled(true);
    }
}

void ParticleEmissorFireworks::postUpdate() {
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


        this->addParticle(new Particle(this,this->force,Tools::random(0, this->ttl), this->color, false));
    }
    firesCounter++;
}

void ParticleEmissorFireworks::updateFireWorksParticles()
{
    std::vector<Particle *>::iterator it;
    for (it = particles.begin(); it != particles.end();) {
        Particle *p = *(it);

        if (p->isRemoved()) {
            particles.erase(it);
            continue;
        } else {
            it++;
        }

        p->onUpdate();
    }
}

