
#include "../../include/Particles/ParticleEmitter.h"
#include "../../include/Render/Transforms.h"
#include "../../include/ComponentsManager.h"

ParticleEmitter::ParticleEmitter(
    ParticleEmitterState state,
    Object3D *parent,
    Vertex3D position,
    float ttlEmitter,
    Color colorFrom,
    Color colorTo,
    OCParticlesContext particlesContext
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
    shaderParticles = new ShaderParticles(true, colorFrom, colorTo, particlesContext);

    shaderExplosion = new ShaderExplosion(
        true,
        colorFrom,
        colorTo,
        Transforms::WorldToPoint(position, ComponentsManager::get()->getComponentCamera()->getCamera()),
        EngineSetup::get()->SHADER_PARTICLE_EXPLOSION_EMISSION_TIME,
        particlesContext
    );
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
        shaderParticles->update();
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

