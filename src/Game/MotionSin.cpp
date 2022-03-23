//
// Created by eduardo on 20/3/22.
//

#include "../../include/Game/MotionSin.h"

void MotionSin::onUpdate(Vertex3D &position) {
    Motion::onUpdate(position);
    position.x = position.x + sin(getExecutionTime()) * speed;
}

float MotionSin::getSpeed() const {
    return speed;
}

void MotionSin::setSpeed(float speed) {
    MotionSin::speed = speed;
}

MotionSin::MotionSin(float speed) : speed(speed) {}
