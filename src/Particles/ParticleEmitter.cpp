
#include "../../include/Particles/ParticleEmitter.h"
#include "../../include/Render/Logging.h"

ParticleEmitter::ParticleEmitter(Object3D *parent, Vertex3D position, float ttlEmitter, float force, float ttl, float step, Color c):
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
    setPosition(position);

    this->timeToNextParticleCounter.setStep(step);
    this->lifeCounter.setStep(ttlEmitter);
    this->lifeCounter.setEnabled(true);
}

Counter &ParticleEmitter::getTimeToNetParticleCounter() {
    return timeToNextParticleCounter;
}

bool ParticleEmitter::isActive() const {
    return active;
}

void ParticleEmitter::onUpdate()
{
    if (isRemoved()) return;

    Object3D::onUpdate();

    if (parent != nullptr) {
        setPosition(parent->getPosition());
    }

    updateParticles();

    lifeCounter.update();
    if (this->lifeCounter.isFinished()) {
        setRemoved(true);
    }

    if (!isActive()) return;

    setRotation(getRotation() * M3::getMatrixRotationForEulerAngles(rotFrameX, rotFrameY, rotFrameZ));

    timeToNextParticleCounter.update();
    if (timeToNextParticleCounter.isFinished() && isActiveAdding()) {
        particles.emplace_back(this, this->force, this->ttl, this->color);
        timeToNextParticleCounter.setEnabled(true);
    }
}

void ParticleEmitter::updateParticles()
{
    for (auto &p : particles) {
        p.onUpdate();
    }
}

void ParticleEmitter::postUpdate()
{
    Object3D::postUpdate();
}

void ParticleEmitter::setRotationFrame(float x, float y, float z)
{
    this->rotFrameX = x;
    this->rotFrameY = y;
    this->rotFrameZ = z;
}

void ParticleEmitter::setActive(bool value) {
    this->active = value;
}

bool ParticleEmitter::isActiveAdding() const {
    return activeAdding;
}

void ParticleEmitter::setActiveAdding(bool value) {
    ParticleEmitter::activeAdding = value;
}

std::vector<Particle> &ParticleEmitter::getParticles() {
    return particles;
}
