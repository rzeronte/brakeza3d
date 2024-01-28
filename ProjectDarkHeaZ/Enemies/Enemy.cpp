
#include "Enemy.h"
#include "../../include/Brakeza3D.h"

Enemy::Enemy()
:
    light(LightPoint3D::base()),
    counterLight(Counter(0.1)),
    stuck(false),
    state(EnemyState::ENEMY_STATE_STOP),
    weapon(nullptr),
    range(0),
    soundChannel(-1),
    rewards(false),
    lightPositionOffset(Vertex3D(0, 0, -2))
{
    light->setRotation(180, 0, 0);
    light->setMultiScene(true);
    light->setEnabled(false);
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

    light->setRemoved(true);
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

LightPoint3D *Enemy::getLight() const {
    return light;
}

std::vector<AmmoProjectileBodyEmitter *> *Enemy::getProjectileEmitters() {
    return &projectileEmitters;
}

void Enemy::addProjectileEmitter(AmmoProjectileBodyEmitter *emitter)
{
    projectileEmitters.push_back(emitter);
}

void Enemy::updateEmitters()
{
    for (auto e: projectileEmitters) {
        e->onUpdate();
    }
}

void Enemy::setEmittersEnabled(bool value) {
    for (auto e: projectileEmitters) {
        e->setEnabled(value);
        e->setActive(value);
    }
}