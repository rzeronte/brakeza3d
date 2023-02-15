
#include "../../include/Particles/ParticleEmissor.h"
#include "../../include/Render/Logging.h"

ParticleEmissor::ParticleEmissor(Object3D *parent, float ttlEmitter, float force, float ttl, float step, Color c):
    force(force),
    ttl(ttl),
    step(step),
    color(c),
    active(true),
    activeAdding(true),
    rotFrameX(0),
    rotFrameY(0),
    rotFrameZ(0)
{
    setParent(parent);
    setEnabled(true);
    setPosition(parent->getPosition());
    this->timeToNextParticleCounter.setStep(step);
    this->lifeCounter.setStep(ttlEmitter);
    this->lifeCounter.setEnabled(true);
}

Counter &ParticleEmissor::getTimeToNetParticleCounter() {
    return timeToNextParticleCounter;
}

bool ParticleEmissor::isActive() const {
    return active;
}

void ParticleEmissor::onUpdate()
{
    Object3D::onUpdate();

    if (isRemoved()) return;

    updateParticles();

    lifeCounter.update();
    if (this->lifeCounter.isFinished()) {
        setRemoved(true);
    }

    if (!isActive()) return;

    setRotation(getRotation() * M3::getMatrixRotationForEulerAngles(rotFrameX, rotFrameY, rotFrameZ));

    timeToNextParticleCounter.update();
    if (timeToNextParticleCounter.isFinished() && isActiveAdding()) {
        particles.emplace_back(new Particle(this, this->force, this->ttl, this->color, false));
        timeToNextParticleCounter.setEnabled(true);
    }
}

void ParticleEmissor::updateParticles()
{
    for (auto p : particles) {
        if (!p->isRemoved()) {
            p->onUpdate();
        }
    }
}

void ParticleEmissor::postUpdate()
{
    Object3D::postUpdate();
}

void ParticleEmissor::setRotationFrame(float x, float y, float z)
{
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

void ParticleEmissor::setActiveAdding(bool value) {
    ParticleEmissor::activeAdding = value;
}

std::vector<Particle *> &ParticleEmissor::getParticles() {
    return particles;
}

ParticleEmissor::~ParticleEmissor() {
    for (auto p : particles) {
        delete p;
    }
}