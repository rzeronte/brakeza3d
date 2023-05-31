
#include "Enemy.h"
#include "../../../include/Brakeza3D.h"

Enemy::Enemy()
:
    stuck(false),
    state(EnemyState::ENEMY_STATE_STOP),
    weapon(nullptr),
    startStamina(100),
    stamina(100),
    range(0),
    soundChannel(-1),
    rewards(false)
{
}

void Enemy::takeDamage(float damageTaken) {
    this->stamina -= damageTaken;
    if (this->stamina <= 0) {
        setState(EnemyState::ENEMY_STATE_DIE);
    }
}

EnemyState Enemy::getState() const {
    return state;
}

void Enemy::setState(EnemyState value) {
    Enemy::state = value;
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
    delete avatar;
}

bool Enemy::isRewards() const {
    return rewards;
}

void Enemy::setRewards(bool value) {
    Enemy::rewards = value;
}

bool Enemy::isStuck() const
{
    return stuck;
}

void Enemy::setStuck(bool value)
{
    Enemy::stuck = value;
}

void Enemy::setAvatar(Image *avatar) {
    Enemy::avatar = avatar;
}

Image *Enemy::getAvatar(){
    return avatar;
}
