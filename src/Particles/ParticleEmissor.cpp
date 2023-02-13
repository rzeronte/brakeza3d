
#include "../../include/Particles/ParticleEmissor.h"
#include "../../include/Render/Logging.h"

ParticleEmissor::ParticleEmissor(bool active, float force, float ttl, float step, Color c): active(active), activeAdding(true) {
    this->counter.setStep(step);
    this->lifeCounter.setStep(5);
    this->lifeCounter.setEnabled(true);
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

    updateParticles();

    if (!isActive()) {
        return;
    }
    lifeCounter.update();

    setRotation(getRotation() * M3::getMatrixRotationForEulerAngles(rotFrameX, rotFrameY, rotFrameZ));

    this->counter.update();

    if (this->counter.isFinished() && isActiveAdding()) {
        this->addParticle(new Particle(this, this->force, this->ttl, this->color, false));
        this->counter.setEnabled(true);
    }

    if (this->lifeCounter.isFinished()) {
        setRemoved(true);
    }

}

void ParticleEmissor::updateParticles() {
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

void ParticleEmissor::postUpdate() {
    Object3D::postUpdate();
}


void ParticleEmissor::setRotationFrame(float x, float y, float z) {
    this->rotFrameX = x;
    this->rotFrameY = y;
    this->rotFrameZ = z;
}

void ParticleEmissor::setActive(bool value) {
    ParticleEmissor::active = value;
}

bool ParticleEmissor::isActiveAdding() const {
    return activeAdding;
}

void ParticleEmissor::setActiveAdding(bool activeAdding) {
    ParticleEmissor::activeAdding = activeAdding;
}



