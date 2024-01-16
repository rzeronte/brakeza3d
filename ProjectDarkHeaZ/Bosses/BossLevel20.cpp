#include "BossLevel20.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"


BossLevel20::BossLevel20()
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
}

void BossLevel20::onStart()
{
    EnemyGhost::onStart();
    for (auto fixedLaser : fixedLasers) {
        fixedLaser->setIntensity(0.001f);
    }
}

void BossLevel20::onUpdate()
{
    BossEnemy::onUpdate();

    changePhase.update();

    if (changePhase.isFinished()) {
        changePhase.setEnabled(true);
        invertPhase();
    }

    switch(phase) {
        case BOSS10_PHASE_ONE: {
            progress -= SPEED_BOSS_LASERS * Brakeza3D::get()->getDeltaTime();
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

void BossLevel20::invertPhase()
{
    if (phase == BOSS10_PHASE_ONE) {
        phase = BOSS10_PHASE_TWO;
        getProjectileEmitter()->setEnabled(false);
        return;
    }

    if (phase == BOSS10_PHASE_TWO) {
        phase = BOSS10_PHASE_ONE;
        getProjectileEmitter()->setEnabled(true);
        return;
    }
}

void BossLevel20::updateRays()
{
    for (auto rayLight : rays) {
        rayLight->setDirection(M3::getMatrixRotationForEulerAngles(0, 0, 0.5) * rayLight->getDirection());
        rayLight->setColor(PaletteColors::getEnemyLaser());
        rayLight->setDamage(1);
        rayLight->setEnabled(true);
        rayLight->setIntensity(0.0015f);
        rayLight->setReach(progress * 10.0f);
        rayLight->update(false);
    }
}
