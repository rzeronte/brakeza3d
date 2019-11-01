
#include "../../headers/Game/Enemy.h"
#include "../../headers/Render/Engine.h"

Enemy::Enemy() : startStamina (100), stamina(0), dead(false), damage(0), range(0), speed(0), cadence(0), acumulatedTime(0), lastTicks(0)
{
    this->stamina = this->startStamina;
    this->cadenceTimer.stop();
}

void Enemy::takeDamage(float damageTaken)
{
    this->stamina -= damageTaken;
}

bool Enemy::isTakeHeavyDamage(float damageTaken)
{
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
}

void Enemy::startFire()
{
    this->cadenceTimer.start();
}

void Enemy::endFire()
{
    this->cadenceTimer.stop();
    this->acumulatedTime = 0;
    this->lastTicks = 0;
}

void Enemy::updateCadenceTimer()
{
    if (!this->cadenceTimer.isStarted()) return;

    float deltatime = this->cadenceTimer.getTicks() - this->lastTicks;
    this->lastTicks = this->cadenceTimer.getTicks();

    this->acumulatedTime += deltatime / 1000;

    if  (this->acumulatedTime >= this->cadence ) {
        this->endFire();
    }
}

bool Enemy::isCadenceInProgress()
{
    return this->cadenceTimer.isStarted();
}