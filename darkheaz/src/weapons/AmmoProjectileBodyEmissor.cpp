#include "AmmoProjectileBodyEmissor.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

AmmoProjectileBodyEmissor::AmmoProjectileBodyEmissor(float step, Weapon *weaponType) : step(step), weaponType(weaponType)
{
    setStop(false);
    setActive(true);
    setStep(step);
    setWeapon(weaponType);
    this->counter.setStep(step);
}

const Counter &AmmoProjectileBodyEmissor::getCounter() const {
    return counter;
}

void AmmoProjectileBodyEmissor::setCounter(const Counter &counter) {
    AmmoProjectileBodyEmissor::counter = counter;
}

bool AmmoProjectileBodyEmissor::isActive() const {
    return active;
}

void AmmoProjectileBodyEmissor::setActive(bool active) {
    AmmoProjectileBodyEmissor::active = active;
}

void AmmoProjectileBodyEmissor::onUpdate() {
    Object3D::onUpdate();

    if (!active) {
        return;
    }

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

void AmmoProjectileBodyEmissor::setWeapon(Weapon *weaponType) {
    AmmoProjectileBodyEmissor::weaponType = weaponType;
}

void AmmoProjectileBodyEmissor::addProjectile()
{
    Logging::getInstance()->Log("addEnemy");

    Vertex3D direction = this->AxisForward().getNormalize();

    auto *projectile = new AmmoProjectileBody();
    projectile->setParent(this);
    projectile->setLabel("projectile_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    projectile->setWeaponType(this->weaponType);
    projectile->setEnableLights(false);
    projectile->setFlatTextureColor(weaponType->getModelProjectile()->isFlatTextureColor());
    projectile->setFlatColor(weaponType->getModelProjectile()->getFlatColor());
    projectile->clone(weaponType->getModelProjectile());
    projectile->setPosition( getPosition() );
    projectile->setEnabled(true);
    projectile->setTTL(EngineSetup::get()->PROJECTILE_DEMO_TTL);
    projectile->makeProjectileRigidBody(
            0.1,
            direction,
            (float) weaponType->getSpeed(),
            weaponType->getAccuracy(),
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::Projectile,
            EngineSetup::collisionGroups::Player
    );

    Brakeza3D::get()->addObject3D(projectile, projectile->getLabel());
}

bool AmmoProjectileBodyEmissor::isStop() const {
    return stop;
}

void AmmoProjectileBodyEmissor::setStop(bool stop) {
    AmmoProjectileBodyEmissor::stop = stop;
}

float AmmoProjectileBodyEmissor::getStopDuration() const {
    return stopDuration;
}

void AmmoProjectileBodyEmissor::setStopDuration(float stopDuration)
{
    AmmoProjectileBodyEmissor::stopDuration = stopDuration;
    this->counterStopDuration = new Counter(stopDuration);
    this->counterStopDuration->setEnabled(false);
}

float AmmoProjectileBodyEmissor::getStopEvery() const {
    return stopEvery;
}

void AmmoProjectileBodyEmissor::setStopEvery(float stopEverySeconds) {
    AmmoProjectileBodyEmissor::stopEvery = stopEverySeconds;
    this->counterStopEvery = new Counter(stopEverySeconds);
    this->counterStopEvery->setEnabled(true);
}

float AmmoProjectileBodyEmissor::getStep() const {
    return step;
}

void AmmoProjectileBodyEmissor::setStep(float step) {
    AmmoProjectileBodyEmissor::step = step;
}

