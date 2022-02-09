
#include "../../include/Objects/ParticleEmissor.h"
#include "../../include/Render/Logging.h"

ParticleEmissor::ParticleEmissor(bool active, float force, float ttl, float step, Color c) {
    this->active = active;
    this->timer.setStep(step);
    this->ttl = ttl;
    this->step = step;
    this->color = c;
    this->force = force;
}

const Counter &ParticleEmissor::getCounter() const {
    return timer;
}

void ParticleEmissor::clear() {
    this->particles.resize(0);
}

void ParticleEmissor::addParticle(Particle *p) {
    this->particles.push_back(p);
}

void ParticleEmissor::onUpdate() {
    Object3D::onUpdate();

    if (!active) {
        return;
    }

    this->timer.update();

    if (this->timer.isFinished()) {
        this->addParticle(new Particle(this,this->force,this->ttl, this->color));
        this->timer.setEnabled(true);
    }

    std::vector<Particle *>::iterator it;
    for (it = particles.begin(); it != particles.end();) {
        Particle *object = *(it);

        if (object->isRemoved()) {
            particles.erase(it);
            continue;
        } else {
            it++;
        }

        object->onUpdate();
    }
}

bool ParticleEmissor::isActive() const {
    return active;
}

const std::vector<Particle *> &ParticleEmissor::getParticles() const {
    return particles;
}
