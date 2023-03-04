#include "../../include/Components/ComponentGameInput.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ComponentGameInput::ComponentGameInput()
:
    controllerAxisThreshold(0.1),
    lockRightStick(false)
{
}

void ComponentGameInput::onStart()
{
    Logging::Log("ComponentGameInput onStart");
}

void ComponentGameInput::preUpdate()
{
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

void ComponentGameInput::onSDLPollEvent(SDL_Event *event, bool &finish)
{
    if (!isEnabled()) return;

    this->handleInGameInput(event, finish);
}

void ComponentGameInput::handleInGameInput(SDL_Event *event, bool &end)
{
    this->handleEscape(event);
    this->handleCheckPadConnection(event);

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
        this->handleMakeReflection(event);
        this->handleBomb(event);
        this->handleEnergyShield(event);
    }

    //this->handleZoom(event);
}

void ComponentGameInput::handleEscape(SDL_Event *event)
{
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->getKeyboard();

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
                ComponentsManager::get()->getComponentWindow()->getWindow(),
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
    auto menuOptions = componentMenu->getOptions();

    int currentOption = componentMenu->getCurrentOption();
    Uint8 *keyboard = componentInput->getKeyboard();


    // Up / Down menu options
    if (keyboard[SDL_SCANCODE_DOWN] || (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
        if (currentOption + 1 < componentMenu->getNumOptions()) {
            componentMenu->increaseMenuOption();
            ComponentSound::playSound(
                BUFFERS->soundPackage->getByLabel("soundMenuClick"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }
    }

    if (keyboard[SDL_SCANCODE_UP] || (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP)) {
        if (currentOption > 0) {
            componentMenu->decreaseMenuOption();
            ComponentSound::playSound(
                BUFFERS->soundPackage->getByLabel("soundMenuClick"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }
    }

    // Execute Menu option
    if (keyboard[SDL_SCANCODE_RETURN] || componentInput->getControllerButtonA()) {
        if (menuOptions[currentOption].getAction() == ComponentMenu::MNU_EXIT) {
            end = true;
            return;
        }

        if (menuOptions[currentOption].getAction() == ComponentMenu::MNU_NEW_GAME) {
            ComponentsManager::get()->getComponentGame()->pressedKeyForNewGame();
        }

        if (menuOptions[currentOption].getAction() == ComponentMenu::MNU_HELP) {
            componentGame->makeFadeToGameState(EngineSetup::GameState::HELP);
        }

        if (menuOptions[currentOption].getAction() == ComponentMenu::MNU_CREDITS) {
            componentGame->makeFadeToGameState(EngineSetup::GameState::CREDITS);
        }
    }
}

void ComponentGameInput::handleFire() const
{
    auto componentInput = ComponentsManager::get()->getComponentInput();
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();

    player->getShaderLaser().setEnabled(false);

    Uint8 *keyboard = componentInput->getKeyboard();
    if (keyboard[SDL_SCANCODE_SPACE] || componentInput->getControllerAxisTriggerRight() > this->controllerAxisThreshold) {
        player->shoot(componentInput->getControllerAxisTriggerRight());
    }
}

void ComponentGameInput::handleWeaponSelector(SDL_Event *event) {
    SoundPackage *soundPackage = BUFFERS->soundPackage;
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->getKeyboard();
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();

    if (event->type == SDL_KEYDOWN) {
        if (keyboard[SDL_SCANCODE_1]) {
            componentGame->getPlayer()->setWeaponTypeByIndex(0);
        }

        if (keyboard[SDL_SCANCODE_2]) {
            componentGame->getPlayer()->setWeaponTypeByIndex(1);
            ComponentSound::playSound(
                soundPackage->getByLabel("switchWeapon"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }

        if (keyboard[SDL_SCANCODE_3] ) {
            componentGame->getPlayer()->setWeaponTypeByIndex(2);
            ComponentSound::playSound(
                soundPackage->getByLabel("switchWeapon"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }

        if (keyboard[SDL_SCANCODE_4] ) {
            componentGame->getPlayer()->setWeaponTypeByIndex(3);
            ComponentSound::playSound(
                soundPackage->getByLabel("switchWeapon"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }
    }

    if (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) {
        player->nextWeapon();
    }

    if (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER) {
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

        ComponentsManager::get()->getComponentCamera()->getCamera()->getFrustum()->setup(
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

void ComponentGameInput::handleKeyboardMovingPlayer()
{
    auto componentInput = ComponentsManager::get()->getComponentInput();
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();
    Uint8 *keyboard = componentInput->getKeyboard();

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
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->getKeyboard();
    auto input = ComponentsManager::get()->getComponentInput();
    auto render = ComponentsManager::get()->getComponentRender();
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();

    if (
        (keyboard[SDL_SCANCODE_TAB] && event->type == SDL_KEYDOWN) ||
        (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSTICK)
     ) {
        componentGame->selectClosestObject3DFromPlayer();
    }

    if (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSTICK) {
        ComponentsManager::get()->getComponentRender()->setSelectedObject(nullptr);
        lockRightStick = false;
    }

    auto rightStickDirection = Vertex3D(input->getControllerAxisRightX(), input->getControllerAxisRightY(), 0);

    if (render->getSelectedObject() != nullptr && !lockRightStick && rightStickDirection.getModule() > 0.5) {
        lockRightStick = true;

        auto currentClosestObject = componentGame->getClosesObject3DDirection(player->getPosition(), rightStickDirection, true, true);

        if (currentClosestObject != nullptr) {
            ComponentsManager::get()->getComponentRender()->setSelectedObject(currentClosestObject);

            ComponentSound::playSound(
                BUFFERS->soundPackage->getByLabel("tic"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }
    }

    if (rightStickDirection.getModule() < 0.5 && lockRightStick) {
        lockRightStick = false;
    }
}

void ComponentGameInput::handleGamePadMovingPlayer()
{
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();

    if (!EngineSetup::get()->GAMEPAD_CONTROLLER_ENABLED) return;

    if (player->isStucked()) return;

    auto componentInput = ComponentsManager::get()->getComponentInput();

    float speed = player->power * Brakeza3D::get()->getDeltaTime();
    speed = std::clamp(speed, 0.f, player->maxVelocity);

    player->setVelocity(
        player->getVelocity() +
        Vertex3D(componentInput->getControllerAxisLeftX() * speed, componentInput->getControllerAxisLeftY() * speed, 0)
    );
}

void ComponentGameInput::handleDashMovement(SDL_Event *event)
{
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();

    if (player->isStucked()) return;

    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->getKeyboard();
    auto input = ComponentsManager::get()->getComponentInput();

    if (
        (keyboard[SDL_SCANCODE_LSHIFT] && event->type == SDL_KEYDOWN) ||
        (event->type == SDL_CONTROLLERBUTTONDOWN && input->getControllerButtonB())
    ) {
        const float dashEnergyCost = 10;

        if (player->getEnergy() < dashEnergyCost) {
            return;
        }

        ComponentSound::playSound(
            BUFFERS->soundPackage->getByLabel("dash"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );

        Brakeza3D::get()->addObject3D(
            new ParticleEmitterFireworks(player->getPosition(), 3, 1520, 1, 0.01, Color::green(), 6, 25),
            "fireworks" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel()
        );

        float speed = player->dashPower * Brakeza3D::get()->getDeltaTime();

        player->setVelocity(
            player->getVelocity() +
            Vertex3D(input->getControllerAxisLeftX() * speed, input->getControllerAxisLeftY() * speed, 0)
        );

        player->useEnergy(dashEnergyCost);
    }
}

void ComponentGameInput::handleEnergyShield(SDL_Event *event)
{
    auto componentInput = ComponentsManager::get()->getComponentInput();
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();

    if (componentInput->getControllerAxisTriggerLeft() > 0.20 && !player->isEnergyShieldEnabled()) {
        if (!player->isAllowEnergyShield()) {
            return;
        }

        player->setEnergyShieldEnabled(true);
        player->getShieldModel()->setEnabled(true);
        ComponentSound::playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("energyShield"),
            1,
            0
        );
    }

    if (componentInput->getControllerAxisTriggerLeft() < 0.20 && player->isEnergyShieldEnabled()) {
        player->setEnergyShieldEnabled(false);
    }
}



void ComponentGameInput::handleMakeReflection(SDL_Event *event)
{
    auto componentInput = ComponentsManager::get()->getComponentInput();
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();

    if (event->cbutton.type == SDL_CONTROLLERBUTTONDOWN && componentInput->getControllerButtonA()) {
        if (player->isAllowedMakeReflections()) {
            player->makeReflection();
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

void ComponentGameInput::handleCheckPadConnection(SDL_Event *pEvent)
{
    if (pEvent->type == SDL_CONTROLLERDEVICEADDED ) {
        ComponentsManager::get()->getComponentInput()->initJoystick();
    }

    if (pEvent->type == SDL_CONTROLLERDEVICEREMOVED ) {
    }
}

void ComponentGameInput::handleBomb(SDL_Event *event)
{
    auto componentInput = ComponentsManager::get()->getComponentInput();
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();

    if (event->cbutton.type == SDL_CONTROLLERBUTTONDOWN && componentInput->getControllerButtonX()) {
        auto weapon = player->getWeaponTypeByLabel("Bomb");
        weapon->onUpdate();
        weapon->shootBomb(player, player->getPosition());
    }
}

float ComponentGameInput::getControllerAxisThreshold() const {
    return controllerAxisThreshold;
}
