#include "../../headers/Game/GameInputController.h"
#include "../../headers/Brakeza3D.h"
#include "../../headers/EngineBuffers.h"

GameInputController::GameInputController(Player *player) : player(player)
{
}
void GameInputController::handleMouse(SDL_Event *event)
{
    if ( player->isDead() && !EngineSetup::getInstance()->MENU_ACTIVE ) {
        if (this->MousePressed) {
            player->respawn();
        }

        return;
    }

    InputController::handleMouse(event);

}

void GameInputController::handleMovingCamera(SDL_Event *event, bool &end)
{
    if (EngineSetup::getInstance()->MENU_ACTIVE) return;

    if ( !player->isDead() ) {
        InputController::handleMovingCamera(event, end);
    }

    // jump
    if (keyboard[SDL_SCANCODE_SPACE]) {
        if (Brakeza3D::get()->getBSP()->isCurrentLeafLiquid()) {
            this->jump( false, -10, false);
        } else {
            this->jump( true, EngineSetup::getInstance()->JUMP_FORCE.y, true );
        }
    }

    // step sounds
    if (keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_S] || keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_D]) {
        if (player->counterStep->isFinished()) {

            //player->counterStep->reset();
            player->counterStep->setEnabled(true);

            if (!Mix_Playing(EngineSetup::SoundChannels::SND_PLAYER_STEPS)) {
                int rndStep = Tools::random(1, 6);
                Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("playerStep" + std::to_string(rndStep)), EngineSetup::SoundChannels::SND_PLAYER_STEPS, 0);
            }
        }
    }

}

void GameInputController::handleInGameInput(SDL_Event *event, bool &end)
{
    if (keyboard[SDL_SCANCODE_ESCAPE] && event->type == SDL_KEYDOWN && player->state != PlayerState::GAMEOVER) {
        EngineSetup::getInstance()->MENU_ACTIVE = !EngineSetup::getInstance()->MENU_ACTIVE;

        if (!EngineSetup::getInstance()->MENU_ACTIVE) {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            Mix_HaltMusic();
            Mix_PlayMusic( EngineBuffers::getInstance()->soundPackage->getMusicByLabel("musicBaseLevel0"), -1 );
            EngineSetup::getInstance()->DRAW_WEAPON = true;
            EngineSetup::getInstance()->DRAW_HUD    = true;
        } else {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_WarpMouseInWindow(Brakeza3D::get()->window, EngineSetup::getInstance()->screenWidth/2, EngineSetup::getInstance()->screenHeight/2);
            Mix_HaltMusic();
            Mix_PlayMusic( EngineBuffers::getInstance()->soundPackage->getMusicByLabel("musicMainMenu"), -1 );
            EngineSetup::getInstance()->DRAW_WEAPON = false;
            EngineSetup::getInstance()->DRAW_HUD    = false;
        }

        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU, 0);
    }

    handleMenuKeyboard(end);

    if (EngineSetup::getInstance()->MENU_ACTIVE || EngineSetup::getInstance()->SPLASHING || EngineSetup::getInstance()->LOADING) return;

    this->handleZoom( event );
    this->handleCrouch( event );
    this->handleFire( event );
    this->handleSniper( event );
    this->handleWeaponSelector();

}

