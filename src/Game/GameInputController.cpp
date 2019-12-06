#include "../../headers/Game/GameInputController.h"
#include "../../headers/Brakeza3D.h"
#include "../../headers/Render/EngineBuffers.h"

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

void GameInputController::handleKeyboardContinuous(SDL_Event *event, bool &end)
{
    if (EngineSetup::getInstance()->MENU_ACTIVE) return;

    InputController::handleKeyboardContinuous(event, end);

    if (keyboard[SDL_SCANCODE_SPACE]) {
        this->jump();
    }

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

void GameInputController::handleKeyboard(SDL_Event *event, bool &end)
{
    InputController::handleKeyboard(event, end);

    if (event->type == SDL_WINDOWEVENT) {
        switch (event->window.event) {
            case SDL_WINDOWEVENT_SHOWN:
                break;
            case SDL_WINDOWEVENT_HIDDEN:
                break;
            case SDL_WINDOWEVENT_EXPOSED:
                break;
            case SDL_WINDOWEVENT_MOVED:
                break;
            case SDL_WINDOWEVENT_RESIZED:
                break;
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:
                break;
            case SDL_WINDOWEVENT_RESTORED:
                break;
            case SDL_WINDOWEVENT_ENTER:
                break;
            case SDL_WINDOWEVENT_LEAVE:
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                break;
            case SDL_WINDOWEVENT_CLOSE:
                end = true;
                break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
            case SDL_WINDOWEVENT_TAKE_FOCUS:
                break;
            case SDL_WINDOWEVENT_HIT_TEST:
                break;
#endif
            default:
                break;
        }
    }

    if (keyboard[SDL_SCANCODE_ESCAPE] && event->type == SDL_KEYDOWN && player->state != PlayerState::GAMEOVER) {
        EngineSetup::getInstance()->MENU_ACTIVE = !EngineSetup::getInstance()->MENU_ACTIVE;
        if (!EngineSetup::getInstance()->MENU_ACTIVE) {
            Mix_HaltMusic();
            Mix_PlayMusic( EngineBuffers::getInstance()->soundPackage->getMusicByLabel("musicBaseLevel0"), -1 );
            EngineSetup::getInstance()->DRAW_WEAPON = true;
            EngineSetup::getInstance()->DRAW_HUD    = true;
        } else {
            Mix_HaltMusic();
            Mix_PlayMusic( EngineBuffers::getInstance()->soundPackage->getMusicByLabel("musicMainMenu"), -1 );
            EngineSetup::getInstance()->DRAW_WEAPON = false;
            EngineSetup::getInstance()->DRAW_HUD    = false;
        }

        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU, 0);
    }

    handleMenuKeyboard(end);

    if (EngineSetup::getInstance()->MENU_ACTIVE || EngineSetup::getInstance()->SPLASHING || EngineSetup::getInstance()->LOADING) return;

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

    this->handleCrouch( event );
    this->handleFire( event );

    if (keyboard[SDL_SCANCODE_1]) {
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::PISTOL;
    }

    if (keyboard[SDL_SCANCODE_2]) {
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::REPEATER;
    }

    if (keyboard[SDL_SCANCODE_3]) {
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::STATIC_RIFLE;
    }

    if (keyboard[SDL_SCANCODE_4]) {
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::HAR;
    }

    if (keyboard[SDL_SCANCODE_5]) {
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::CHAINGUN;
    }

    if (keyboard[SDL_SCANCODE_6]) {
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::GAUSS_CANNON;
    }

    if (keyboard[SDL_SCANCODE_7]) {
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::RAILGUN;
    }

    if (keyboard[SDL_SCANCODE_8]) {
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::ROCKETLAUNCHER;
    }


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
        if (EngineSetup::getInstance()->MENU_ACTIVE && Brakeza3D::get()->getMenuManager()->options[Brakeza3D::get()->getMenuManager()->currentOption]->label == "exit") {
            end = true;
            return;
        }

        if (EngineSetup::getInstance()->MENU_ACTIVE && player->state == PlayerState::GAMEOVER && Brakeza3D::get()->getMenuManager()->options[Brakeza3D::get()->getMenuManager()->currentOption]->label == "new game") {
            Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU, 0);

            player->newGame();
        }
    }
}

void GameInputController::jump()
{
    if( Brakeza3D::get()->getCamera()->kinematicController->onGround() ) {
        int rndJump = Tools::random(1, 4);
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("playerJump" + std::to_string(rndJump)), EngineSetup::SoundChannels::SND_PLAYER, 0);

        Brakeza3D::get()->getCamera()->kinematicController->jump(btVector3(0, EngineSetup::getInstance()->JUMP_FORCE.y, 0));
    }
}

void GameInputController::handleCrouch(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_TAB ) {

        btTransform trans;
        btConvexShape *capsule;
        float radius, height;
        bool flag = false;
        float offsetCrouch = 2.25;

        if (event->type == SDL_KEYDOWN && !player->isStooped()) {
            height = 1.0f;
            radius = 1.0f;

            btVector3 pos = Brakeza3D::get()->getCamera()->kinematicController->getGhostObject()->getWorldTransform().getOrigin();
            pos.setY(pos.y() + offsetCrouch);
            trans.setIdentity();
            trans.setOrigin(pos);

            Brakeza3D::get()->getCollisionManager()->getDynamicsWorld()->removeCollisionObject(Brakeza3D::get()->getCamera()->m_ghostObject);
            Brakeza3D::get()->getCollisionManager()->getDynamicsWorld()->removeAction(Brakeza3D::get()->getCamera()->kinematicController);


            capsule = new btCapsuleShapeZ( radius, height );

            player->setStooped( true );
            flag = true;
        }

        if (event->type == SDL_KEYUP && player->isStooped()) {
            height = 4.5f;
            radius = 1.5f;

            btVector3 pos = Brakeza3D::get()->getCamera()->kinematicController->getGhostObject()->getWorldTransform().getOrigin();
            pos.setY(pos.y() - offsetCrouch);
            trans.setIdentity();
            trans.setOrigin(pos);

            Brakeza3D::get()->getCollisionManager()->getDynamicsWorld()->removeCollisionObject(Brakeza3D::get()->getCamera()->m_ghostObject);
            Brakeza3D::get()->getCollisionManager()->getDynamicsWorld()->removeAction(Brakeza3D::get()->getCamera()->kinematicController);

            capsule = new btCapsuleShapeZ( radius, height );

            player->setStooped( false );
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

    if (event->key.keysym.sym == SDLK_q ) {
        // First keydown
        if (event->type == SDL_KEYDOWN && !weaponType->isFiring()) {
            if (weaponType->isKeyDownHandle()) {
                Logging::getInstance()->Log("Fire KeyDown");
                int keyDownAnimationStatus = weaponType->getKeyDownAnimationStatus();

                Tools::playMixedSound( weaponType->fireSounds[ keyDownAnimationStatus ], EngineSetup::SoundChannels::SND_PLAYER, 0);

                weaponType->setWeaponAnimation( keyDownAnimationStatus );
                weaponType->setFiring( true );
                weaponType->fireCounters[ keyDownAnimationStatus ].setEnabled( true );
                weaponType->status = keyDownAnimationStatus;

            } else {
                Logging::getInstance()->Log("Avoid Fire KeyDown");
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
                weaponType->setWeaponAnimation( keyUpAnimationStatus );
                weaponType->fireCounters[ keyUpAnimationStatus ].setEnabled( true );
                weaponType->status = keyUpAnimationStatus;

            } else {
                Logging::getInstance()->Log("Avoid Fire KeyUp");
            }
        }
    }
}
