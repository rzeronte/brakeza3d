//
// Created by eduardo on 20/3/22.
//

#include "../../include/Game/EnemyBehaviorFollow.h"
EnemyBehaviorFollow::EnemyBehaviorFollow(Object3D *target, float speed, float separation)
{
    this->target = target;
    this->speed = speed;
    this->separation = separation;
}

void EnemyBehaviorFollow::onUpdate(Vertex3D &position)
{
    EnemyBehavior::onUpdate(position);
    Vector3D direction(position, target->getPosition());

    if (direction.getComponent().getModule() > this->separation) {
        position = position + direction.getComponent().getNormalize().getScaled(speed);
    }
}

float EnemyBehaviorFollow::getSpeed() const {
    return speed;
}

void EnemyBehaviorFollow::setSpeed(float speed) {
    EnemyBehaviorFollow::speed = speed;
}

float EnemyBehaviorFollow::getSeparation() const {
    return separation;
}

void EnemyBehaviorFollow::setSeparation(float separation) {
    EnemyBehaviorFollow::separation = separation;
}

Object3D *EnemyBehaviorFollow::getTarget() const {
    return target;
}

void EnemyBehaviorFollow::setTarget(Object3D *target) {
    EnemyBehaviorFollow::target = target;
}

