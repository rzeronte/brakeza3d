//
// Created by eduardo on 20/3/22.
//

#include "../../include/Game/EnemyGhostRespawner.h"
#include "../../include/Brakeza3D.h"

EnemyGhostRespawner::EnemyGhostRespawner(EnemyGhost *object, float step)
{
    this->object = object;
    counter.setStep(step);
    counter.setEnabled(true);
    object->setEnabled(true);
    object->setAlphaEnabled(true);
    Brakeza3D::get()->addObject3D(object, object->getLabel());
}

void EnemyGhostRespawner::onUpdate()
{
    Object3D::onUpdate();

    counter.update();

    const float increase = (Brakeza3D::get()->getDeltaTime() * 255) / counter.getStep();
    object->setAlpha(object->getAlpha() + increase);

      if (counter.isFinished()) {
        object->getWeaponType()->counterCadence->setEnabled(true);
        object->setAlphaEnabled(false);
        counter.setEnabled(false);
        this->setRemoved(true);
        return;
    }
}

