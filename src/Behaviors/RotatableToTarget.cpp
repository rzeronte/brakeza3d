//
// Created by eduardo on 15/07/23.
//

#include "../../include/Behaviors/RotatableToTarget.h"
#include "../../include/EngineSetup.h"


RotatableToTarget::RotatableToTarget(Object3D *target, Object3D *parent, float rotationToTargetSpeed)
:
    owner(parent),
    rotationToTargetSpeed(rotationToTargetSpeed),
    rotationTarget(target)
{
}

void RotatableToTarget::makeRotationToTarget()
{
    if (owner->isRemoved()) return;

    if (rotationTarget != owner && rotationTarget != nullptr) {
        Vector3D way(rotationTarget->getPosition(), owner->getPosition());
        M3 newRot = M3::getFromVectors(EngineSetup::get()->forward,way.getComponent().getNormalize());
        Vertex3D b = owner->getRotation() * EngineSetup::get()->up;

        const float theta = newRot.X() * b;
        M3 rotation = M3::getMatrixRotationForEulerAngles(0, 0, theta * rotationToTargetSpeed);
        owner->setRotation(owner->getRotation() * rotation.getTranspose());
    }

    if (rotationTarget == nullptr) {
        Vertex3D end = owner->getPosition() + Vertex3D(0, 1, 0);
        Vector3D way(owner->getPosition(), end);

        M3 newRot = M3::getFromVectors(EngineSetup::get()->forward, way.getComponent().getNormalize());
        Vertex3D b = owner->getRotation() * EngineSetup::get()->up;

        const float theta = newRot.X() * b;
        M3 rotation = M3::getMatrixRotationForEulerAngles(0, 0, theta * rotationToTargetSpeed);
        owner->setRotation(owner->getRotation() * rotation.getTranspose());
    }
}

void RotatableToTarget::setRotationTarget(Object3D *target)
{
    RotatableToTarget::rotationTarget = target;
}
