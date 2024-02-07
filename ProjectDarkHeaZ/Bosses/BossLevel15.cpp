#include "BossLevel15.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"


BossLevel15::BossLevel15()
:
    phase(BOSS10_PHASE_ONE),
    changePhase(Counter(8)),
    progress(0)
{
    changePhase.setEnabled(true);

    const auto filter = EngineSetup::collisionGroups::ProjectileEnemy;
    const auto mask = EngineSetup::collisionGroups::Player;

    rays.push_back(new RayLight(false, this, Vertex3D(1, 0, 0), Vertex3D::zero(), 0.1, 0, PaletteColors::getEnemyLaser(), Color::orange(), filter, mask ));
    rays.push_back(new RayLight(false, this, Vertex3D(-1, 0, 0), Vertex3D::zero(), 0.1, 0, PaletteColors::getEnemyLaser(), Color::orange(), filter, mask ));
    rays.push_back(new RayLight(false, this, Vertex3D(0, 1, 0), Vertex3D::zero(), 0.1, 0, PaletteColors::getEnemyLaser(), Color::orange(), filter, mask ));
    rays.push_back(new RayLight(false, this, Vertex3D(0, -1, 0), Vertex3D::zero(), 0.1, 0, PaletteColors::getEnemyLaser(), Color::orange(), filter, mask ));

    behaviorPhaseOne = new EnemyBehaviorPath(0.5f);
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 45), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 10), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(15, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(15, 85), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(85, 85), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(85, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 15), Z_COORDINATE_GAMEPLAY));
    behaviorPhaseOne->addPoint(LevelLoader::getPositionFromScreenPoint(getScreenPoint(50, 45), Z_COORDINATE_GAMEPLAY));
}

void BossLevel15::onStart()
{
    EnemyGhost::onStart();
    //for (auto fixedLaser : fixedLasers) {
        //fixedLaser->setIntensity(0.0005f);
    //}
}

void BossLevel15::onUpdate()
{
    BossEnemy::onUpdate();

    changePhase.update();

    if (changePhase.isFinished()) {
        changePhase.setEnabled(true);
        invertPhase();
    }

    switch(phase) {
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
}

void BossLevel15::invertPhase()
{
    if (phase == BOSS10_PHASE_ONE) {
        phase = BOSS10_PHASE_TWO;
        setEmittersEnabled(false);
        setBehavior(nullptr);
        return;
    }

    if (phase == BOSS10_PHASE_TWO) {
        phase = BOSS10_PHASE_ONE;
        setEmittersEnabled(true);
        setBehavior(behaviorPhaseOne);

        return;
    }
}

void BossLevel15::updateRays()
{
    for (auto rayLight : rays) {
        rayLight->setDirection(M3::getMatrixRotationForEulerAngles(0, 0, 0.3) * rayLight->getDirection());
        rayLight->setColor(PaletteColors::getEnemyLaser());
        rayLight->setDamage(1);
        rayLight->setIntensity(0.00001f);
        rayLight->setReach(progress * 10.0f);
        rayLight->update(false);
    }
}
