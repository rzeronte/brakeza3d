
#include "../../include/Particles/ParticleEmitter.h"
#include "../../include/Render/Transforms.h"
#include "../../include/ComponentsManager.h"

ParticleEmitter::ParticleEmitter(
    Object3D *parent,
    Vertex3D position,
    float ttlEmitter,
    float force,
    float ttl,
    float step,
    Color colorFrom,
    Color colorTo,
    Vertex3D rotationFrame
) :
    active(true),
    rotFrameX(0),
    rotFrameY(0),
    rotFrameZ(0),
    lifeCounter(Counter(ttlEmitter)),
    force(force),
    ttl(ttl),
    colorTo(colorTo),
    colorFrom(colorFrom)
{
    setParent(parent);
    setPosition(position);
    setRotationFrame(rotationFrame.x, rotationFrame.y, rotationFrame.z);

    lifeCounter.setEnabled(true);
    shaderParticles = new ShaderParticles(true, colorFrom, colorTo, OCParticlesContext());
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

    //setRotation(getRotation() * M3::getMatrixRotationForEulerAngles(rotFrameX, rotFrameY, rotFrameZ));
}

void ParticleEmitter::drawCall()
{
    shaderParticles->update(
        Transforms::WorldToPoint(getPosition(),ComponentsManager::get()->getComponentCamera()->getCamera()),
        AxisForward(),
        1.0f
    );
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

ParticleEmitter::~ParticleEmitter() {
    delete shaderParticles;
}

bool ParticleEmitter::isStopAdd() const {
    return stopAdd;
}

void ParticleEmitter::setStopAdd(bool stopAdd) {
    ParticleEmitter::stopAdd = stopAdd;
    shaderParticles->setStopAdd(stopAdd);
}

