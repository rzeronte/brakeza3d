//
// Created by darkhead on 14/1/20.
//

#include "../../headers/Components/ComponentGameInput.h"
#include "../../headers/EngineSetup.h"
#include "../../headers/ComponentsManager.h"


ComponentGameInput::ComponentGameInput(Player *player)
{
    this->player = player;
    setEnabled( true );
}

void ComponentGameInput::onStart()
{
    ComponentsManager::get()->getComponentInput()->setEnabled( false );
}

void ComponentGameInput::preUpdate() {

}

void ComponentGameInput::onUpdate()
{
}

void ComponentGameInput::postUpdate() {

}

void ComponentGameInput::onEnd() {

}

void ComponentGameInput::onSDLPollEvent(SDL_Event *event, bool &finish)
{
    handleMouse(event);
    handleInGameInput(event, finish);
    handleMovingCamera(event, finish);
}

void ComponentGameInput::handleMouse(SDL_Event *event)
{

}

void ComponentGameInput::handleMovingCamera(SDL_Event *event, bool &end)
{
    if (SETUP->MENU_ACTIVE) return;

    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;

    // jump
    if (keyboard[SDL_SCANCODE_SPACE]) {
        if (ComponentsManager::get()->getComponentBSP()->getBSP()->isCurrentLeafLiquid()) {
            this->jump( false, -10, false);
        } else {
            this->jump( true, SETUP->JUMP_FORCE.y, true );
        }
    }

    // step sounds
    if (keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_S] || keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_D]) {
        if (player->counterStep->isFinished()) {

            player->counterStep->setEnabled(true);

            if (!Mix_Playing(EngineSetup::SoundChannels::SND_PLAYER_STEPS)) {
                int rndStep = Tools::random(1, 6);
                Tools::playMixedSound( BUFFERS->soundPackage->getSoundByLabel("playerStep" + std::to_string(rndStep)), EngineSetup::SoundChannels::SND_PLAYER_STEPS, 0);
            }
        }
    }
}

void ComponentGameInput::handleInGameInput(SDL_Event *event, bool &end)
{

    this->handleEscape(event);

    handleRespawnAfterDead(event);

    handleMenuKeyboard(end);

    if (SETUP->MENU_ACTIVE || SETUP->LOADING) return;

    this->handleZoom( event );
    this->handleCrouch( event );
    this->handleFire( event );
    this->handleWeaponReload( event );
    this->handleSniper( event );
    this->handleWeaponSelector();

}

void ComponentGameInput::handleEscape(SDL_Event *event)
{
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;

    if (keyboard[SDL_SCANCODE_ESCAPE] && event->type == SDL_KEYDOWN && player->state != PlayerState::GAMEOVER) {
        SETUP->MENU_ACTIVE = !SETUP->MENU_ACTIVE;

        if (!SETUP->MENU_ACTIVE) {
            //SDL_SetRelativeMouseMode(SDL_TRUE);
            Mix_HaltMusic();
            Mix_PlayMusic( BUFFERS->soundPackage->getMusicByLabel("musicBaseLevel0"), -1 );
            SETUP->DRAW_WEAPON = true;
            SETUP->DRAW_HUD    = true;
            ComponentsManager::get()->getComponentInput()->setEnabled( true );
        } else {
            //SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_WarpMouseInWindow(ComponentsManager::get()->getComponentWindow()->window, SETUP->screenWidth/2, SETUP->screenHeight/2);
            Mix_HaltMusic();
            Mix_PlayMusic( BUFFERS->soundPackage->getMusicByLabel("musicMainMenu"), -1 );
            SETUP->DRAW_WEAPON = false;
            SETUP->DRAW_HUD    = false;
            ComponentsManager::get()->getComponentInput()->setEnabled( false );
        }

        Tools::playMixedSound( BUFFERS->soundPackage->getSoundByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU, 0);
    }
}

