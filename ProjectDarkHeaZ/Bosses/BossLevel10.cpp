#include "BossLevel10.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

BossLevel10::BossLevel10()
:
    phase(BOSS10_PHASE_ONE)
{
    auto game = ComponentsManager::get()->getComponentGame();
    auto levelLoader = game->getLevelLoader();

    behaviorPhaseTwo = new EnemyBehaviorPatrol(
        levelLoader->getPositionFromScreenPoint(getScreenPoint(50, 5), Z_COORDINATE_GAMEPLAY),
        levelLoader->getPositionFromScreenPoint(getScreenPoint(50, 50), Z_COORDINATE_GAMEPLAY),
        1.0f
    );

    behaviorPhaseOne = new EnemyBehaviorPath(0.5f);
    behaviorPhaseOne->addPoint(levelLoader->getPositionFromScreenPoint(getScreenPoint(50, 5), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(levelLoader->getPositionFromScreenPoint(getScreenPoint(5, 5), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(levelLoader->getPositionFromScreenPoint(getScreenPoint(50, 5), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(levelLoader->getPositionFromScreenPoint(getScreenPoint(90, 5), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(levelLoader->getPositionFromScreenPoint(getScreenPoint(50, 5), Z_COORDINATE_GAMEPLAY));
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

Point2D BossLevel10::getScreenPoint(int x, int y)
{
    const int xFrom = x * EngineSetup::get()->screenWidth / 100;
    const int yFrom = y * EngineSetup::get()->screenHeight / 100;

    return Point2D(xFrom, yFrom);
}
