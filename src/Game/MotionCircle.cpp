//
// Created by eduardo on 19/3/22.
//

#include "../../include/Game/MotionCircle.h"

MotionCircle::MotionCircle(const Vertex3D &center, float speed, float radius) : speed(speed), center(center),
                                                                                radius(radius) {
    this->center = center + Vertex3D(0, radius, 0);
}


void MotionCircle::onUpdate(Vertex3D &position) {
    Motion::onUpdate(position);

    center = (M3::getMatrixRotationForEulerAngles(0, 0, speed) * center);
    position = center;
}
