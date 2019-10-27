
#include "Player3D.h"

Player3D::Player3D() : dead(false), stamina(100), lives(4) {

}

void Player3D::takeDamage(float dmg) {
    if (dead) return;

    this->stamina -= dmg;

    if (stamina <= 0) {
        this->dead = true;
    }
}
