//
// Created by eduardo on 19/3/22.
//

#include "../enemies/EnemyBehaviorCircle.h"

EnemyBehaviorCircle::EnemyBehaviorCircle(const Vertex3D &center, float speed, float radius) : speed(speed), center(center),
                                                                                              radius(radius) {
    this->center = center;
    this->offset = Vertex3D(0, radius, 0);
}


void EnemyBehaviorCircle::onUpdate(Vertex3D &position) {
    EnemyBehavior::onUpdate(position);

    offset = M3::getMatrixRotationForEulerAngles(0, 0, speed) * offset;
    position = center + offset;
}

EnemyBehavior *EnemyBehaviorCircle::clone() {
    return new EnemyBehaviorCircle(
        this->center,
        this->speed,
        this->radius
    );
}
