
#include "../headers/Objects/Enemy.h"
#include "../headers/Render/Engine.h"

extern Engine *brakeza3D;

Enemy::Enemy() : startStamina (100), stamina(0), dead(false), damage(0), range(0), speed(0)
{
    this->state = EnemyState::ENEMY_STATE_STOP;
    this->stamina = this->startStamina;
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

void Enemy::evalStatusMachine(Object3D *object, bool raycastResult, Body *body)
{
    switch(state) {
        case EnemyState::ENEMY_STATE_ATTACK:
            break;
        case EnemyState::ENEMY_STATE_DIE:
            break;
        case EnemyState::ENEMY_STATE_FOLLOW:
            break;
        case EnemyState::ENEMY_STATE_STOP:
            this->doFollowPathfinding(object, body, raycastResult);
            break;
        default:
            assert(0);
    }
}

void Enemy::doFollowPathfinding(Object3D *object, Body *body, bool raycastResult)
{
    if (!raycastResult) return;

    if (this->points.size() > 0) {
        btTransform t = body->getRigidBody()->getWorldTransform();
        btMotionState *mMotionState = body->getRigidBody()->getMotionState();

        btVector3 dest;
        Vector3D way = Vector3D(this->points[0], this->points[1]);

        Vertex3D p = way.getComponent().getNormalize().getScaled(this->getSpeed());
        Vertex3D pos = *object->getPosition() + p;

        if (!Tools::isValidVector(pos)) {
            return;
        }

        M3 newRot = M3::getFromVectors(way.getComponent().getNormalize(), EngineSetup::getInstance()->up);
        object->setRotation(newRot.getTranspose());
        pos.saveToBtVector3(&dest);

        t.setOrigin(dest);
        mMotionState->setWorldTransform(t);
        body->getRigidBody()->setWorldTransform(t);
    }
}