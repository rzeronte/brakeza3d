#include "BossLevel10.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

BossLevel10::BossLevel10()
:
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
    behaviorPhaseOne->start();
    setBehavior(behaviorPhaseOne);
}

void BossLevel10::onUpdate()
{
    BossEnemy::onUpdate();

    if (behaviorPhaseOne->getReturnedCounter() >= 2) {
        behaviorPhaseOne->setReturnedCounter(0);
        invertPhase();
    }

    if (behaviorPhaseTwo->getReturnedCounter() >= 2) {
        behaviorPhaseTwo->setReturnedCounter(0);
        invertPhase();
    }
}

void BossLevel10::invertPhase()
{
    if (phase == BOSS10_PHASE_ONE) {
        phase = BOSS10_PHASE_TWO;
        behaviorPhaseTwo->resetTimer();
        setBehavior(behaviorPhaseTwo);
        return;
    }

    if (phase == BOSS10_PHASE_TWO) {
        phase = BOSS10_PHASE_ONE;
        behaviorPhaseOne->resetTimer();
        setBehavior(behaviorPhaseOne);
        return;
    }
}
