
#include "../../include/Game/Enemy.h"

Enemy::Enemy() : startStamina(100), stamina(0), range(0), speed(0) {
    this->stamina = this->startStamina;
    setState(EnemyState::ENEMY_STATE_STOP);
    weaponType = new WeaponType("EnemyWeapon");
}

void Enemy::takeDamage(float damageTaken) {
    this->stamina -= damageTaken;
    if (this->stamina <= 0) {
        setState(EnemyState::ENEMY_STATE_DIE);
    }
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

EnemyState Enemy::getState() const {
    return state;
}

void Enemy::setState(EnemyState state) {
    Enemy::state = state;
}

float Enemy::getStamina() const {
    return stamina;
}

void Enemy::setWeaponType(WeaponType *weaponType) {
    Enemy::weaponType = weaponType;
}

void Enemy::shoot(Object3D *parent, Vertex3D direction, Vertex3D projectilePosition) {
    weaponType->shoot(parent, projectilePosition, direction);
}
