
#include "../../include/Particles/ParticleEmissor.h"
#include "../../include/Render/Logging.h"

ParticleEmissor::ParticleEmissor(bool active, float force, float ttl, float step, Color c) {
    this->active = active;
    this->counter.setStep(step);
    this->ttl = ttl;
    this->step = step;
    this->color = c;
    this->force = force;
}

const Counter &ParticleEmissor::getCounter() const {
    return counter;
}

void ParticleEmissor::clear() {
    this->particles.resize(0);
}

void ParticleEmissor::addParticle(Particle *p) {
    this->particles.push_back(p);
}


bool ParticleEmissor::isActive() const {
    return active;
}

const std::vector<Particle *> &ParticleEmissor::getParticles() const {
    return particles;
}

void ParticleEmissor::onUpdate() {

    if (!active) {
        return;
    }
    this->counter.update();

    if (this->counter.isFinished()) {
        this->addParticle(new Particle(this,this->force,this->ttl, this->color));
        this->counter.setEnabled(true);
    }
}
