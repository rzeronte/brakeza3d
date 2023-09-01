#include "ComponentGameInput.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ComponentGameInput::ComponentGameInput() : controllerAxisThreshold(0.1), lockRightStick(false)
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

void ComponentGameInput::onUpdate()
{
}

void ComponentGameInput::postUpdate()
{
}

void ComponentGameInput::onEnd()
{
}

void ComponentGameInput::onSDLPollEvent(SDL_Event *event, bool &finish)
{
    if (!isEnabled()) return;

    this->handleInGameInput(event, finish);
}

void ComponentGameInput::handleInGameInput(SDL_Event *event, bool &end)
{
    this->handleCheckPadConnection(event);

    auto state = ComponentsManager::get()->getComponentGame()->getGameState();

    if (state == EngineSetup::GameState::MENU) {
        handleMenuKeyboard(event, end);
        return;
    }

    this->handlePressKeyGameStates(event);

    if (state == EngineSetup::GameState::GAMING) {
        handleFindClosestObject3D(event);
        handleWeaponSelector(event);
        handleDashMovement(event);
        handleEnergyShield(event);
        handleBomb(event);
        handleShield(event);
        handleMakeReflection(event);
        updateWeaponStatus(event);
    }

    //this->handleZoom(event);
}
void ComponentGameInput::updateWeaponStatus(SDL_Event *event)
{
    auto game = ComponentsManager::get()->getComponentGame();

    auto player = game->getPlayer();

    if (event->type == SDL_CONTROLLERBUTTONUP && event->cbutton.button == SDL_CONTROLLER_BUTTON_A) {
        player->getWeapon()->setStatus(WeaponStatus::RELEASED);
        player->getRayLight().setReach(0);
    }

    if (event->cbutton.type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_A) {
        player->getWeapon()->setStatus(WeaponStatus::PRESSED);
    }

    if (event->type == SDL_KEYUP && event->key.keysym.sym == SDLK_SPACE) {
        player->getRayLight().setReach(0);
    }
}

void ComponentGameInput::handleEscape(SDL_Event *event)
{
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->getKeyboard();
    auto game = ComponentsManager::get()->getComponentGame();
    auto componentSound = ComponentsManager::get()->getComponentSound();

    auto gameState = game->getGameState();
    auto escPressed = (keyboard[SDL_SCANCODE_ESCAPE] && event->type == SDL_KEYDOWN);
    auto buttonControllerPressed = event->cbutton.type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_GUIDE;

    if (escPressed || buttonControllerPressed) {
        componentSound->sound("soundMenuClick", EngineSetup::SoundChannels::SND_GLOBAL, 0);

        if (gameState == EngineSetup::GameState::MENU && game->getLevelLoader()->isLevelStartedToPlay()) {
            game->getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);
            game->makeFadeToGameState(EngineSetup::GameState::GAMING, true);
            return;
        }

        if (gameState == EngineSetup::GameState::STORE) {
            game->gameState = EngineSetup::GameState::PRESS_KEY_BY_WIN;

            return;
        }
        if (gameState == EngineSetup::GameState::GAMING_TUTORIAL) {
            game->setGameState(EngineSetup::GameState::GAMING);
            return;
        }

        if (gameState == EngineSetup::GameState::INTRO)  {
            return;
        }

        if (gameState == EngineSetup::GameState::COUNTDOWN ||
            gameState == EngineSetup::GameState::PRESS_KEY_NEW_LEVEL ||
            gameState == EngineSetup::GameState::PRESS_KEY_BY_DEAD ||
                gameState == EngineSetup::GameState::PRESS_KEY_BY_WIN
        ) return;

        game->getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);
        game->makeFadeToGameState(EngineSetup::GameState::MENU, true);

        SDL_WarpMouseInWindow(
            ComponentsManager::get()->getComponentWindow()->getWindow(),
            SETUP->screenWidth / 2,
            SETUP->screenHeight / 2
        );
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
            ComponentsManager::get()->getComponentSound()->sound("soundMenuClick", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }
    }

    if (keyboard[SDL_SCANCODE_UP] || (event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP)) {
        if (currentOption > 0) {
            componentMenu->decreaseMenuOption();
            ComponentsManager::get()->getComponentSound()->sound("soundMenuClick", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }
    }

    // Execute Menu option
    if (keyboard[SDL_SCANCODE_RETURN] || componentInput->getControllerButtonA()) {
        if (menuOptions[currentOption].getAction() == ComponentMenu::MNU_EXIT) {
            end = true;
            return;
        }

        if (menuOptions[currentOption].getAction() == ComponentMenu::MNU_NEW_GAME) {
            ComponentsManager::get()->getComponentGame()->pressedKeyForNewGameOrResumeGame();
        }

        if (menuOptions[currentOption].getAction() == ComponentMenu::MNU_HELP) {
            componentGame->getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);

            componentGame->makeFadeToGameState(EngineSetup::GameState::HELP, true);
        }

        if (menuOptions[currentOption].getAction() == ComponentMenu::MNU_CREDITS) {
            componentGame->getFadeToGameState()->setSpeed(FADE_SPEED_FADEOUT_TIME);
            componentGame->makeFadeToGameState(EngineSetup::GameState::CREDITS, true);
        }
    }
}

