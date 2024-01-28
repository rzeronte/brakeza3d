//
// Created by edu on 25/01/24.
//

#include "BossLevel20.h"
#include "../LevelLoader.h"
#include "../Components/ComponentGame.h"
#include "../../include/Brakeza3D.h"

BossLevel20::BossLevel20()
:
    phase(BOSS10_PHASE_ONE),
    progress(0),
    counter(Counter(8))
{

    // up left-right
    behaviorPhaseOne = new EnemyBehaviorPath(0.3f);
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(0, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(99, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 15), Z_COORDINATE_GAMEPLAY));


    // collision left-right
    behaviorPhaseTwo = new EnemyBehaviorPath(0.5f);
    behaviorPhaseTwo->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseTwo->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(-115, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseTwo->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(-115, 90), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseTwo->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 90), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseTwo->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(215, 90), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseTwo->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(215, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseTwo->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 15), Z_COORDINATE_GAMEPLAY));

    behaviorPhaseOne->start();

    r1 = M3::getMatrixRotationForEulerAngles(0, 0, 0);
    r2 = M3::getMatrixRotationForEulerAngles(0, 0, 90);
    r3 = M3::getMatrixRotationForEulerAngles(-60, 0, 0);

    setEmittersEnabled(false);
}

void BossLevel20::onStart() {
    EnemyGhost::onStart();
    phase = BOSS10_PHASE_THREE;
    setBehavior(nullptr);
    counter.setEnabled(true);
    setEmittersEnabled(false);
    setEmitterIndexEnabled(1, true);

    progress = 0;
}

void BossLevel20::onUpdate()
{
    BossEnemy::onUpdate();
    auto gameState = ComponentsManager::get()->getComponentGame()->getGameState();

    if (gameState == EngineSetup::GAMING) {
        counter.update();
    }

    switch (phase) {
        case BOSS10_PHASE_ONE: {
            progress += 0.01f * 2 * Brakeza3D::get()->getDeltaTime();
            progress = std::min(progress, (float) 1);
            setRotation(M3::interpolateLinear(getRotation(), r1, progress));

            if (behaviorPhaseOne->getReturnedCounter() >= 1) {
                behaviorPhaseOne->setReturnedCounter(0);
                invertPhase();
            }

            break;
        }
        case BOSS10_PHASE_TWO: {
            progress += 0.01f * 2 * Brakeza3D::get()->getDeltaTime();
            progress = std::min(progress, (float) 1);
            setRotation(M3::interpolateLinear(getRotation(), r2, progress));

            if (behaviorPhaseTwo->getReturnedCounter() >= 2) {
                behaviorPhaseTwo->setReturnedCounter(0);
                invertPhase();
            }
            break;
        }
        case BOSS10_PHASE_THREE: {
            progress += 0.01f * 2 * Brakeza3D::get()->getDeltaTime();
            progress = std::min(progress, (float) 1);
            setRotation(M3::interpolateLinear(getRotation(), r3, progress));

            if (counter.isFinished()) {
                counter.setEnabled(true);
                invertPhase();
            }
            break;
        }
    }
}

void BossLevel20::invertPhase()
{
    auto gameState = ComponentsManager::get()->getComponentGame()->getGameState();

    if (gameState != EngineSetup::GAMING) {
        return;
    }

    if (phase == BOSS10_PHASE_ONE) {
        phase = BOSS10_PHASE_TWO;
        behaviorPhaseTwo->resetTimer();
        setBehavior(behaviorPhaseTwo);
        setEmittersEnabled(false);
        setEmitterIndexEnabled(0, true);
        progress = 0;
        return;
    }

    if (phase == BOSS10_PHASE_TWO) {
        phase = BOSS10_PHASE_THREE;
        setBehavior(nullptr);
        counter.setEnabled(true);
        setEmittersEnabled(false);
        setEmitterIndexEnabled(1, true);
        progress = 0;
        return;
    }

    if (phase == BOSS10_PHASE_THREE) {
        phase = BOSS10_PHASE_ONE;
        behaviorPhaseOne->resetTimer();
        setBehavior(behaviorPhaseOne);
        setEmittersEnabled(false);
        progress = 0;
        return;
    }
}

void BossLevel20::setEmitterIndexEnabled(int i, bool value) {
    projectileEmitters[i]->setEnabled(value);
    projectileEmitters[i]->setActive(value);
}
