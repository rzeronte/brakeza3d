#include "BossLevel30.h"
#include "../Common/PaletteColors.h"
#include "../../include/Brakeza3D.h"

BossLevel30::BossLevel30()
:
    phase(BOSS10_PHASE_TWO),
    changePhase(Counter(9)),
    progress(0)
{

    const auto filter = EngineSetup::collisionGroups::ProjectileEnemy;
    const auto mask = EngineSetup::collisionGroups::Player;

    rays.push_back(new RayLight(false, this, Vertex3D(0, -1, 0), Vertex3D::zero(), 0.1, 0, PaletteColors::getEnemyLaser(), Color::orange(), filter, mask ));
    rays.push_back(new RayLight(false, this, Vertex3D(0, -1, 0), Vertex3D::zero(), 0.1, 0, PaletteColors::getEnemyLaser(), Color::orange(), filter, mask ));
    rays.push_back(new RayLight(false, this, Vertex3D(0, -1, 0), Vertex3D::zero(), 0.1, 0, PaletteColors::getEnemyLaser(), Color::orange(), filter, mask ));
    rays.push_back(new RayLight(false, this, Vertex3D(0, -1, 0), Vertex3D::zero(), 0.1, 0, PaletteColors::getEnemyLaser(), Color::orange(), filter, mask ));

    horizontal.push_back(new RayLight(false, this, Vertex3D(1, 0, 0), Vertex3D::zero(), 0.1, 0, PaletteColors::getEnemyLaser(), Color::orange(), filter, mask ));
    horizontal.push_back(new RayLight(false, this, Vertex3D(1, 0, 0), Vertex3D::zero(), 0.1, 0, PaletteColors::getEnemyLaser(), Color::orange(), filter, mask ));
    horizontal.push_back(new RayLight(false, this, Vertex3D(1, 0, 0), Vertex3D::zero(), 0.1, 0, PaletteColors::getEnemyLaser(), Color::orange(), filter, mask ));
    horizontal.push_back(new RayLight(false, this, Vertex3D(1, 0, 0), Vertex3D::zero(), 0.1, 0, PaletteColors::getEnemyLaser(), Color::orange(), filter, mask ));

    behaviorPhaseOne = new EnemyBehaviorPath(0.5f);
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(95, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(5, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 15), Z_COORDINATE_GAMEPLAY));

    setBehavior(behaviorPhaseOne);
}

void BossLevel30::onUpdate()
{
    BossEnemy::onUpdate();

    switch (phase) {
        case BOSS10_PHASE_ONE: {
            progress -= SPEED_BOSS_LASERS * 2 * Brakeza3D::get()->getDeltaTime();
            if (progress < 0) progress = 0;
            break;
        }
        case BOSS10_PHASE_TWO: {
            progress += SPEED_BOSS_LASERS * Brakeza3D::get()->getDeltaTime();
            progress = std::min(progress, (float) BOSS_LASER_LENGTH);
            break;
        }
    }

    updateRays();

    if (behaviorPhaseOne->getReturnedCounter() >= 1) {
        behaviorPhaseOne->setReturnedCounter(0);
        invertPhase();
        return;
    }

    if (phase == BOSS10_PHASE_TWO) {
        changePhase.update();
        if (changePhase.isFinished()) {
            changePhase.setEnabled(true);
            invertPhase();
            return;
        }
    }

}

void BossLevel30::invertPhase()
{
    if (phase == BOSS10_PHASE_ONE) {
        phase = BOSS10_PHASE_TWO;
        setEmittersEnabled(true);
        setBehavior(nullptr);
        changePhase.setEnabled(true);
        return;
    }

    if (phase == BOSS10_PHASE_TWO) {
        phase = BOSS10_PHASE_ONE;
        behaviorPhaseOne->resetTimer();
        setEmittersEnabled(false);
        setBehavior(behaviorPhaseOne);
        return;
    }
}


void BossLevel30::updateRays()
{
    float startX = 4;

   float startY = 4;

    for (int i = 0 ; i < rays.size() ; i++) {
        rays[i]->setColor(PaletteColors::getEnemyLaser());
        rays[i]->setDamage(1);
        rays[i]->setIntensity(0.00001f);
        rays[i]->setReach(progress * 10.0f);
        rays[i]->update(false, Vertex3D(-6 + startX * i, 8.5f, Z_COORDINATE_GAMEPLAY));
    }

    for (int i = 0 ; i < horizontal.size() ; i++) {
        horizontal[i]->setColor(PaletteColors::getStamina());
        horizontal[i]->setDamage(1);
        horizontal[i]->setIntensity(0.00001f);
        horizontal[i]->setReach(progress * 10.0f);
        horizontal[i]->update(false, Vertex3D(-12, -2 + startY * i, Z_COORDINATE_GAMEPLAY));
    }
}

