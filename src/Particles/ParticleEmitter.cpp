
#include "../../include/Particles/ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(
    Object3D *parent,
    Vertex3D position,
    int maxParticles,
    float ttlEmitter,
    float force,
    float ttl,
    float step,
    Color colorFrom,
    Color colorTo,
    Vertex3D rotationFrame
) :
    timeToNextParticleCounter(Counter(step)),
    active(true),
    activeAdding(true),
    rotFrameX(0),
    rotFrameY(0),
    rotFrameZ(0),
    maxParticles(maxParticles),
    lifeCounter(Counter(ttlEmitter)),
    force(force),
    ttl(ttl),
    colorTo(colorTo),
    colorFrom(colorFrom)
{
    setParent(parent);
    setPosition(position);
    setRotationFrame(rotationFrame.x, rotationFrame.y, rotationFrame.z);
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

    if (!isActive()) return;

    Object3D::onUpdate();

    if (parent != nullptr && !parent->isRemoved()) {
        setPosition(parent->getPosition());
    }

    lifeCounter.update();
    if (this->lifeCounter.isFinished()) {
        setRemoved(true);
    }

    updateParticles();

    setRotation(getRotation() * M3::getMatrixRotationForEulerAngles(rotFrameX, rotFrameY, rotFrameZ));

    timeToNextParticleCounter.update();
    if (timeToNextParticleCounter.isFinished() && isActiveAdding() && (int) particles.size() < maxParticles) {
        particles.emplace_back(this, this->force, this->ttl, this->colorFrom, this->colorTo);
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
