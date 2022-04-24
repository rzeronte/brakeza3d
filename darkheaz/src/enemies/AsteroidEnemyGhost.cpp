//
// Created by eduardo on 15/04/22.
//

#include "AsteroidEnemyGhost.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"
#include "EnemyBehaviorRandom.h"

void AsteroidEnemyGhost::onUpdate()
{
    EnemyGhost::onUpdate();

    if (isRemoved()) {
        if (!isExplode()) {
            return;
        }

        for (int i = 0; i < getExplodeNumberPartitions(); i++) {
            addAsteroid();
        }
    }
}

void AsteroidEnemyGhost::addAsteroid() {
    auto *asteroid = new AsteroidEnemyGhost();
    asteroid->setEnabled(true);
    asteroid->setLabel("asteroid_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    asteroid->setEnableLights(false);
    asteroid->setPosition(getPosition());
    asteroid->setStencilBufferEnabled(true);
    asteroid->setScale(1);
    asteroid->setSpeed(speed);
    asteroid->setStamina(startStamina/this->getExplodeNumberPartitions());
    asteroid->setStartStamina(startStamina);
    asteroid->setEnableLights(true);
    asteroid->clone(modelPartitions);
    asteroid->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), asteroid, EngineSetup::Enemy, EngineSetup::AllFilter);
    asteroid->setSoundChannel(-1);
    asteroid->setExplode(false);

    auto currentMotionRandom = dynamic_cast<EnemyBehaviorRandom*> (getBehavior());
    if (currentMotionRandom != nullptr) {
        asteroid->setBehavior(new EnemyBehaviorRandom(currentMotionRandom->getSpeed()));
    }

    Brakeza3D::get()->addObject3D(asteroid, asteroid->getLabel());
}

bool AsteroidEnemyGhost::isExplode() const {
    return explode;
}

void AsteroidEnemyGhost::setExplode(bool explode) {
    AsteroidEnemyGhost::explode = explode;
}

Mesh3D *AsteroidEnemyGhost::getModelPartitions() const {
    return modelPartitions;
}

void AsteroidEnemyGhost::setModelPartitions(Mesh3D *modelPartitions) {
    AsteroidEnemyGhost::modelPartitions = modelPartitions;
}

int AsteroidEnemyGhost::getExplodeNumberPartitions() const {
    return explodeNumberPartitions;
}

void AsteroidEnemyGhost::setExplodeNumberPartitions(int explodeNumberPartitions) {
    AsteroidEnemyGhost::explodeNumberPartitions = explodeNumberPartitions;
}
