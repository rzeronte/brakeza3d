
#include "../../headers/PhysicsGame/Projectile3DBody.h"

Projectile3DBody::Projectile3DBody() : fromEnemy(false) {

}

bool Projectile3DBody::isFromEnemy() {
    return fromEnemy;
};

void Projectile3DBody::setFromEnemy(bool isFromEnemy){
    fromEnemy = isFromEnemy;
}

