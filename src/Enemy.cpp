
#include "../headers/Objects/Enemy.h"

Enemy::Enemy() {
    stamina = startStamina;
}

void Enemy::damage(float damage)
{
    this->stamina -= damage;
}

bool Enemy::isHeavyDamage(float damage)
{
    if (damage > (startStamina/4)) {
        return true;
    }

    return false;
}

