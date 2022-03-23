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
    if (ComponentsManager::get()->getComponentGame()->getGameState() == EngineSetup::GameState::MENU) return;



    if (ComponentsManager::get()->getComponentGame()->getGameState() == EngineSetup::GameState::GAMING) {
        this->handleKeyboardMovingPlayer();
        this->handleGamePadMovingPlayer();
        this->handleFire();
    }
}

void ComponentGameInput::postUpdate() {

}

void ComponentGameInput::onEnd() {

}

void ComponentGameInput::onSDLPollEvent(SDL_Event *event, bool &finish) {
    this->handleInGameInput(event, finish);
}

void ComponentGameInput::handleInGameInput(SDL_Event *event, bool &end) {

    this->handleEscape(event);

    auto state = ComponentsManager::get()->getComponentGame()->getGameState();
    auto componentInput = ComponentsManager::get()->getComponentInput();

    if (state == EngineSetup::GameState::MENU) {
        this->handleMenuKeyboard(event, end);
    }
    if (state == EngineSetup::GameState::PRESSKEY && (event->type == SDL_KEYDOWN || (event->type == SDL_CONTROLLERBUTTONDOWN && componentInput->isAnyControllerButtonPressed()))) {
        ComponentsManager::get()->getComponentGame()->getLevelInfo()->loadNext();
        ComponentsManager::get()->getComponentGame()->setGameState(EngineSetup::GameState::GAMING);
    }

    if (state == EngineSetup::GameState::ENDGAME && (event->type == SDL_KEYDOWN || (event->type == SDL_CONTROLLERBUTTONDOWN && componentInput->isAnyControllerButtonPressed()))) {
        ComponentsManager::get()->getComponentGame()->getPlayer()->setLevelCompletedCounter(0);
        ComponentsManager::get()->getComponentGame()->getLevelInfo()->setCurrentLevelIndex(-1);
        ComponentsManager::get()->getComponentGame()->getFadeToGameState()->resetTo(EngineSetup::GameState::MENU);
    }

    if (state == EngineSetup::GameState::GAMING) {
        this->handleFindClosestObject3D(event);
        this->handleWeaponSelector(event);
    }

    //this->handleZoom(event);
}

void ComponentGameInput::handleEscape(SDL_Event *event) {
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;

    auto game = ComponentsManager::get()->getComponentGame();
    auto gameState = game->getGameState();

    if (
        (keyboard[SDL_SCANCODE_ESCAPE] && event->type == SDL_KEYDOWN) ||
        (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_GUIDE)
    ) {
        if (gameState == EngineSetup::GameState::MENU && game->getLevelInfo()->isLevelStartedToPlay()) {
            game->getFadeToGameState()->resetTo(EngineSetup::GameState::GAMING);
        } else {
            game->getFadeToGameState()->resetTo(EngineSetup::GameState::MENU);

            SDL_WarpMouseInWindow(
                ComponentsManager::get()->getComponentWindow()->window,
                SETUP->screenWidth / 2,
                SETUP->screenHeight / 2
            );
        }
    }
}

void ComponentGameInput::handleMenuKeyboard(SDL_Event *event, bool &end)
{
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto componentMenu = ComponentsManager::get()->getComponentMenu();
    auto componentInput = ComponentsManager::get()->getComponentInput();
    auto menuOptions = componentMenu->options;

    int currentOption = componentMenu->currentOption;
    Uint8 *keyboard = componentInput->keyboard;


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
            if (!ComponentsManager::get()->getComponentGame()->getLevelInfo()->isLevelStartedToPlay()) {
                ComponentsManager::get()->getComponentGame()->getFadeToGameState()->resetTo(EngineSetup::GameState::PRESSKEY);
                player->respawn();
            } else {
                ComponentsManager::get()->getComponentGame()->getFadeToGameState()->resetTo(EngineSetup::GameState::GAMING);
            }
        }

        if (player->state == PlayerState::DEAD && menuOptions[currentOption]->getAction() == ComponentMenu::MNU_NEW_GAME) {
            Tools::playSound(BUFFERS->soundPackage->getByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU,0);
            ComponentsManager::get()->getComponentGame()->getFadeToGameState()->resetTo(EngineSetup::GameState::GAMING);
        }

        if (menuOptions[currentOption]->getAction() == ComponentMenu::MNU_HELP) {
            componentGame->getFadeToGameState()->resetTo(EngineSetup::GameState::HELP);
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

void ComponentGameInput::handleWeaponSelector(SDL_Event *event) {
    SoundPackage *soundPackage = BUFFERS->soundPackage;
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;
    auto game = ComponentsManager::get()->getComponentGame();

    if (event->type == SDL_KEYDOWN) {
        if (keyboard[SDL_SCANCODE_1]) {
            game->getPlayer()->setWeaponTypeByIndex(0);
        }

        if (keyboard[SDL_SCANCODE_2]) {
            game->getPlayer()->setWeaponTypeByIndex(1);
            Tools::playSound(soundPackage->getByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        }

        if (keyboard[SDL_SCANCODE_3] ) {
            game->getPlayer()->setWeaponTypeByIndex(2);
            Tools::playSound(soundPackage->getByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        }

        if (keyboard[SDL_SCANCODE_4] ) {
            game->getPlayer()->setWeaponTypeByIndex(3);
            Tools::playSound(soundPackage->getByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
        }
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

void ComponentGameInput::handleFindClosestObject3D(SDL_Event *event)
{
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;
    auto game = ComponentsManager::get()->getComponentGame();

    if (
        (keyboard[SDL_SCANCODE_TAB] && event->type == SDL_KEYDOWN) ||
        (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
     ) {
        game->selectClosestObject3DFromPlayer();
    }
}

void ComponentGameInput::handleGamePadMovingPlayer()
{
    if (!EngineSetup::get()->GAMEPAD_CONTROLLER_ENABLED) return;

    auto componentInput = ComponentsManager::get()->getComponentInput();

    float speed = player->power * Brakeza3D::get()->getDeltaTime();
    speed = std::clamp(speed, 0.f, player->maxVelocity);

    player->setVelocity(
        player->getVelocity() +
        Vertex3D(componentInput->controllerAxisLeftX * speed, componentInput->controllerAxisLeftY * speed, 0)
    );
}