void ComponentGameInput::handleMenuKeyboard(bool &end)
{
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;

    // Subir y bajar de opción
    if (keyboard[SDL_SCANCODE_DOWN]) {
        if (SETUP->MENU_ACTIVE) {
            if (ComponentsManager::get()->getComponentMenu()->currentOption + 1 < ComponentsManager::get()->getComponentMenu()->numOptions) {
                ComponentsManager::get()->getComponentMenu()->currentOption++;
                Tools::playMixedSound( BUFFERS->soundPackage->getSoundByLabel("soundMenuClick"), EngineSetup::SoundChannels::SND_MENU, 0);
            }
        }
    }

    if (keyboard[SDL_SCANCODE_UP]) {
        if (SETUP->MENU_ACTIVE) {
            if (ComponentsManager::get()->getComponentMenu()->currentOption > 0) {
                ComponentsManager::get()->getComponentMenu()->currentOption--;
                Tools::playMixedSound( BUFFERS->soundPackage->getSoundByLabel("soundMenuClick"), EngineSetup::SoundChannels::SND_MENU, 0);
            }
        }
    }

    // Enter en opción
    if (keyboard[SDL_SCANCODE_RETURN]) {
        if (SETUP->MENU_ACTIVE && ComponentsManager::get()->getComponentMenu()->options[ComponentsManager::get()->getComponentMenu()->currentOption]->getAction() == ComponentMenu::MNU_EXIT) {
            end = true;
            return;
        }

        if (SETUP->MENU_ACTIVE && player->state == PlayerState::GAMEOVER &&
            ComponentsManager::get()->getComponentMenu()->options[ComponentsManager::get()
            ->getComponentMenu()->currentOption]->getAction() == ComponentMenu::MNU_NEW_GAME
        ) {
            Tools::playMixedSound( BUFFERS->soundPackage->getSoundByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU, 0);

            player->newGame();
        }

        if (SETUP->MENU_ACTIVE && player->state != PlayerState::GAMEOVER &&
            ComponentsManager::get()->getComponentMenu()->options[ComponentsManager::get()
            ->getComponentMenu()->currentOption]->getAction() == ComponentMenu::MNU_NEW_GAME
        ) {

            Tools::playMixedSound( BUFFERS->soundPackage->getSoundByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU, 0);

            Mix_HaltMusic();
            Mix_PlayMusic( BUFFERS->soundPackage->getMusicByLabel("musicBaseLevel0"), -1 );
            SETUP->DRAW_WEAPON = true;
            SETUP->DRAW_HUD    = true;

            SETUP->MENU_ACTIVE = !SETUP->MENU_ACTIVE;
        }
    }
}

void ComponentGameInput::jump(bool checkOnGrount, float YForce, bool soundJump)
{
    if (checkOnGrount && !ComponentsManager::get()->getComponentCamera()->getCamera()->kinematicController->onGround()) {
        return;
    }

    if (soundJump) {
        int rndJump = Tools::random(1, 4);
        Tools::playMixedSound( BUFFERS->soundPackage->getSoundByLabel("playerJump" + std::to_string(rndJump)), EngineSetup::SoundChannels::SND_PLAYER, 0);
    }

    ComponentsManager::get()->getComponentCamera()->getCamera()->kinematicController->jump(btVector3(0, YForce, 0));
}

void ComponentGameInput::handleCrouch(SDL_Event *event)
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

            btVector3 pos = ComponentsManager::get()->getComponentCamera()->getCamera()->kinematicController->getGhostObject()->getWorldTransform().getOrigin();
            pos.setY(pos.y() + offsetCrouch);
            trans.setIdentity();
            trans.setOrigin(pos);

            ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeCollisionObject(
                    ComponentsManager::get()->getComponentCamera()->getCamera()->m_ghostObject);
            ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeAction(
                    ComponentsManager::get()->getComponentCamera()->getCamera()->kinematicController);

            capsule = new btCapsuleShapeZ( radius, height );

            player->setCrouch(true);
            flag = true;
        }

        if (event->type == SDL_KEYUP && player->isCrouch()) {
            height = 4.5f;
            radius = 1.5f;

            btVector3 pos = ComponentsManager::get()->getComponentCamera()->getCamera()->kinematicController->getGhostObject()->getWorldTransform().getOrigin();
            pos.setY(pos.y() - offsetCrouch);
            trans.setIdentity();
            trans.setOrigin(pos);

            ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeCollisionObject(
                    ComponentsManager::get()->getComponentCamera()->getCamera()->m_ghostObject);
            ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeAction(
                    ComponentsManager::get()->getComponentCamera()->getCamera()->kinematicController);

            capsule = new btCapsuleShapeZ( radius, height );

            player->setCrouch(false);
            flag = true;
        }

        if ((event->type == SDL_KEYDOWN || event->type == SDL_KEYUP ) && flag) {

            ComponentsManager::get()->getComponentCamera()->getCamera()->makeKineticCharacter(trans, capsule );

            ComponentsManager::get()->getComponentCamera()->getCamera()->kinematicController->setGravity(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->getGravity() );
            ComponentsManager::get()->getComponentCamera()->getCamera()->kinematicController->setFallSpeed(EngineSetup::getInstance()->BULLET_FALL_SPEED);

            ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->addCollisionObject(
                    ComponentsManager::get()->getComponentCamera()->getCamera()->m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter);
            ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->addAction(
                    ComponentsManager::get()->getComponentCamera()->getCamera()->kinematicController);
        }
    }
}