void GameInputController::handleMenuKeyboard(bool &end)
{
    if (keyboard[SDL_SCANCODE_DOWN]) {
        if (EngineSetup::getInstance()->MENU_ACTIVE) {
            if (Brakeza3D::get()->getMenuManager()->currentOption + 1 < Brakeza3D::get()->getMenuManager()->numOptions) {
                Brakeza3D::get()->getMenuManager()->currentOption++;
                Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("soundMenuClick"), EngineSetup::SoundChannels::SND_MENU, 0);
            }
        }
    }

    if (keyboard[SDL_SCANCODE_UP]) {
        if (EngineSetup::getInstance()->MENU_ACTIVE) {
            if (Brakeza3D::get()->getMenuManager()->currentOption > 0) {
                Brakeza3D::get()->getMenuManager()->currentOption--;
                Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("soundMenuClick"), EngineSetup::SoundChannels::SND_MENU, 0);
            }
        }
    }

    if (keyboard[SDL_SCANCODE_RETURN]) {
        if (EngineSetup::getInstance()->MENU_ACTIVE && Brakeza3D::get()->getMenuManager()->options[Brakeza3D::get()->getMenuManager()->currentOption]->getAction() == MenuManager::MNU_EXIT) {
            end = true;
            return;
        }

        if (EngineSetup::getInstance()->MENU_ACTIVE && player->state == PlayerState::GAMEOVER && Brakeza3D::get()->getMenuManager()->options[Brakeza3D::get()->getMenuManager()->currentOption]->getAction() == MenuManager::MNU_NEW_GAME) {
            Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU, 0);

            player->newGame();
        }

        if (EngineSetup::getInstance()->MENU_ACTIVE && player->state != PlayerState::GAMEOVER && Brakeza3D::get()->getMenuManager()->options[Brakeza3D::get()->getMenuManager()->currentOption]->getAction() == MenuManager::MNU_NEW_GAME) {

            Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU, 0);

            Mix_HaltMusic();
            Mix_PlayMusic( EngineBuffers::getInstance()->soundPackage->getMusicByLabel("musicBaseLevel0"), -1 );
            EngineSetup::getInstance()->DRAW_WEAPON = true;
            EngineSetup::getInstance()->DRAW_HUD    = true;

            EngineSetup::getInstance()->MENU_ACTIVE = !EngineSetup::getInstance()->MENU_ACTIVE;
        }
    }
}

void GameInputController::jump(bool checkOnGrount, float YForce, bool soundJump)
{
    if (checkOnGrount && !Brakeza3D::get()->getCamera()->kinematicController->onGround()) {
        return;
    }

    if (soundJump) {
        int rndJump = Tools::random(1, 4);
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("playerJump" + std::to_string(rndJump)), EngineSetup::SoundChannels::SND_PLAYER, 0);
    }

    Brakeza3D::get()->getCamera()->kinematicController->jump(btVector3(0, YForce, 0));
}

void GameInputController::handleCrouch(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_TAB ) {

        btTransform trans;
        btConvexShape *capsule;
        float radius, height;
        bool flag = false;
        float offsetCrouch = 2.25;

        if (event->type == SDL_KEYDOWN && !player->isCrouch()) {
            height = 1.0f;
            radius = 1.0f;

            btVector3 pos = Brakeza3D::get()->getCamera()->kinematicController->getGhostObject()->getWorldTransform().getOrigin();
            pos.setY(pos.y() + offsetCrouch);
            trans.setIdentity();
            trans.setOrigin(pos);

            Brakeza3D::get()->getCollisionManager()->getDynamicsWorld()->removeCollisionObject(Brakeza3D::get()->getCamera()->m_ghostObject);
            Brakeza3D::get()->getCollisionManager()->getDynamicsWorld()->removeAction(Brakeza3D::get()->getCamera()->kinematicController);

            capsule = new btCapsuleShapeZ( radius, height );

            player->setCrouch(true);
            flag = true;
        }

        if (event->type == SDL_KEYUP && player->isCrouch()) {
            height = 4.5f;
            radius = 1.5f;

            btVector3 pos = Brakeza3D::get()->getCamera()->kinematicController->getGhostObject()->getWorldTransform().getOrigin();
            pos.setY(pos.y() - offsetCrouch);
            trans.setIdentity();
            trans.setOrigin(pos);

            Brakeza3D::get()->getCollisionManager()->getDynamicsWorld()->removeCollisionObject(Brakeza3D::get()->getCamera()->m_ghostObject);
            Brakeza3D::get()->getCollisionManager()->getDynamicsWorld()->removeAction(Brakeza3D::get()->getCamera()->kinematicController);

            capsule = new btCapsuleShapeZ( radius, height );

            player->setCrouch(false);
            flag = true;
        }

        if ((event->type == SDL_KEYDOWN || event->type == SDL_KEYUP ) && flag) {

            Brakeza3D::get()->getCamera()->makeKineticCharacter( trans, capsule );

            Brakeza3D::get()->getCamera()->kinematicController->setGravity( Brakeza3D::get()->getCollisionManager()->getDynamicsWorld()->getGravity() );
            Brakeza3D::get()->getCamera()->kinematicController->setFallSpeed(256);

            Brakeza3D::get()->getCollisionManager()->getDynamicsWorld()->addCollisionObject( Brakeza3D::get()->getCamera()->m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter);
            Brakeza3D::get()->getCollisionManager()->getDynamicsWorld()->addAction(Brakeza3D::get()->getCamera()->kinematicController);
        }
    }
}

