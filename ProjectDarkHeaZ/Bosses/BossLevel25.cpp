//
// Created by edu on 25/01/24.
//

#include "BossLevel25.h"
#include "../LevelLoader.h"
#include "../Components/ComponentGame.h"
#include "../../include/Brakeza3D.h"

BossLevel25::BossLevel25()
:
        changeStopPhase(Counter(8)),
        phase(BOSS10_PHASE_ONE)
{
    behaviorPhaseTwo = new EnemyBehaviorPatrol(
        LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 10), Z_COORDINATE_GAMEPLAY),
        LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 50), Z_COORDINATE_GAMEPLAY),
        1.0f
    );

    behaviorPhaseOne = new EnemyBehaviorPath(0.5f);
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 10), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(10, 10), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 10), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(90, 10), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 10), Z_COORDINATE_GAMEPLAY));

    behaviorPhaseFour = new EnemyBehaviorPath(0.5f);
    behaviorPhaseFour->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 10), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseFour->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(90, 90), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseFour->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 10), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseFour->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(10, 90), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseFour->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 10), Z_COORDINATE_GAMEPLAY));

    behaviorPhaseOne->start();
    setBehavior(behaviorPhaseOne);
    setEmittersEnabled(false);
}

void BossLevel25::postUpdate() {
    EnemyGhost::postUpdate();

    switch(phase) {
        case BOSS10_PHASE_THREE: {
            ComponentsManager::get()->getComponentRender()->setSelectedObject(nullptr);
            break;
        }
    }
}

void BossLevel25::onUpdate()
{
    BossEnemy::onUpdate();

    changeStopPhase.update();

    if (changeStopPhase.isFinished()) {
        changeStopPhase.setEnabled(true);
        invertPhase();
    }

    switch(phase) {
        case BOSS10_PHASE_THREE: {

            progress += SPEED_TRANSPARENCY * 2 * Brakeza3D::get()->getDeltaTime();
            progress = std::min(progress, 1.0f);
            setAlpha(1-progress);
            ComponentsManager::get()->getComponentRender()->setSelectedObject(nullptr);
            break;
        }
        default: {
            progress -= SPEED_TRANSPARENCY * 2 * Brakeza3D::get()->getDeltaTime();
            if (progress < 0) progress = 0;
            setAlpha(1-progress);
            break;
        }
    }

    if (behaviorPhaseOne->getReturnedCounter() >= 2) {
        behaviorPhaseOne->setReturnedCounter(0);
        invertPhase();
    }

    if (behaviorPhaseTwo->getReturnedCounter() >= 2) {
        behaviorPhaseTwo->setReturnedCounter(0);
        invertPhase();
    }

    if (behaviorPhaseFour->getReturnedCounter() >= 1) {
        behaviorPhaseFour->setReturnedCounter(0);
        invertPhase();
    }
}

void BossLevel25::invertPhase()
{
    if (phase == BOSS10_PHASE_ONE) {
        phase = BOSS10_PHASE_TWO;
        behaviorPhaseTwo->resetTimer();
        setBehavior(behaviorPhaseTwo);
        changeStopPhase.setEnabled(false);
        setEmittersEnabled(false);
        getWeapon()->setAvailable(true);
        return;
    }

    if (phase == BOSS10_PHASE_TWO) {
        phase = BOSS10_PHASE_THREE;
        changeStopPhase.setEnabled(true);
        setBehavior(nullptr);
        setEmittersEnabled(true);
        getWeapon()->setAvailable(false);
        return;
    }

    if (phase == BOSS10_PHASE_THREE) {
        phase = BOSS10_PHASE_FOUR;
        behaviorPhaseOne->resetTimer();
        setBehavior(behaviorPhaseFour);
        changeStopPhase.setEnabled(false);
        setEmittersEnabled(false);
        getWeapon()->setAvailable(true);
        return;
    }

    if (phase == BOSS10_PHASE_FOUR) {
        phase = BOSS10_PHASE_ONE;
        behaviorPhaseOne->resetTimer();
        setBehavior(behaviorPhaseOne);
        changeStopPhase.setEnabled(false);
        setEmittersEnabled(false);
        getWeapon()->setAvailable(true);
        return;
    }
}