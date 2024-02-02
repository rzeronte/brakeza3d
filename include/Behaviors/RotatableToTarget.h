//
// Created by eduardo on 15/07/23.
//

#ifndef BRAKEZA3D_ROTATABLETOTARGET_H
#define BRAKEZA3D_ROTATABLETOTARGET_H


#include "../Objects/Object3D.h"

class RotatableToTarget {
    Object3D *owner;

public:
    RotatableToTarget(Object3D *target, Object3D *parent, float rotationToTargetSpeed);

protected:
    void makeRotationToTarget();

    Object3D *rotationTarget;
    float rotationToTargetSpeed;
public:
    void setRotationTarget(Object3D *target);

    void setRotationToTargetSpeed(float rotationToTargetSpeed);
};


#endif //BRAKEZA3D_ROTATABLETOTARGET_H