void GameInputController::handleFire(SDL_Event *event)
{
    WeaponType *weaponType = Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType();

    if (!weaponType->isAvailable()) return;

    if (event->key.keysym.sym == SDLK_q ) {
        // First keydown
        if (event->type == SDL_KEYDOWN && !weaponType->isFiring()) {
            if (weaponType->isKeyDownHandle()) {
                Logging::getInstance()->Log("Fire KeyDown");
                int keyDownAnimationStatus = weaponType->getKeyDownAnimationStatus();

                if (weaponType->animations[ keyDownAnimationStatus ]->isLooping()) {
                    if (!Mix_Playing(EngineSetup::SoundChannels::SND_WEAPON_LOOP)) {
                        Tools::playMixedSound( weaponType->fireSounds[ keyDownAnimationStatus ], EngineSetup::SoundChannels::SND_WEAPON_LOOP, -1);
                        Logging::getInstance()->Log("Init sound looping mode in first");
                    }
                } else {
                    Mix_HaltChannel(EngineSetup::SoundChannels::SND_WEAPON_LOOP);
                    Logging::getInstance()->Log("Init sound fire phase in first ");
                    Tools::playMixedSound( weaponType->fireSounds[ keyDownAnimationStatus ], EngineSetup::SoundChannels::SND_WEAPON, 0);
                }

                if (weaponType->animations[ keyDownAnimationStatus ]->isProjectile()) {
                    player->shoot();
                }

                weaponType->setWeaponAnimation( keyDownAnimationStatus );
                weaponType->setFiring( true );
                weaponType->fireCounters[ keyDownAnimationStatus ].setEnabled( true );
                weaponType->status = keyDownAnimationStatus;
            }
        }

        // Looping keydown
        if (event->type == SDL_KEYDOWN) {
        }

        // keyup
        if (event->type == SDL_KEYUP) {
            if (weaponType->isKeyUpHandle()) {
                Logging::getInstance()->Log("Fire KeyUp");
                int keyUpAnimationStatus = weaponType->getKeyUpAnimationStatus();

                Mix_HaltChannel(EngineSetup::SoundChannels::SND_WEAPON_LOOP);
                Tools::playMixedSound( weaponType->fireSounds[ keyUpAnimationStatus ], EngineSetup::SoundChannels::SND_WEAPON, 0);

                if (weaponType->animations[ keyUpAnimationStatus ]->isProjectile()) {
                    player->shoot();
                }

                weaponType->setWeaponAnimation( keyUpAnimationStatus );
                weaponType->fireCounters[ keyUpAnimationStatus ].setEnabled( true );
                weaponType->status = keyUpAnimationStatus;

                if (weaponType->status == EngineSetup::WeaponsActions::WALKING) {
                    weaponType->setFiring( false );
                }
            }
        }
    }
}

void GameInputController::handleSniper(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_LSHIFT ) {

        Camera3D *cam = Brakeza3D::get()->getCamera();

        if (event->type == SDL_KEYDOWN) {
            if (Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->isSniper()) {
                Logging::getInstance()->Log("Start Snipper");
                Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->setSniperEnabled( true );

                Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("sniperOn"), EngineSetup::SoundChannels::SND_WEAPON, 0);

                cam->horizontal_fov = (float) EngineSetup::getInstance()->ZOOM_FOV;
                cam->frustum->setup(
                        *cam->getPosition(),
                        Vertex3D(0, 0, 1),
                        EngineSetup::getInstance()->up,
                        EngineSetup::getInstance()->right,
                        cam->getNearDistance(),
                        cam->calcCanvasNearHeight(), cam->calcCanvasNearWidth(),
                        cam->farDistance,
                        cam->calcCanvasFarHeight(), cam->calcCanvasFarWidth()
                );
                cam->UpdateFrustum();
            }
        }

        if (event->type == SDL_KEYUP) {
            if (Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->isSniper()) {
                Logging::getInstance()->Log("Down Snipper");

                Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->setSniperEnabled( false );

                cam->horizontal_fov = (float) EngineSetup::getInstance()->HORIZONTAL_FOV;
                cam->frustum->setup(
                        *cam->getPosition(),
                        Vertex3D(0, 0, 1),
                        EngineSetup::getInstance()->up,
                        EngineSetup::getInstance()->right,
                        cam->getNearDistance(),
                        cam->calcCanvasNearHeight(), cam->calcCanvasNearWidth(),
                        cam->farDistance,
                        cam->calcCanvasFarHeight(), cam->calcCanvasFarWidth()
                );
                cam->UpdateFrustum();

            }
        }

        if ((event->type == SDL_KEYDOWN || event->type == SDL_KEYUP )) {
        }
    }
}

