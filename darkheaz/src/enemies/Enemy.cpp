
#include "Enemy.h"
#include "../../../include/Brakeza3D.h"

Enemy::Enemy()
:
    stuck(false),
    state(EnemyState::ENEMY_STATE_STOP),
    weapon(nullptr),
    range(0),
    soundChannel(-1),
    rewards(false)
{
}

EnemyState Enemy::getState() const {
    return state;
}

void Enemy::setState(EnemyState value) {
    Enemy::state = value;
}

void Enemy::setWeapon(Weapon *weaponType) {
    Enemy::weapon = weaponType;
}

Weapon *Enemy::getWeapon() const {
    return weapon;
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

const std::string &Enemy::getName() const {
    return name;
}

void Enemy::setName(const std::string &name) {
    Enemy::name = name;
}

Image *Enemy::getAvatarHud() const {
    return avatarHud;
}

void Enemy::setAvatarHud(Image *avatarHud) {
    Enemy::avatarHud = avatarHud;
}
