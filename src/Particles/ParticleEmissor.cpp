
#include "../../include/Particles/ParticleEmissor.h"
#include "../../include/Render/Logging.h"

ParticleEmissor::ParticleEmissor(bool active, float force, float ttl, float step, Color c) {
    this->active = active;
    this->counter.setStep(step);
    this->ttl = ttl;
    this->step = step;
    this->color = c;
    this->force = force;
    this->rotFrameX = 0;
    this->rotFrameY = 0;
    this->rotFrameZ = 0;
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

void ParticleEmissor::onUpdate()
{
    Object3D::onUpdate();

    if (!active) {
        return;
    }

    setRotation(getRotation() * M3::getMatrixRotationForEulerAngles(rotFrameX, rotFrameY, rotFrameZ));

    this->counter.update();

    if (this->counter.isFinished()) {
        this->addParticle(new Particle(this,this->force,this->ttl, this->color, true));
        this->counter.setEnabled(true);
    }
}

void ParticleEmissor::postUpdate() {
    Object3D::postUpdate();
}


void ParticleEmissor::setRotationFrame(float x, float y, float z) {
    this->rotFrameX = x;
    this->rotFrameY = y;
    this->rotFrameZ = z;
}



