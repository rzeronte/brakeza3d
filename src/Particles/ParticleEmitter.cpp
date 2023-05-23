
#include "../../include/Particles/ParticleEmitter.h"
#include "../../include/Render/Transforms.h"
#include "../../include/ComponentsManager.h"

ParticleEmitter::ParticleEmitter(
    ParticleEmitterState state,
    Object3D *parent,
    Vertex3D position,
    float ttlEmitter,
    Color colorFrom,
    Color colorTo
) :
    state(state),
    active(true),
    lifeCounter(Counter(ttlEmitter)),
    colorTo(colorTo),
    colorFrom(colorFrom),
    stopAdd(false)
{
    setParent(parent);
    setPosition(position);

    lifeCounter.setEnabled(true);
    shaderParticles = new ShaderParticles(true, colorFrom, colorTo, OCParticlesContext());

    Point2D screenPoint = Transforms::WorldToPoint(position, ComponentsManager::get()->getComponentCamera()->getCamera());
    shaderExplosion = new ShaderExplosion(true, Color::red(), Color::yellow(), screenPoint);
}

bool ParticleEmitter::isActive() const {
    return active;
}

void ParticleEmitter::onUpdate()
{
    Object3D::onUpdate();

    if (isRemoved()) return;

    if (!isActive()) return;

    if (parent != nullptr && !parent->isRemoved()) {
        setPosition(parent->getPosition());
    }

    lifeCounter.update();
    if (this->lifeCounter.isFinished()) {
        setRemoved(true);
    }
}

void ParticleEmitter::drawCall()
{
    if (state == ParticleEmitterState::DEFAULT) {
        shaderParticles->update(
            Transforms::WorldToPoint(getPosition(),ComponentsManager::get()->getComponentCamera()->getCamera()),
            AxisForward(),
            1.0f
        );
    }

    if (state == ParticleEmitterState::EXPLOSION) {
        shaderExplosion->update();
    }
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

