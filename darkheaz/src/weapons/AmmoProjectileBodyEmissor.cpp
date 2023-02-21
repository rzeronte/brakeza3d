#include "AmmoProjectileBodyEmissor.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

AmmoProjectileBodyEmissor::AmmoProjectileBodyEmissor(
    ProjectileBodyEmmissorType type,
    float step,
    Weapon *weaponType
) : step(step), weaponType(weaponType), type(type)
{
    setStop(false);
    setActive(false);

    this->counter.setStep(step);
}

const Counter &AmmoProjectileBodyEmissor::getCounter() const {
    return counter;
}

void AmmoProjectileBodyEmissor::setCounter(const Counter &c) {
    AmmoProjectileBodyEmissor::counter = c;
}

bool AmmoProjectileBodyEmissor::isActive() const {
    return active;
}

void AmmoProjectileBodyEmissor::setActive(bool value) {
    AmmoProjectileBodyEmissor::active = value;
}

void AmmoProjectileBodyEmissor::onUpdate()
{
    Object3D::onUpdate();

    if (isRemoved()) return;

    if (!isActive())  return;

    this->counter.update();

    if (isStop()) {
        counterStopEvery->update();
        counterStopDuration->update();

        if (counterStopEvery->isFinished()) {
            counterStopDuration->setEnabled(true);
            counterStopEvery->setEnabled(false);
        }

        if (counterStopDuration->isFinished()) {
            counterStopEvery->setEnabled(true);
            counterStopDuration->setEnabled(false);
        }
    }

    if (isStop() && counterStopDuration->isEnabled()) {
        return;
    }

    if (this->counter.isFinished()) {
        this->addProjectile();
        this->counter.setEnabled(true);
    }
}

Weapon *AmmoProjectileBodyEmissor::getWeapon() const {
    return weaponType;
}

void AmmoProjectileBodyEmissor::setWeapon(Weapon *weapon) {
    AmmoProjectileBodyEmissor::weaponType = weapon;
}

void AmmoProjectileBodyEmissor::addProjectile()
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

bool AmmoProjectileBodyEmissor::isStop() const {
    return stop;
}

void AmmoProjectileBodyEmissor::setStop(bool value) {
    AmmoProjectileBodyEmissor::stop = value;
}

float AmmoProjectileBodyEmissor::getStopDuration() const {
    return stopDuration;
}

void AmmoProjectileBodyEmissor::setStopDuration(float value)
{
    AmmoProjectileBodyEmissor::stopDuration = value;
    this->counterStopDuration = new Counter(stopDuration);
    this->counterStopDuration->setEnabled(false);
}

float AmmoProjectileBodyEmissor::getStopEvery() const {
    return stopEvery;
}

void AmmoProjectileBodyEmissor::setStopEvery(float stopEverySeconds)
{
    AmmoProjectileBodyEmissor::stopEvery = stopEverySeconds;
    this->counterStopEvery = new Counter(stopEverySeconds);
    this->counterStopEvery->setEnabled(true);
}

float AmmoProjectileBodyEmissor::getStep() const {
    return step;
}

void AmmoProjectileBodyEmissor::setStep(float value) {
    AmmoProjectileBodyEmissor::step = value;
}

AmmoProjectileBodyEmissor::~AmmoProjectileBodyEmissor()
{
    delete counterStopDuration;
    delete counterStopEvery;
    delete weaponType;
}

ProjectileBodyEmmissorType AmmoProjectileBodyEmissor::getType() const {
    return type;
}

void AmmoProjectileBodyEmissor::setType(ProjectileBodyEmmissorType value) {
    AmmoProjectileBodyEmissor::type = value;
}

void AmmoProjectileBodyEmissor::launchUniqueProjectile()
{
    if (isRemoved()) return;

    Brakeza3D::get()->addObject3D(new AmmoProjectileBody(
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
        EngineSetup::collisionGroups::Projectile,
        EngineSetup::collisionGroups::Player
    ), "projectile_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
}

void AmmoProjectileBodyEmissor::launchCircleProjectiles()
{
    int shoots = 8;
    for (int i = 0; i < shoots; i++) {
        setRotation(getRotation() * M3::getMatrixRotationForEulerAngles(0, 360.0f/(float) shoots, 0));
        launchUniqueProjectile();
    }
}

const Color &AmmoProjectileBodyEmissor::getColor() const {
    return color;
}

void AmmoProjectileBodyEmissor::setColor(const Color &color) {
    AmmoProjectileBodyEmissor::color = color;
}