void GameInputController::handleWeaponSelector()
{
    WeaponsManager *weaponManager = Brakeza3D::get()->getWeaponsManager();
    SoundPackage *soundPackage    = EngineBuffers::getInstance()->soundPackage;

    if (keyboard[SDL_SCANCODE_1] && weaponManager->weaponTypes[EngineSetup::WeaponsTypes::PISTOL]->isAvailable()) {
        Tools::playMixedSound( soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        weaponManager->currentWeaponIndex = EngineSetup::WeaponsTypes::PISTOL;
    }

    if (keyboard[SDL_SCANCODE_2] && weaponManager->weaponTypes[EngineSetup::WeaponsTypes::REPEATER]->isAvailable()) {
        Tools::playMixedSound( soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        weaponManager->currentWeaponIndex = EngineSetup::WeaponsTypes::REPEATER;
    }

    if (keyboard[SDL_SCANCODE_3] && weaponManager->weaponTypes[EngineSetup::WeaponsTypes::STATIC_RIFLE]->isAvailable()) {
        Tools::playMixedSound( soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        weaponManager->currentWeaponIndex = EngineSetup::WeaponsTypes::STATIC_RIFLE;
    }

    if (keyboard[SDL_SCANCODE_4] && weaponManager->weaponTypes[EngineSetup::WeaponsTypes::HAR]->isAvailable()) {
        Tools::playMixedSound( soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        weaponManager->currentWeaponIndex = EngineSetup::WeaponsTypes::HAR;
    }

    if (keyboard[SDL_SCANCODE_5] && weaponManager->weaponTypes[EngineSetup::WeaponsTypes::CHAINGUN]->isAvailable()) {
        Tools::playMixedSound( soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        weaponManager->currentWeaponIndex = EngineSetup::WeaponsTypes::CHAINGUN;
    }

    if (keyboard[SDL_SCANCODE_6] && weaponManager->weaponTypes[EngineSetup::WeaponsTypes::GAUSS_CANNON]->isAvailable()) {
        Tools::playMixedSound( soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        weaponManager->currentWeaponIndex = EngineSetup::WeaponsTypes::GAUSS_CANNON;
    }

    if (keyboard[SDL_SCANCODE_7] && weaponManager->weaponTypes[EngineSetup::WeaponsTypes::RAILGUN]->isAvailable()) {
        Tools::playMixedSound( soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        weaponManager->currentWeaponIndex = EngineSetup::WeaponsTypes::RAILGUN;
    }

    if (keyboard[SDL_SCANCODE_8] && weaponManager->weaponTypes[EngineSetup::WeaponsTypes::ROCKETLAUNCHER]->isAvailable()) {
        Tools::playMixedSound( soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        weaponManager->currentWeaponIndex = EngineSetup::WeaponsTypes::ROCKETLAUNCHER;
    }
}

void GameInputController::handleZoom(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_z ) {
        if (event->type == SDL_KEYDOWN) {
            Brakeza3D::get()->getCamera()->horizontal_fov = (float) EngineSetup::getInstance()->ZOOM_FOV;
        }

        if (event->type == SDL_KEYUP) {
            Brakeza3D::get()->getCamera()->horizontal_fov = (float) EngineSetup::getInstance()->HORIZONTAL_FOV;
        }

        Brakeza3D::get()->getCamera()->frustum->setup(
                *Brakeza3D::get()->getCamera()->getPosition(),
                Vertex3D(0, 0, 1),
                EngineSetup::getInstance()->up,
                EngineSetup::getInstance()->right,
                Brakeza3D::get()->getCamera()->getNearDistance(),
                Brakeza3D::get()->getCamera()->calcCanvasNearHeight(), Brakeza3D::get()->getCamera()->calcCanvasNearWidth(),
                Brakeza3D::get()->getCamera()->farDistance,
                Brakeza3D::get()->getCamera()->calcCanvasFarHeight(), Brakeza3D::get()->getCamera()->calcCanvasFarWidth()
        );

        Brakeza3D::get()->getCamera()->UpdateFrustum();
    }
}