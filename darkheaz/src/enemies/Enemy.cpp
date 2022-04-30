
#include "Enemy.h"
#include "../../../include/Brakeza3D.h"

Enemy::Enemy() : startStamina(100), stamina(0), range(0), speed(0) {
    this->stamina = this->startStamina;
    setState(EnemyState::ENEMY_STATE_STOP);
    weapon = nullptr;
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

void Enemy::setWeapon(Weapon *weaponType) {
    Enemy::weapon = weaponType;
}

Weapon *Enemy::getWeapon() const {
    return weapon;
}

void Enemy::setStamina(float stamina) {
    Enemy::stamina = stamina;
}

float Enemy::getStartStamina() const {
    return startStamina;
}

void Enemy::setStartStamina(float startStamina) {
    Enemy::startStamina = startStamina;
}

int Enemy::getSoundChannel() const {
    return soundChannel;
}

void Enemy::setSoundChannel(int soundChannel) {
    Enemy::soundChannel = soundChannel;
}

Enemy::~Enemy()
{
    delete weapon;
}
