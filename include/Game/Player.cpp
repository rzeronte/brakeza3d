
#include "../../src/Game/Player.h"
#include "../Brakeza3D.h"
#include "../Render/Transforms.h"

Player::Player() : state(PlayerState::GAMEOVER), dead(false),
                   stamina(INITIAL_STAMINA), lives(INITIAL_LIVES),
                   stopped(false), power(INITIAL_POWER), friction(INITIAL_FRICTION), maxVelocity(INITIAL_MAX_VELOCITY) {

    autoScrollSpeed = Vertex3D(0, -1.0, 0);

}

int Player::getStamina() const {
    return stamina;
}

void Player::setStamina(int stamina) {
    Player::stamina = stamina;
}

void Player::setLives(int lives) {
    Player::lives = lives;
}

bool Player::isDead() const {
    return dead;
}

void Player::setDead(bool dead) {
    this->dead = dead;
}

void Player::evalStatusMachine() {

    switch (state) {
        case PlayerState::LIVE:
            break;
        case PlayerState::DEAD:
            break;
    }
}

void Player::takeDamage(float dmg) {
    if (dead) return;

    this->stamina -= dmg;

    if (stamina <= 0) {
        state = PlayerState::DEAD;
        setDead(true);
        lives--;

        if (lives <= 0) {
            state = PlayerState::GAMEOVER;
            EngineSetup::get()->MENU_ACTIVE = true;
        }
    }
}

void Player::newGame() {
    setLives(INITIAL_LIVES);
    //SDL_SetRelativeMouseMode(SDL_TRUE);
    EngineSetup::get()->MENU_ACTIVE = false;
    EngineSetup::get()->DRAW_HUD = true;

    this->state = PlayerState::LIVE;
}

void Player::respawn() {
    if (Tools::isValidVector(
            Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera()->getPosition())) {
    }

    setDead(false);
    state = PlayerState::LIVE;
    setStamina(INITIAL_STAMINA);
}

void Player::shoot() {
    Brakeza3D::get()->getComponentsManager()->getComponentWeapons()->shoot();
}

void Player::jump() {
}

void Player::reload() {
    Brakeza3D::get()->getComponentsManager()->getComponentWeapons()->reload();
}

void Player::getAid(float aid) {
    this->stamina = stamina + aid;

    if (stamina > (float) INITIAL_STAMINA) {
        this->stamina = (float) INITIAL_STAMINA;
    }
}

void Player::onUpdate() {
    Mesh3D::onUpdate();


    applyFriction();

    float rightRotation = velocity * AxisForward() * Brakeza3D::get()->getDeltaTime();
    M3 r = M3::getMatrixRotationForEulerAngles(-rightRotation, 0, 0);

    setRotation( getRotation() * r );

    checkCollidingWithAutoScroll();

    setPosition(getPosition() + this->velocity );
}

Vertex3D Player::getVelocity() {
    return this->velocity;
}

void Player::applyFriction() {
    if (Tools::isZeroVector(this->velocity)) {
        return;
    }

    velocity = velocity + velocity.getInverse().getScaled(Brakeza3D::get()->getDeltaTime() * friction);
}

void Player::setVelocity(Vertex3D v) {
    this->velocity = v;
}

void Player::checkCollidingWithAutoScroll() {
    auto *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    Vertex3D o;
    Vertex3D destinyPoint = getPosition() + velocity;
    Transforms::cameraSpace(o, destinyPoint, camera);
    o = Transforms::PerspectiveNDCSpace(o, camera->frustum);

    if (o.y > 1) {
        setPosition(getPosition() + this->autoScrollSpeed );
        if (velocity.y > 0) {
            velocity.y = -1;
        }
    }
}
