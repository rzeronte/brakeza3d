//
// Created by eduardo on 19/3/22.
//

#include "../../include/Game/MotionLinearRect.h"

MotionLinearRect::MotionLinearRect(const Vector3D &direction) : direction(direction) {
    setSpeed(MOTIONLINEARRECT_DEFAULT_SPEED);
}

void MotionLinearRect::onUpdate(Vertex3D &position) {
    Motion::onUpdate(position);
    position = direction.getComponent().getNormalize().getScaled(getExecutionTime() * speed);
}

void MotionLinearRect::setDirection(const Vector3D &direction) {
    MotionLinearRect::direction = direction;
}

float MotionLinearRect::getSpeed() const {
    return speed;
}

void MotionLinearRect::setSpeed(float speed) {
    MotionLinearRect::speed = speed;
}
