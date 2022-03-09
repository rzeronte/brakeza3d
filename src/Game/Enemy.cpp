
#include "../../include/Game/Enemy.h"

Enemy::Enemy() : startStamina(100), stamina(0), range(0), speed(0), cadence(0) {
    this->stamina = this->startStamina;
    this->counterCadence = new Counter();
    setState(EnemyState::ENEMY_STATE_STOP);
}

void Enemy::takeDamage(float damageTaken) {
    this->stamina -= damageTaken;
}

float Enemy::getRange() const {
    return range;
}

void Enemy::setRange(float range) {
    Enemy::range = range;
}

float Enemy::getSpeed() const {
    return speed;
}

void Enemy::setSpeed(float speed) {
    Enemy::speed = speed;
}

float Enemy::getCadence() const {
    return cadence;
}

void Enemy::setCadence(float cadence) {
    Enemy::cadence = cadence;
    this->counterCadence->setStep(cadence);
}

void Enemy::evalStatusMachine() {
    switch (state) {
        case EnemyState::ENEMY_STATE_ATTACK:
            break;
        case EnemyState::ENEMY_STATE_FOLLOW:
            break;
        case EnemyState::ENEMY_STATE_INJURIED:
            break;
        case EnemyState::ENEMY_STATE_STOP:
            break;
        case EnemyState::ENEMY_STATE_DIE:
            break;
        default:
            assert(0);
    }
}

EnemyState Enemy::getState() const {
    return state;
}

void Enemy::setState(EnemyState state) {
    Enemy::state = state;
}
