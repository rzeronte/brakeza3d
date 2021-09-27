
#include "../../headers/Game/Enemy.h"

Enemy::Enemy() : startStamina(100), stamina(0), dead(false), damage(0), range(0), speed(0), cadence(0) {
    this->stamina = this->startStamina;
    this->counterCadence = new Counter();
}

void Enemy::takeDamage(float damageTaken) {
    this->stamina -= damageTaken;
}

bool Enemy::isTakeHeavyDamage(float damageTaken) const {
    if (damageTaken > (startStamina / 4)) {
        return true;
    }

    return false;
}

float Enemy::getDamage() const {
    return damage;
}

void Enemy::setDamage(float damage) {
    Enemy::damage = damage;
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

const std::string &Enemy::getClassname() const {
    return classname;
}

void Enemy::setClassname(const std::string &classname) {
    Enemy::classname = classname;
    Enemy::classname = classname;
}

bool Enemy::isDead() const {
    return dead;
}

void Enemy::setDead(bool dead) {
    Enemy::dead = dead;
}

float Enemy::getCadence() const {
    return cadence;
}

void Enemy::setCadence(float cadence) {
    Enemy::cadence = cadence;
    this->counterCadence->setStep(cadence);
}

const Vertex3D &Enemy::getRespawnPosition() const {
    return respawnPosition;
}

void Enemy::setRespawnPosition(const Vertex3D &respawnPosition) {
    Enemy::respawnPosition = respawnPosition;
}

const M3 &Enemy::getRespawnRotation() const {
    return respawnRotation;
}

void Enemy::setRespawnRotation(const M3 &respawnRotation) {
    Enemy::respawnRotation = respawnRotation;
}
