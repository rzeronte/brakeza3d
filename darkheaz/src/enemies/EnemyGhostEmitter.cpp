//
// Created by eduardo on 20/3/22.
//

#include "EnemyGhostEmitter.h"
#include "../../../include/Brakeza3D.h"

EnemyGhostEmitter::EnemyGhostEmitter(EnemyGhost *object, float step): object(object)
{
    counter.setStep(step);
    counter.setEnabled(false);
    object->setEnabled(true);
    if (object->getWeapon() != nullptr) {
        object->getWeapon()->counterCadence->setEnabled(false);
    }
    object->setStencilBufferEnabled(false);
    object->setAlphaEnabled(true);
    Brakeza3D::get()->addObject3D(object, object->getLabel());
}

void EnemyGhostEmitter::onUpdate()
{
    if (object == nullptr) {
        return;
    }

    if (object->isRemoved()) {
        counter.setEnabled(false);
        this->setRemoved(true);
        return;
    }

    Object3D::onUpdate();

    if (!counter.isEnabled()) return;

    counter.update();

    const float increase = (Brakeza3D::get()->getDeltaTime() * 255) / counter.getStep();
    object->setAlpha(std::fmin(object->getAlpha() + increase, 255));

    if (counter.isFinished()) {
        if (object->getWeapon() != nullptr) {
            object->getWeapon()->counterCadence->setEnabled(true);
        }
        object->setStencilBufferEnabled(true);
        object->setAlphaEnabled(false);
        object->setAlpha(255);
        counter.setEnabled(false);
        this->setRemoved(true);
        return;
    }
}

void EnemyGhostEmitter::startCounter()
{
    if (this->isRemoved()) {
        return;
    }

    this->counter.setEnabled(true);
}

