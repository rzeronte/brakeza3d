
#include "../../headers/Game/Projectile3DBody.h"

Projectile3DBody::Projectile3DBody() : fromEnemy(false) {

}

bool Projectile3DBody::isFromEnemy() {
    return fromEnemy;
};

void Projectile3DBody::setFromEnemy(bool isFromEnemy){
    fromEnemy = isFromEnemy;
}

float Projectile3DBody::getDamage() const {
    return damage;
}

void Projectile3DBody::setDamage(float damage) {
    Projectile3DBody::damage = damage;
}

float Projectile3DBody::getDamageRadius() const {
    return damageRadius;
}

void Projectile3DBody::setDamageRadius(float damageRadius) {
    Projectile3DBody::damageRadius = damageRadius;
}