void ComponentGameInput::handleFire() const
{
    auto componentInput = ComponentsManager::get()->getComponentInput();
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();

    player->getRayLight().setEnabled(false);

    Uint8 *keyboard = componentInput->getKeyboard();
    if (keyboard[SDL_SCANCODE_SPACE] || componentInput->getControllerAxisTriggerRight() > this->controllerAxisThreshold) {
        // controller intensity: componentInput->getControllerAxisTriggerRight()
        player->shoot(2.5f);
    }
}

void ComponentGameInput::handleWeaponSelector(SDL_Event *event)
{
    auto soundPackage =ComponentsManager::get()->getComponentSound()->getSoundPackage();

    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->getKeyboard();
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();

    if (event->type == SDL_KEYDOWN) {
        if (keyboard[SDL_SCANCODE_1]) {
            componentGame->getPlayer()->setWeaponTypeByIndex(0);
        }

        if (keyboard[SDL_SCANCODE_2]) {
            componentGame->getPlayer()->setWeaponTypeByIndex(1);
            ComponentsManager::get()->getComponentSound()->sound("switchWeapon", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }

        if (keyboard[SDL_SCANCODE_3] ) {
            componentGame->getPlayer()->setWeaponTypeByIndex(2);
            ComponentsManager::get()->getComponentSound()->sound("switchWeapon", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }

        /*if (keyboard[SDL_SCANCODE_4] ) {
            componentGame->getPlayer()->setWeaponTypeByIndex(3);
            ComponentsManager::get()->getComponentSound()->sound("switchWeapon", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }
        if (keyboard[SDL_SCANCODE_5] ) {
            componentGame->getPlayer()->setWeaponTypeByIndex(4);
            ComponentsManager::get()->getComponentSound()->sound("switchWeapon", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }*/

        if (keyboard[SDL_SCANCODE_0] ) {
            player->nextWeapon();
        }
        if (keyboard[SDL_SCANCODE_9] ) {
            player->previousWeapon();
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
    auto player = ComponentsManager::get()->getComponentGame()->getPlayer();

    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->getKeyboard();

    float speed = player->power * Brakeza3D::get()->getDeltaTime();

    if (ComponentsManager::get()->getComponentGame()->getStoreManager()->isItemEnabled(EngineSetup::StoreItems::ITEM_EXTRA_POWER)) {
        speed *= 1.75f;
    }

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
        ComponentsManager::get()->getComponentGame()->shaderEdgeObject->setEnabled(false);
        ComponentsManager::get()->getComponentGame()->shaderEdgeObject->setObject(nullptr);
        lockRightStick = false;
    }

    auto rightStickDirection = Vertex3D(input->getControllerAxisRightX(), input->getControllerAxisRightY(), 0);

    if (render->getSelectedObject() != nullptr && !lockRightStick && rightStickDirection.getModule() > 0.5) {
        lockRightStick = true;

        auto currentClosestObject = componentGame->getClosesObject3DDirection(player->getPosition(), rightStickDirection, true, true);

        if (currentClosestObject != nullptr) {
            ComponentsManager::get()->getComponentRender()->setSelectedObject(currentClosestObject);
            ComponentsManager::get()->getComponentGame()->shaderEdgeObject->setEnabled(true);
            ComponentsManager::get()->getComponentGame()->shaderEdgeObject->setObject((Mesh3D*) currentClosestObject);

            ComponentsManager::get()->getComponentSound()->sound("tic", EngineSetup::SoundChannels::SND_GLOBAL, 0);
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

    if (ComponentsManager::get()->getComponentGame()->getStoreManager()->isItemEnabled(EngineSetup::StoreItems::ITEM_EXTRA_POWER)) {
        speed *= 1.75f;
    }

    speed = std::clamp(speed, 0.f, player->maxVelocity);

    player->setVelocity(
        player->getVelocity() +
        Vertex3D(componentInput->getControllerAxisLeftX() * speed, componentInput->getControllerAxisLeftY() * speed, 0)
    );
}

void ComponentGameInput::handleDashMovement(SDL_Event *event)
{
    auto input = ComponentsManager::get()->getComponentInput();
    Uint8 *keyboard = input->getKeyboard();
    auto leftShiftPressed = keyboard[SDL_SCANCODE_LSHIFT] && event->type == SDL_KEYDOWN;
    auto controlButtonPressed = event->type == SDL_CONTROLLERBUTTONDOWN && input->getControllerButtonB();

    if ( leftShiftPressed || controlButtonPressed) {
        ComponentsManager::get()->getComponentGame()->getPlayer()->dashMovement();
    }
}

void ComponentGameInput::handleEnergyShield(SDL_Event *event)
{
    auto componentInput = ComponentsManager::get()->getComponentInput();
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->getKeyboard();
    const bool triggerLeftOn = componentInput->getControllerAxisTriggerLeft() >= 0.20;
    const bool actionKey = keyboard[SDL_SCANCODE_I] && event->type == SDL_KEYDOWN;
    const bool actionKeyReleased = !keyboard[SDL_SCANCODE_I] && event->type == SDL_KEYUP;
    const bool leftAxisTriggerRelease = componentInput->getControllerAxisTriggerLeft() < 0.20;
    const bool isKeyboardAction = event->type == SDL_KEYUP || event->type == SDL_KEYDOWN;
    const float percentageReached =  player->getEnergy() * 100 / player->getStartEnergy();

    if (isKeyboardAction) {
        if (actionKey && !player->isEnergyShieldEnabled() && percentageReached > 10) {
            if (!player->isAllowEnergyShield()) {
                return;
            }

            player->setEnergyShieldEnabled(true);
            Brakeza3D::get()->addObject3D(new ShockWave(player->getPosition(), 0.75, 50, 0.5, true), Brakeza3D::uniqueObjectLabel("shockWave"));
            ComponentsManager::get()->getComponentSound()->sound("energyShield", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }

        if (actionKeyReleased && player->isEnergyShieldEnabled()) {
            player->setEnergyShieldEnabled(false);
        }

        return;
    }
    if (triggerLeftOn && !player->isEnergyShieldEnabled()) {
        if (!player->isAllowEnergyShield()) {
            return;
        }

        player->setEnergyShieldEnabled(true);
        Brakeza3D::get()->addObject3D(new ShockWave(player->getPosition(), 0.75, 50, 0.5, true), Brakeza3D::uniqueObjectLabel("shockWave"));
        ComponentsManager::get()->getComponentSound()->sound("energyShield", EngineSetup::SoundChannels::SND_GLOBAL, 0);
    }

    if (leftAxisTriggerRelease && player->isEnergyShieldEnabled() && !keyboard[SDL_SCANCODE_I]) {
        player->setEnergyShieldEnabled(false);
    }
}

void ComponentGameInput::handleMakeReflection(SDL_Event *event)
{
    auto componentInput = ComponentsManager::get()->getComponentInput();
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();

    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->getKeyboard();
    const bool reflectionKeyPressed = event->type == SDL_KEYDOWN && keyboard[SDL_SCANCODE_P];
    const bool controllerXButtonPressed = event->cbutton.type == SDL_CONTROLLERBUTTONDOWN && componentInput->getControllerButtonA();

    if (controllerXButtonPressed || reflectionKeyPressed) {
        auto weapon = player->getWeaponTypeByLabel("reflection");
        weapon->onUpdate();
        weapon->shootHologram(player, player->getPosition());
    }
}

void ComponentGameInput::handlePressKeyGameStates(SDL_Event *event)
{
    auto state = ComponentsManager::get()->getComponentGame()->getGameState();
    bool isButtonGuidedPressed = event->cbutton.type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_GUIDE;
    auto componentSound = ComponentsManager::get()->getComponentSound();
    auto game = ComponentsManager::get()->getComponentGame();
    auto componentInput = ComponentsManager::get()->getComponentInput();

    Uint8 *keyboard = componentInput->getKeyboard();

    bool controllerLeft = event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    bool controllerRight = event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    bool controllerUpPad = event->type == SDL_CONTROLLERBUTTONDOWN && event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP;

    const bool cursorLeft = event->type == SDL_KEYDOWN && keyboard[SDL_SCANCODE_LEFT];
    const bool cursorRight = event->type == SDL_KEYDOWN && keyboard[SDL_SCANCODE_RIGHT];
    const bool keyStorePressed = event->type == SDL_KEYDOWN && keyboard[SDL_SCANCODE_UP];
    const bool enter = event->type == SDL_KEYDOWN && keyboard[SDL_SCANCODE_RETURN];

    if ((state == EngineSetup::GameState::PRESS_KEY_BY_WIN) && (enter || isButtonGuidedPressed)) {
        game->pressedKeyForWin();
        return;
    }

    if (state == EngineSetup::GameState::PRESS_KEY_BY_WIN && (controllerUpPad || keyStorePressed)) {
        game->setGameState(EngineSetup::STORE);
        componentSound->sound("wrench", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        return;
    }

    if (state == EngineSetup::GameState::GAMING_TUTORIAL && (enter || isButtonGuidedPressed)) {
        game->setGameState(EngineSetup::GAMING);
        componentSound->sound("tic", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        return;
    }

    if (state == EngineSetup::GameState::STORE) {
        if (cursorLeft || controllerLeft) {
            game->getStoreManager()->decreaseItemSelected();
            componentSound->sound("tic", EngineSetup::SoundChannels::SND_GLOBAL, 0);
            return;
        }

        if (cursorRight || controllerRight) {
            game->getStoreManager()->increaseItemSelected();
            componentSound->sound("tic", EngineSetup::SoundChannels::SND_GLOBAL, 0);
            return;
        }

        if (enter || componentInput->getControllerButtonA()) {
            game->getStoreManager()->buyCurrentSelected();
            return;
        }
    }

    if (state == EngineSetup::GameState::SPACESHIP_SELECTOR) {
        if (cursorLeft || controllerLeft) {
            game->decreaseSpaceshipSelected();
            componentSound->sound("tic", EngineSetup::SoundChannels::SND_GLOBAL, 0);
            return;
        }

        if (cursorRight || controllerRight) {
            game->increaseSpaceshipSelected();
            componentSound->sound("tic", EngineSetup::SoundChannels::SND_GLOBAL, 0);
            return;
        }

        if (enter || componentInput->getControllerButtonA()) {
            ComponentsManager::get()->getComponentGame()->selectSpaceshipAndStartGame();
            return;
        }
    }

    if (state == EngineSetup::GameState::HELP) {
        if (cursorLeft || controllerLeft) {
            game->decreaseHelpImage();
            componentSound->sound("tic", EngineSetup::SoundChannels::SND_GLOBAL, 0);
            return;
        }

        if (cursorRight || controllerRight) {
            game->increaseHelpImage();
            componentSound->sound("tic", EngineSetup::SoundChannels::SND_GLOBAL, 0);
            return;
        }
    }

    if ((state == EngineSetup::GameState::PRESS_KEY_NEW_LEVEL || state == EngineSetup::PRESS_KEY_PREVIOUS_LEVEL)) {
        if (isButtonGuidedPressed || enter) {
            game->pressedKeyForBeginLevel();
            return;
        }

        if (cursorLeft || controllerLeft) {
            componentSound->sound("soundMenuClick", EngineSetup::SoundChannels::SND_GLOBAL, 0);
            game->getLevelLoader()->decreaseTutorialImage();
            return;
        }

        if (cursorRight || controllerRight) {
            componentSound->sound("soundMenuClick", EngineSetup::SoundChannels::SND_GLOBAL, 0);
            game->getLevelLoader()->increaseTutorialImage();
            return;
        }
    }

    if (state == EngineSetup::GameState::PRESS_KEY_GAMEOVER && (enter || isButtonGuidedPressed)) {
        game->pressedKeyForFinishGameAndRestart();
        return;
    }

    if (state == EngineSetup::GameState::PRESS_KEY_BY_DEAD && (enter || isButtonGuidedPressed)) {
        game->pressedKeyByDead();
        return;
    }

    this->handleEscape(event);

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

    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->getKeyboard();
    const bool bombKeyPressed = event->type == SDL_KEYDOWN && keyboard[SDL_SCANCODE_O];
    const bool controllerXButtonPressed = event->cbutton.type == SDL_CONTROLLERBUTTONDOWN && componentInput->getControllerButtonX();

    if (controllerXButtonPressed || bombKeyPressed) {
        auto weapon = player->getWeaponTypeByLabel("bomb");
        weapon->onUpdate();
        weapon->shootBomb(player, player->getPosition());
    }
}

void ComponentGameInput::handleShield(SDL_Event *event)
{
    auto componentInput = ComponentsManager::get()->getComponentInput();
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto player = componentGame->getPlayer();

    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->getKeyboard();
    const bool bombKeyPressed = event->type == SDL_KEYDOWN && keyboard[SDL_SCANCODE_L];
    const bool controllerYButtonPressed = event->cbutton.type == SDL_CONTROLLERBUTTONDOWN && componentInput->getControllerButtonY();

    if (controllerYButtonPressed || bombKeyPressed) {
        auto weapon = player->getWeaponTypeByLabel("shield");
        weapon->onUpdate();
        weapon->shootShield(player, player->getPosition());
    }
}

float ComponentGameInput::getControllerAxisThreshold() const {
    return controllerAxisThreshold;
}
