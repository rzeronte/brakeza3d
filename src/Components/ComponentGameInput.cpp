#include "../../include/Components/ComponentGameInput.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"


ComponentGameInput::ComponentGameInput(Player *player) {
    this->player = player;
    setEnabled(true);
}

void ComponentGameInput::onStart() {
    Logging::Log("ComponentGameInput onStart", "ComponentGameInput");
}

void ComponentGameInput::preUpdate() {

}

void ComponentGameInput::onUpdate() {
    if (ComponentsManager::get()->getComponentGame()->getGameState() == GameState::MENU) return;

    this->handleKeyboardMovingPlayer();
    this->handleFire();
}

void ComponentGameInput::postUpdate() {

}

void ComponentGameInput::onEnd() {

}

void ComponentGameInput::onSDLPollEvent(SDL_Event *event, bool &finish) {
    this->handleInGameInput(event, finish);
}

void ComponentGameInput::handleInGameInput(SDL_Event *event, bool &end) {

    this->handleMenuKeyboard(event, end);
    this->handleEscape(event);

    if (ComponentsManager::get()->getComponentGame()->getGameState() != GameState::GAMING) {
        return;
    }

    this->handleFindClosestObject3D(event);

    //this->handleZoom(event);
    //this->handleWeaponSelector();
}

void ComponentGameInput::handleEscape(SDL_Event *event) {
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;

    GameState gameState = ComponentsManager::get()->getComponentGame()->getGameState();

    if (keyboard[SDL_SCANCODE_ESCAPE] && event->type == SDL_KEYDOWN ) {
        if (gameState == GameState::MENU) {
            ComponentsManager::get()->getComponentGame()->setGameState(GameState::GAMING);
            //SDL_SetRelativeMouseMode(SDL_TRUE);
            Mix_HaltMusic();
            Mix_PlayMusic(BUFFERS->soundPackage->getMusicByLabel("musicBaseLevel0"), -1);
            SETUP->DRAW_HUD = true;
        } else {
            ComponentsManager::get()->getComponentGame()->setGameState(GameState::MENU);
            //SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_WarpMouseInWindow(
                ComponentsManager::get()->getComponentWindow()->window,
                SETUP->screenWidth / 2,
                SETUP->screenHeight / 2
            );
            Mix_HaltMusic();
            Mix_PlayMusic(BUFFERS->soundPackage->getMusicByLabel("musicMainMenu"), -1);
            SETUP->DRAW_HUD = false;
        }

        Tools::playSound(BUFFERS->soundPackage->getByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU, 0);
    }
}

