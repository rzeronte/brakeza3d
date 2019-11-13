
#include "../../src/Game/Player.h"
#include "../Brakeza3D.h"
#include "../Render/EngineBuffers.h"

Player::Player() : defaultLives(5), state(PlayerState::GAMEOVER), dead(false), stamina(100), lives(defaultLives), tookDamage(false)
{
    this->counterStep       = new Counter(0.40);
    this->counterTakeDamage = new Counter(0.10);
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

void Player::setDead(bool dead)
{
    if (this->dead != dead && dead) {
        int rndPlayerDead = Tools::random(1, 6);
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("playerDead" + std::to_string(rndPlayerDead)), EngineSetup::SoundChannels::SND_PLAYER);
    }

    this->dead = dead;
}

void Player::evalStatusMachine()
{
    this->tookDamage = false;

    this->counterStep->update();
    this->counterTakeDamage->update();

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
    this->tookDamage = true;

    if (counterTakeDamage->isFinished()) {
        int rndPlayerPain = Tools::random(1, 4);
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("playerPain" + std::to_string(rndPlayerPain)), EngineSetup::SoundChannels::SND_PLAYER);
        Logging::getInstance()->Log("Sound player damage");
    }

    if (stamina <= 0) {
        state = PlayerState::DEAD;
        setDead( true );
        lives--;

        if (lives <= 0) {
            state = PlayerState::GAMEOVER;
            EngineSetup::getInstance()->MENU_ACTIVE = true;
        }
    }
}

void Player::newGame()
{
    setLives(defaultLives);

    this->respawn();
    this->respawnNPCS();

    EngineSetup::getInstance()->MENU_ACTIVE = false;
    EngineSetup::getInstance()->DRAW_WEAPON = true;
    EngineSetup::getInstance()->DRAW_HUD    = true;
}

void Player::respawn()
{
    Brakeza3D::get()->setCameraInBSPStartPosition();
    setDead(false);
    state = PlayerState::LIVE;
    setStamina(100);
    Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("startGame"), EngineSetup::SoundChannels::SND_ENVIRONMENT);
}

void Player::shoot()
{
    Projectile3DBody *projectile = new Projectile3DBody();
    projectile->setPosition(*Brakeza3D::get()->getCamera()->getPosition());
    projectile->setLabel("projectile");
    projectile->setEnabled(true);
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

    Tools::playMixedSound(Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->soundFire, EngineSetup::SoundChannels::SND_PLAYER);
}


void Player::respawnNPCS()
{
    std::vector<Object3D *>::iterator it;
    for (it = Brakeza3D::get()->getSceneObjects().begin(); it != Brakeza3D::get()->getSceneObjects().end(); it++) {
        Object3D *object = *(it);
        auto *enemy = dynamic_cast<NPCEnemyBody *> (object);

        if (enemy != NULL) {
            enemy->respawn();
        }
    }
}