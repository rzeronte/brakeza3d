
#include <BulletCollision/CollisionShapes/btConvexShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include "../../src/Game/Player.h"
#include "../Brakeza3D.h"

Player::Player() : state(PlayerState::LIVE), dead(false), stamina(100), lives(4) {

}

void Player::evalStatusMachine() {
    switch (state) {
        case PlayerState::LIVE:
            break;
        case PlayerState::DEAD:
            break;
    }
}

void Player::takeDamage(float dmg)
{
    if (dead) return;

    this->stamina -= dmg;

    if (stamina <= 0) {
        state = PlayerState::DEAD;
        dead  = true;
        lives--;

        if (lives <= 0) {
            state = PlayerState::GAMEOVER;
        }
    }
}

int Player::getStamina() const {
    return stamina;
}

void Player::setStamina(int stamina) {
    Player::stamina = stamina;
}

int Player::getLives() const {
    return lives;
}

void Player::setLives(int lives) {
    Player::lives = lives;
}

bool Player::isDead() const {
    return dead;
}

void Player::setDead(bool dead) {
    Player::dead = dead;
}
