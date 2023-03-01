#include "AmmoProjectileBodyEmitter.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

AmmoProjectileBodyEmitter::AmmoProjectileBodyEmitter(
    ProjectileBodyEmmissorType type,
    float step,
    bool stop,
    float stopDuration,
    float stopEvery,
    Weapon *weaponType
) :
    step(step),
    stopDuration(stopDuration),
    stopEvery(stopEvery),
    active(false),
    stop(stop),
    weaponType(weaponType),
    counterStopDuration(Counter(stopDuration)),
    counterStopEvery(Counter(stopEvery)),
    type(type)
{
    this->counter.setStep(step);
}

bool AmmoProjectileBodyEmitter::isActive() const {
    return active;
}

void AmmoProjectileBodyEmitter::setActive(bool value) {
    AmmoProjectileBodyEmitter::active = value;
}

void AmmoProjectileBodyEmitter::onUpdate()
{
    Object3D::onUpdate();

    if (isRemoved()) return;

    if (!isActive())  return;

    this->counter.update();

    if (isStop()) {
        counterStopEvery.update();
        counterStopDuration.update();

        if (counterStopEvery.isFinished()) {
            counterStopDuration.setEnabled(true);
            counterStopEvery.setEnabled(false);
        }

        if (counterStopDuration.isFinished()) {
            counterStopEvery.setEnabled(true);
            counterStopDuration.setEnabled(false);
        }
    }

    if (isStop() && counterStopDuration.isEnabled()) {
        return;
    }

    if (this->counter.isFinished()) {
        this->addProjectile();
        this->counter.setEnabled(true);
    }
}

Weapon *AmmoProjectileBodyEmitter::getWeapon() const {
    return weaponType;
}

void AmmoProjectileBodyEmitter::setWeapon(Weapon *weapon) {
    AmmoProjectileBodyEmitter::weaponType = weapon;
}

void AmmoProjectileBodyEmitter::addProjectile()
{
    switch(getType()) {
        case ProjectileBodyEmmissorType::UNIQUE_PROJECTILE: {
            launchUniqueProjectile();
            break;
        }
        case ProjectileBodyEmmissorType::CIRCLE_PROJECTILE: {
            launchCircleProjectiles();
            break;
        }
    }
}

bool AmmoProjectileBodyEmitter::isStop() const {
    return stop;
}

void AmmoProjectileBodyEmitter::setStep(float value) {
    AmmoProjectileBodyEmitter::step = value;
}

AmmoProjectileBodyEmitter::~AmmoProjectileBodyEmitter()
{
    delete weaponType;
}

ProjectileBodyEmmissorType AmmoProjectileBodyEmitter::getType() const {
    return type;
}

void AmmoProjectileBodyEmitter::launchUniqueProjectile()
{
    if (isRemoved()) return;

    auto *projectile = new AmmoProjectileBody(
        getPosition(),
        this,
        weaponType,
        M3::getMatrixIdentity(),
        Vertex3D(50, 50, 50),
        this->AxisForward().getNormalize(),
        weaponType->getDamage(),
        (float) weaponType->getSpeed(),
        100,
        EngineSetup::get()->PROJECTILE_DEMO_TTL,
        EngineSetup::collisionGroups::ProjectileEnemy,
        EngineSetup::collisionGroups::Player,
        nullptr
    );

    auto *projectileParticleEmissor = new ParticleEmissor(projectile, getPosition(), 4, 1000, 1, 0.003, weaponType->getModelProjectile()->getFlatColor());
    projectileParticleEmissor->setRotationFrame(0, 25, 25);

    Brakeza3D::get()->addObject3D(projectile, "projectile_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    Brakeza3D::get()->addObject3D(projectileParticleEmissor, "projectileBodyParticleEmissor" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
}

void AmmoProjectileBodyEmitter::launchCircleProjectiles()
{
    int shoots = 8;
    for (int i = 0; i < shoots; i++) {
        setRotation(getRotation() * M3::getMatrixRotationForEulerAngles(0, 360.0f/(float) shoots, 0));
        launchUniqueProjectile();
    }
}

const Color &AmmoProjectileBodyEmitter::getColor() const {
    return color;
}

void AmmoProjectileBodyEmitter::setColor(const Color &color) {
    AmmoProjectileBodyEmitter::color = color;
}

