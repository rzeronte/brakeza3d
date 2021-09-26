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
    if (event->type == SDL_BUTTON_RIGHT) {
    }

    if (event->type == SDL_BUTTON_LEFT) {
    }
}

void ComponentGameInput::handleMovingCamera(SDL_Event *event, bool &end)
{
    if (SETUP->MENU_ACTIVE) return;

    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;
    Camera3D *cam = ComponentsManager::get()->getComponentCamera()->getCamera();
    ComponentGame *game = ComponentsManager::get()->getComponentGame();

    // jump
    if (keyboard[SDL_SCANCODE_SPACE]) {
        if (event->type == SDL_KEYDOWN) {
            if (player->isVehicle()) {
                player->setIsVehicle(false);
                cam->setFollowTo( game->character );
                game->car->setEnabled(false);
                game->character->setEnabled(true);
            } else {
                player->setIsVehicle(true);
                cam->setFollowTo( game->car );
                game->car->setEnabled(true);
                game->character->setEnabled(false);
            }
        }
        //player->jump();
    }

    // step sounds
    if (keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_S] || keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_D]) {
        if (event->type == SDL_KEYDOWN) {
        }

        if (player->counterStep->isFinished()) {

            player->counterStep->setEnabled(true);

            if (!Mix_Playing(EngineSetup::SoundChannels::SND_PLAYER_STEPS)) {
                Tools::playMixedSound( BUFFERS->soundPackage->getSoundByLabel("playerSteps"), EngineSetup::SoundChannels::SND_PLAYER_STEPS, 0);
            }
        }
    } else {
        Mix_HaltChannel(EngineSetup::SoundChannels::SND_PLAYER_STEPS);
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

void ComponentGameInput::jump(bool checkOnGround, float YForce, bool soundJump)
{
    if (soundJump) {
        player->jump();
    }
}

void ComponentGameInput::handleCrouch(SDL_Event *event)
{
    if (event->key.keysym.sym == SDLK_TAB ) {

        if (event->type == SDL_KEYDOWN && !player->isCrouch()) {
            player->setCrouch(true);
        }

        if (event->type == SDL_KEYUP && player->isCrouch()) {
            player->setCrouch(false);
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