void ComponentGameInput::handleFire(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_q ) {

        // First keydown
        if (event->type == SDL_KEYDOWN) {
            player->shoot();
        }

        // Looping keydown
        if (event->type == SDL_KEYDOWN) {
        }

        // keyup
        if (event->type == SDL_KEYUP) {
        }
    }
}

void ComponentGameInput::handleWeaponReload(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_e ) {

        // First keydown
        if (event->type == SDL_KEYDOWN) {
            player->reload();
        }

        // Looping keydown
        if (event->type == SDL_KEYDOWN) {
        }

        // keyup
        if (event->type == SDL_KEYUP) {
        }
    }
}

void ComponentGameInput::handleSniper(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_LSHIFT ) {

        Camera3D *cam = ComponentsManager::get()->getComponentCamera()->getCamera();

        if (event->type == SDL_KEYDOWN) {
            if (ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->isSniper()) {
                Logging::getInstance()->Log("Start Snipper");
                ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->setSniperEnabled(true );

                Tools::playMixedSound( BUFFERS->soundPackage->getSoundByLabel("sniperOn"), EngineSetup::SoundChannels::SND_WEAPON, 0);

                cam->horizontal_fov = (float) SETUP->ZOOM_FOV;
                cam->frustum->setup(
                        cam->getPosition(),
                        Vertex3D(0, 0, 1),
                        SETUP->up,
                        SETUP->right,
                        cam->getNearDistance(),
                        cam->calcCanvasNearHeight(), cam->calcCanvasNearWidth(),
                        cam->farDistance,
                        cam->calcCanvasFarHeight(), cam->calcCanvasFarWidth()
                );
                cam->UpdateFrustum();
            }
        }

        if (event->type == SDL_KEYUP) {
            if (ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->isSniper()) {
                Logging::getInstance()->Log("Down Snipper");

                ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->setSniperEnabled(false );

                cam->horizontal_fov = (float) SETUP->HORIZONTAL_FOV;
                cam->frustum->setup(
                        cam->getPosition(),
                        Vertex3D(0, 0, 1),
                        SETUP->up,
                        SETUP->right,
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

void ComponentGameInput::handleWeaponSelector()
{
    return;
    ComponentWeapons *weaponManager = ComponentsManager::get()->getComponentWeapons();
    SoundPackage *soundPackage = BUFFERS->soundPackage;
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;

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

void ComponentGameInput::handleZoom(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_z ) {
        if (event->type == SDL_KEYDOWN) {
            ComponentsManager::get()->getComponentCamera()->getCamera()->horizontal_fov = (float) SETUP->ZOOM_FOV;
        }

        if (event->type == SDL_KEYUP) {
            ComponentsManager::get()->getComponentCamera()->getCamera()->horizontal_fov = (float) SETUP->HORIZONTAL_FOV;
        }

        ComponentsManager::get()->getComponentCamera()->getCamera()->frustum->setup(
                ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition(),
                Vertex3D(0, 0, 1),
                SETUP->up,
                SETUP->right,
                ComponentsManager::get()->getComponentCamera()->getCamera()->getNearDistance(),
                ComponentsManager::get()->getComponentCamera()->getCamera()->calcCanvasNearHeight(), ComponentsManager::get()->getComponentCamera()->getCamera()->calcCanvasNearWidth(),
                ComponentsManager::get()->getComponentCamera()->getCamera()->farDistance,
                ComponentsManager::get()->getComponentCamera()->getCamera()->calcCanvasFarHeight(), ComponentsManager::get()->getComponentCamera()->getCamera()->calcCanvasFarWidth()
        );

        ComponentsManager::get()->getComponentCamera()->getCamera()->UpdateFrustum();
    }
}


void ComponentGameInput::handleRespawnAfterDead(SDL_Event *event)
{
    if ( player->isDead() && !SETUP->MENU_ACTIVE ) {
        if ( event->type == SDL_KEYDOWN ) {
            player->respawn();
        }

        return;
    }
}

bool ComponentGameInput::isEnabled() const {
    return enabled;
}

void ComponentGameInput::setEnabled(bool enabled) {
    ComponentGameInput::enabled = enabled;
}