void ComponentGameInput::handleMenuKeyboard(SDL_Event *event, bool &end) {

    auto gameState = ComponentsManager::get()->getComponentGame()->getGameState();
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto componentMenu = ComponentsManager::get()->getComponentMenu();
    auto componentInput = ComponentsManager::get()->getComponentInput();
    auto menuOptions = componentMenu->options;

    int currentOption = componentMenu->currentOption;
    Uint8 *keyboard = componentInput->keyboard;

    if (gameState != GameState::MENU) {
        return;
    }

    // Up / Down menu options
    if (keyboard[SDL_SCANCODE_DOWN] || (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
        if (componentMenu->currentOption + 1 < componentMenu->numOptions) {
            componentMenu->currentOption++;
            Tools::playSound(BUFFERS->soundPackage->getByLabel("soundMenuClick"),EngineSetup::SoundChannels::SND_MENU, 0);
        }
    }

    if (keyboard[SDL_SCANCODE_UP] || (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP)) {
        if (currentOption > 0) {
            componentMenu->currentOption--;
            Tools::playSound(BUFFERS->soundPackage->getByLabel("soundMenuClick"),EngineSetup::SoundChannels::SND_MENU, 0);
        }
    }

    // Execute Menu option
    if (keyboard[SDL_SCANCODE_RETURN] || componentInput->controllerButtonA) {
        if (menuOptions[currentOption]->getAction() == ComponentMenu::MNU_EXIT) {
            end = true;
            return;
        }

        if (menuOptions[currentOption]->getAction() == ComponentMenu::MNU_NEW_GAME) {
            Tools::playSound(BUFFERS->soundPackage->getByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU,0);
            componentGame->setGameState(GameState::GAMING);
            player->newGame();
        }

        if (player->state == PlayerState::DEAD && menuOptions[currentOption]->getAction() == ComponentMenu::MNU_NEW_GAME) {
            Tools::playSound(BUFFERS->soundPackage->getByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU,0);
            componentGame->setGameState(GameState::GAMING);
            Mix_HaltMusic();
            Mix_PlayMusic(BUFFERS->soundPackage->getMusicByLabel("musicBaseLevel0"), -1);
            SETUP->DRAW_HUD = true;
        }
    }
}

void ComponentGameInput::jump(bool soundJump) const {
    if (soundJump) {
        player->jump();
    }
}

void ComponentGameInput::handleFire() const {
    auto componentInput = ComponentsManager::get()->getComponentInput();
    Uint8 *keyboard = componentInput->keyboard;
    if (keyboard[SDL_SCANCODE_SPACE] || componentInput->controllerButtonA) {
        player->shoot();
    }
}

void ComponentGameInput::handleWeaponSelector() {
    SoundPackage *soundPackage = BUFFERS->soundPackage;
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;

    if (keyboard[SDL_SCANCODE_1]) {
    }

    if (keyboard[SDL_SCANCODE_2]) {
        Tools::playSound(soundPackage->getByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
    }

    if (keyboard[SDL_SCANCODE_3] ) {
        Tools::playSound(soundPackage->getByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
    }
}

void ComponentGameInput::handleZoom(SDL_Event *event)
{
    float horizontal_fov = SETUP->HORIZONTAL_FOV;
    if (event->key.keysym.sym == SDLK_z) {
        if (event->type == SDL_KEYDOWN) {
            horizontal_fov = SETUP->ZOOM_FOV;
        }

        if (event->type == SDL_KEYUP) {
            horizontal_fov = SETUP->HORIZONTAL_FOV;
        }

        ComponentsManager::get()->getComponentCamera()->getCamera()->frustum->setup(
                ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition(),
                Vertex3D(0, 0, 1),
                SETUP->up,
                SETUP->right,
                horizontal_fov,
                ((float) EngineSetup::get()->screenHeight / (float) EngineSetup::get()->screenWidth),
                EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE
        );

        ComponentsManager::get()->getComponentCamera()->getCamera()->updateFrustum();
    }
}

bool ComponentGameInput::isEnabled() const {
    return enabled;
}

void ComponentGameInput::setEnabled(bool enabled) {
    ComponentGameInput::enabled = enabled;
}

void ComponentGameInput::handleKeyboardMovingPlayer() {
    auto componentInput = ComponentsManager::get()->getComponentInput();
    Uint8 *keyboard = componentInput->keyboard;

    float speed = player->power * Brakeza3D::get()->getDeltaTime();
    speed = std::clamp(speed, 0.f, player->maxVelocity);

    if (keyboard[SDL_SCANCODE_A]) {
        Vertex3D velocity = player->getVelocity() - Vertex3D(speed, 0, 0);
        player->setVelocity(velocity);
    }

    if (keyboard[SDL_SCANCODE_D]) {
        Vertex3D velocity = player->getVelocity() + Vertex3D(speed, 0, 0);
        player->setVelocity(velocity);
    }

    if (keyboard[SDL_SCANCODE_S]) {
        Vertex3D velocity = player->getVelocity() + Vertex3D(0, speed, 0);
        player->setVelocity(velocity);
    }

    if (keyboard[SDL_SCANCODE_W]) {
        Vertex3D velocity = player->getVelocity() - Vertex3D(0, speed, 0);
        player->setVelocity(velocity);
    }
}

void ComponentGameInput::handleFindClosestObject3D(SDL_Event *event ) {
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;
    auto game = ComponentsManager::get()->getComponentGame();

    if (keyboard[SDL_SCANCODE_TAB] && event->type == SDL_KEYDOWN) {
        game->selectClosestObject3DFromPlayer();
    }
}


