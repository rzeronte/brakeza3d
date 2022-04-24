#include "../../include/Components/ComponentGameInput.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"


ComponentGameInput::ComponentGameInput(Player *player) {
    this->player = player;
    setEnabled(true);
    currentWeaponIndex = 0;
}

void ComponentGameInput::onStart() {
    Logging::Log("ComponentGameInput onStart", "ComponentGameInput");
}

void ComponentGameInput::preUpdate() {
    if (!isEnabled()) return;

    if (ComponentsManager::get()->getComponentGame()->getGameState() == EngineSetup::GameState::MENU) return;

    if (ComponentsManager::get()->getComponentGame()->getGameState() == EngineSetup::GameState::GAMING) {
        this->handleKeyboardMovingPlayer();
        this->handleGamePadMovingPlayer();
        this->handleFire();
    }
}

void ComponentGameInput::onUpdate() {

}

void ComponentGameInput::postUpdate() {

}

void ComponentGameInput::onEnd() {

}

void ComponentGameInput::onSDLPollEvent(SDL_Event *event, bool &finish) {
    if (!isEnabled()) return;

    this->handleInGameInput(event, finish);
}

void ComponentGameInput::handleInGameInput(SDL_Event *event, bool &end) {

    this->handleEscape(event);

    auto state = ComponentsManager::get()->getComponentGame()->getGameState();

    if (state == EngineSetup::GameState::MENU) {
        this->handleMenuKeyboard(event, end);
        return;
    }

    this->handlePressKeyGameStates(event);


    if (state == EngineSetup::GameState::GAMING) {
        this->handleFindClosestObject3D(event);
        this->handleWeaponSelector(event);
        this->handleDashMovement(event);
        this->handleMakeGravitationalShields(event);
        this->handleEnergyShield(event);
        this->updateWeaponStatus(event);
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
            game->makeFadeToGameState(EngineSetup::GameState::GAMING);
        } else {
            // can't cancel countdown
            if (gameState == EngineSetup::GameState::COUNTDOWN || gameState == EngineSetup::GameState::PRESSKEY_NEWLEVEL ) return;

            game->makeFadeToGameState(EngineSetup::GameState::MENU);

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
            ComponentsManager::get()->getComponentSound()->playSound(
                BUFFERS->soundPackage->getByLabel("soundMenuClick"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }
    }

    if (keyboard[SDL_SCANCODE_UP] || (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP)) {
        if (currentOption > 0) {
            componentMenu->currentOption--;
            ComponentsManager::get()->getComponentSound()->playSound(
                BUFFERS->soundPackage->getByLabel("soundMenuClick"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }
    }

    // Execute Menu option
    if (keyboard[SDL_SCANCODE_RETURN] || componentInput->controllerButtonA) {
        if (menuOptions[currentOption]->getAction() == ComponentMenu::MNU_EXIT) {
            end = true;
            return;
        }

        if (menuOptions[currentOption]->getAction() == ComponentMenu::MNU_NEW_GAME) {
            ComponentsManager::get()->getComponentGame()->pressedKeyForNewGame();
        }

        if (menuOptions[currentOption]->getAction() == ComponentMenu::MNU_HELP) {
            componentGame->makeFadeToGameState(EngineSetup::GameState::HELP);
        }

        if (menuOptions[currentOption]->getAction() == ComponentMenu::MNU_CREDITS) {
            componentGame->makeFadeToGameState(EngineSetup::GameState::CREDITS);
        }
    }
}

void ComponentGameInput::handleFire() const
{
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
            currentWeaponIndex = 0;
            game->getPlayer()->setWeaponTypeByIndex(0);
        }

        if (keyboard[SDL_SCANCODE_2]) {
            currentWeaponIndex = 1;
            game->getPlayer()->setWeaponTypeByIndex(1);
            ComponentsManager::get()->getComponentSound()->playSound(
                soundPackage->getByLabel("switchWeapon"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }

        if (keyboard[SDL_SCANCODE_3] ) {
            currentWeaponIndex = 2;
            game->getPlayer()->setWeaponTypeByIndex(2);
            ComponentsManager::get()->getComponentSound()->playSound(
                soundPackage->getByLabel("switchWeapon"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }

        if (keyboard[SDL_SCANCODE_4] ) {
            currentWeaponIndex = 3;
            game->getPlayer()->setWeaponTypeByIndex(3);
            ComponentsManager::get()->getComponentSound()->playSound(
                soundPackage->getByLabel("switchWeapon"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }
    }

    if (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
        player->nextWeapon();
    }

    if (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) {
        player->previousWeapon();
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

    if (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) {
        ComponentsManager::get()->getComponentRender()->setSelectedObject(nullptr);
        ComponentsManager::get()->getComponentRender()->updateSelectedObject3DInShaders(nullptr);
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

void ComponentGameInput::handleDashMovement(SDL_Event *event)
{
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;
    auto input = ComponentsManager::get()->getComponentInput();

    if (
        (keyboard[SDL_SCANCODE_BACKSPACE] && event->type == SDL_KEYDOWN) ||
        (event->type == SDL_CONTROLLERBUTTONDOWN && input->controllerButtonB)
    ) {
        const float dashEnergyCost = 10;

        if (player->getEnergy() < dashEnergyCost) {
            return;
        }

        ComponentsManager::get()->getComponentSound()->playSound(
            BUFFERS->soundPackage->getByLabel("dash"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );

        auto fireworks = new ParticleEmissorFireworks(true, 1520, 1, 0.01, Color::green(), 6, 25);
        fireworks->setPosition(player->getPosition());
        fireworks->setRotationFrame(0, 4, 5);
        Brakeza3D::get()->addObject3D(fireworks, "fireworks" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());

        float speed = player->dashPower * Brakeza3D::get()->getDeltaTime();

        player->setVelocity(
            player->getVelocity() +
            Vertex3D(input->controllerAxisLeftX * speed, input->controllerAxisLeftY * speed, 0)
        );

        player->useEnergy(dashEnergyCost);
    }
}

void ComponentGameInput::handleEnergyShield(SDL_Event *event) {

    if ( event->cbutton.type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_X) {
        if (!player->isAllowEnergyShield()) {
            return;
        }

        player->setEnergyShieldEnabled(true);
        player->shieldModel->setEnabled(true);
        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("energyShield"),
            1,
            0
        );
    }

    if (event->cbutton.type == SDL_CONTROLLERBUTTONUP && event->cbutton.button == SDL_CONTROLLER_BUTTON_X) {
        player->setEnergyShieldEnabled(false);
    }
}

void ComponentGameInput::updateWeaponStatus(SDL_Event *event) {

    if (event->type == SDL_CONTROLLERBUTTONUP && event->cbutton.button == SDL_CONTROLLER_BUTTON_A) {
        player->getWeapon()->setStatus(WeaponStatus::RELEASED);
    }

    if (event->cbutton.type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_A) {
        player->getWeapon()->setStatus(WeaponStatus::PRESSED);
    }
}

void ComponentGameInput::handleMakeGravitationalShields(SDL_Event *event)
{
    auto componentInput = ComponentsManager::get()->getComponentInput();
    if (event->cbutton.type == SDL_CONTROLLERBUTTONDOWN && componentInput->controllerButtonY) {
        if (player->isAllowGravitationalShields()) {
            player->makeGravitationalShield();
        }
    }
}

void ComponentGameInput::handlePressKeyGameStates(SDL_Event *event)
{
    auto state = ComponentsManager::get()->getComponentGame()->getGameState();
    auto componentInput = ComponentsManager::get()->getComponentInput();

    if ((state == EngineSetup::GameState::PRESSKEY_NEWLEVEL|| state == EngineSetup::PRESSKEY_PREVIOUS_LEVEL) &&
        (event->type == SDL_KEYDOWN || (event->type == SDL_CONTROLLERBUTTONDOWN && componentInput->isAnyControllerButtonPressed()))) {
        ComponentsManager::get()->getComponentGame()->pressedKeyForBeginLevel();
    }

    if (state == EngineSetup::GameState::PRESSKEY_GAMEOVER && (event->type == SDL_KEYDOWN || (event->type == SDL_CONTROLLERBUTTONDOWN && componentInput->isAnyControllerButtonPressed()))) {
        ComponentsManager::get()->getComponentGame()->pressedKeyForFinishGameAndRestart();
    }

    if (state == EngineSetup::GameState::PRESSKEY_BY_DEAD && (event->type == SDL_KEYDOWN || (event->type == SDL_CONTROLLERBUTTONDOWN && componentInput->isAnyControllerButtonPressed()))) {
        ComponentsManager::get()->getComponentGame()->pressedKeyByDead();
    }
}
