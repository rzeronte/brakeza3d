//
// Created by eduardo on 10/04/22.
//

#include "../../include/Game/EnemyGhostRespawnerEmissor.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Game/EnemyBehaviorCircle.h"

EnemyGhostRespawnerEmissor::EnemyGhostRespawnerEmissor(float step, EnemyGhost *enemy)
{
    setActive(true);
    setEnemy(enemy);
    setStep(step);
    setStop(false);
    this->counter.setStep(step);
}

const Counter &EnemyGhostRespawnerEmissor::getCounter() const {
    return counter;
}

void EnemyGhostRespawnerEmissor::setCounter(const Counter &counter) {
    EnemyGhostRespawnerEmissor::counter = counter;
}

bool EnemyGhostRespawnerEmissor::isActive() const {
    return active;
}

void EnemyGhostRespawnerEmissor::setActive(bool active) {
    EnemyGhostRespawnerEmissor::active = active;
}

void EnemyGhostRespawnerEmissor::onUpdate() {
    EnemyGhost::onUpdate();

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
        this->addEnemy();
        this->counter.setEnabled(true);
    }
 }

void EnemyGhostRespawnerEmissor::addEnemy()
{
    Logging::getInstance()->Log("addEnemy");

    auto *enemy = new EnemyGhost();
    enemy->setEnabled(true);
    enemy->setLabel("npc_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    enemy->setEnableLights(false);
    enemy->setPosition(getPosition());
    enemy->setStencilBufferEnabled(true);
    enemy->setScale(1);
    enemy->setSpeed(enemy->getSpeed());
    enemy->setStamina(enemy->getStamina());
    enemy->setStartStamina(enemy->getStartStamina());
    enemy->setEnableLights(true);
    enemy->setBehavior(new EnemyBehaviorCircle(getPosition(), 1, 5000));
    enemy->copyFrom(this->enemy);
    enemy->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), enemy, EngineSetup::collisionGroups::Enemy, EngineSetup::collisionGroups::AllFilter);
    enemy->setSoundChannel(-1);

    auto respawner = new EnemyGhostRespawner(enemy, 5);
    respawner->setLabel("respawner_" +ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());

    Brakeza3D::get()->addObject3D(respawner, respawner->getLabel());

    this->respawners.push_back(respawner);
}

bool EnemyGhostRespawnerEmissor::isStop() const {
    return stop;
}

void EnemyGhostRespawnerEmissor::setStop(bool stop) {
    EnemyGhostRespawnerEmissor::stop = stop;
}

float EnemyGhostRespawnerEmissor::getStopDuration() const {
    return stopDuration;
}

void EnemyGhostRespawnerEmissor::setStopDuration(float stopDuration)
{
    EnemyGhostRespawnerEmissor::stopDuration = stopDuration;
    this->counterStopDuration = new Counter(stopDuration);
    this->counterStopDuration->setEnabled(false);
}

float EnemyGhostRespawnerEmissor::getStopEvery() const {
    return stopEvery;
}

void EnemyGhostRespawnerEmissor::setStopEvery(float stopEverySeconds) {
    EnemyGhostRespawnerEmissor::stopEvery = stopEverySeconds;
    this->counterStopEvery = new Counter(stopEverySeconds);
    this->counterStopEvery->setEnabled(true);
}

float EnemyGhostRespawnerEmissor::getStep() const {
    return step;
}

void EnemyGhostRespawnerEmissor::setStep(float step) {
    EnemyGhostRespawnerEmissor::step = step;
}

EnemyGhost *EnemyGhostRespawnerEmissor::getEnemy() const {
    return enemy;
}

void EnemyGhostRespawnerEmissor::setEnemy(EnemyGhost *enemy) {
    EnemyGhostRespawnerEmissor::enemy = enemy;
}

