
#include "Enemy.h"
#include "../../../include/Brakeza3D.h"

Enemy::Enemy() : startStamina(100), stamina(0), range(0), rewards(false)
{
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

void Enemy::setRange(float value) {
    Enemy::range = value;
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

void Enemy::setStamina(float value) {
    Enemy::stamina = value;
}

float Enemy::getStartStamina() const {
    return startStamina;
}

void Enemy::setStartStamina(float value) {
    Enemy::startStamina = value;
}

int Enemy::getSoundChannel() const {
    return soundChannel;
}

void Enemy::setSoundChannel(int value) {
    Enemy::soundChannel = value;
}

Enemy::~Enemy()
{
    delete weapon;
}

bool Enemy::isRewards() const {
    return rewards;
}

void Enemy::setRewards(bool rewards) {
    Enemy::rewards = rewards;
}

bool Enemy::isStucked() const
{
    return stucked;
}

void Enemy::setStucked(bool value)
{
    Enemy::stucked = value;
}
