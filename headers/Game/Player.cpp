
#include "../../src/Game/Player.h"
#include "../Brakeza3D.h"

Player::Player() : defaultLives(4), state(PlayerState::GAMEOVER), dead(false), stamina(100), lives(defaultLives) {

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

void Player::evalStatusMachine()
{
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
            EngineSetup::getInstance()->MENU_ACTIVE = true;
        }
    }
}

void Player::newGame()
{
    state = PlayerState::LIVE;
    setDead(false);
    setLives(defaultLives);
    setStamina(100);
    EngineSetup::getInstance()->MENU_ACTIVE = false;
    EngineSetup::getInstance()->DRAW_WEAPON = true;
    EngineSetup::getInstance()->DRAW_HUD = true;
    Brakeza3D::get()->setCameraInBSPStartPosition();
}

void Player::respawn()
{
    Brakeza3D::get()->setCameraInBSPStartPosition();
    setDead(false);
    state = PlayerState::LIVE;
    setStamina(100);
}

void Player::shoot()
{
    Projectile3DBody *projectile = new Projectile3DBody();
    projectile->setPosition(*Brakeza3D::get()->getCamera()->getPosition());
    projectile->setLabel("projectile");
    projectile->setEnabled(true);
    projectile->setTimerAnimation(Brakeza3D::get()->getTimer());
    projectile->linkTexturesTo(Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->getProjectileTemplate());
    projectile->setAnimation(0);
    projectile->makeProjectileRigidBody(1, Brakeza3D::get()->getSceneObjects(), Brakeza3D::get()->getCamera(), Brakeza3D::get()->getCollisionManager()->getDynamicsWorld(), true, Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->speed);
    projectile->getBillboard()->setDimensions(
            Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->projectileWidth,
            Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->projectileHeight
    );

    // Reduce ammo for this weapon type
    Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->ammo--;

    // Giramos antes de hacer el rigidbody, para no alterar los cálculos en la dirección
    // del impulso en el interior del makeRigidBody
    projectile->setRotation(Brakeza3D::get()->getCamera()->getRotation());

    // Si ya estábamos disparando, no interrumpimos la animación
    Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->startAction();
    Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->setWeaponAnimation(EngineSetup::getInstance()->WeaponsActions::WEAPON_ACTION_FIRE);

    Tools::playMixedSound(Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->soundFire);
}
