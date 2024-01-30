#include "../../include/Components/ComponentInput.h"
#include "../../include/ComponentsManager.h"


ComponentInput::ComponentInput()
:
    mouseMotion(false),
    mouseLeftButton(false),
    mouseRightButton(false),
    keyboard(nullptr),
    controllerButtonA(0),
    controllerButtonB(0),
    controllerButtonX(0),
    controllerButtonY(0),
    controllerAxisLeftX(0),
    controllerAxisLeftY(0),
    controllerAxisRightX(0),
    controllerAxisRightY(0),
    controllerAxisTriggerLeft(0),
    controllerAxisTriggerRight(0),
    mouseButtons(0),
    mouseX(0),
    mouseY(0),
    relativeRendererMouseX(0),
    relativeRendererMouseY(0),
    gameController(nullptr),
    keyUpEvent(false),
    keyDownEvent(false)
{
}

void ComponentInput::onStart()
{
    Logging::head("ComponentInput onStart");
    setEnabled(true);
    initJoystick();
}

void ComponentInput::preUpdate()
{
    updateKeyboardMapping();
    updateMouseMapping();

    keyDownEvent = false;
    keyUpEvent = false;
}

void ComponentInput::onUpdate()
{
    if (!isEnabled()) return;
    handleKeyboardMovingCamera();
}

void ComponentInput::postUpdate()
{
}

void ComponentInput::onEnd()
{
}

void ComponentInput::onSDLPollEvent(SDL_Event *e, bool &finish)
{
    updateMouseStates(e);
    handleCheckPadConnection(e);
    updateGamePadStates();
    updateKeyboardStates(e);
    handleWindowEvents(e, finish);
    handleToggleImGui(e);

    if (!isEnabled()) return;

    handleMouse(e);
    handleProjectileDemo(e);
}

void ComponentInput::handleMouse(SDL_Event *event) const {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    if (mouseMotion && isLeftMouseButtonPressed()) {
        if (event->type == SDL_MOUSEMOTION) {
            ComponentsManager::get()->getComponentCamera()->getCamera()->Yaw((float) event->motion.xrel);
            ComponentsManager::get()->getComponentCamera()->getCamera()->Pitch((float) event->motion.yrel);
        }
    }
}

void ComponentInput::handleKeyboardMovingCamera() const
{
    if (keyboard[SDL_SCANCODE_W]) {
        ComponentsManager::get()->getComponentCamera()->getCamera()->MoveForward();
    }
    if (keyboard[SDL_SCANCODE_S]) {
        ComponentsManager::get()->getComponentCamera()->getCamera()->MoveBackward();
    }
    if (keyboard[SDL_SCANCODE_A]) {
        ComponentsManager::get()->getComponentCamera()->getCamera()->StrafeLeft();
    }
    if (keyboard[SDL_SCANCODE_D]) {
        ComponentsManager::get()->getComponentCamera()->getCamera()->StrafeRight();
    }
    if (keyboard[SDL_SCANCODE_RIGHT]) {
        ComponentsManager::get()->getComponentCamera()->getCamera()->TurnRight();
    }
    if (keyboard[SDL_SCANCODE_LEFT]) {
        ComponentsManager::get()->getComponentCamera()->getCamera()->TurnLeft();
    }
    if (keyboard[SDL_SCANCODE_DOWN]) {
        ComponentsManager::get()->getComponentCamera()->getCamera()->PitchUp();
    }
    if (keyboard[SDL_SCANCODE_UP]) {
        ComponentsManager::get()->getComponentCamera()->getCamera()->PitchDown();
    }
}

void ComponentInput::handleWindowEvents(SDL_Event *e, bool &end) {
    if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_CLOSE) {
        end = true;
    }
}

void ComponentInput::updateKeyboardMapping()
{
    this->keyboard = (unsigned char *) SDL_GetKeyboardState(nullptr);
}

void ComponentInput::updateMouseStates(SDL_Event *event)
{
    if (event->type == SDL_MOUSEMOTION) {
        mouseMotion = true;
    } else {
        mouseMotion = false;
    }
}

void ComponentInput::handleProjectileDemo(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN) {
        if (keyboard[SDL_SCANCODE_1]) {
            ComponentsManager::get()->getComponentCollisions()->demoProjectile(0);
        }
        if (keyboard[SDL_SCANCODE_2]) {
            ComponentsManager::get()->getComponentCollisions()->demoProjectile(1);
        }
        if (keyboard[SDL_SCANCODE_3]) {
            ComponentsManager::get()->getComponentCollisions()->demoProjectile(2);
        }
        if (keyboard[SDL_SCANCODE_4]) {
            ComponentsManager::get()->getComponentCollisions()->demoProjectile(3);
        }
        if (keyboard[SDL_SCANCODE_5]) {
            ComponentsManager::get()->getComponentCollisions()->demoProjectile(4);
        }
        if (keyboard[SDL_SCANCODE_6]) {
            ComponentsManager::get()->getComponentCollisions()->demoProjectile(5);
        }
    }
}

bool ComponentInput::isLeftMouseButtonPressed() const
{
    return mouseLeftButton;
}

bool ComponentInput::isRightMouseButtonPressed() const
{
    return mouseRightButton;
}

void ComponentInput::updateMouseMapping()
{
    auto *window = ComponentsManager::get()->getComponentWindow();

    mouseLeftButton = false;
    mouseRightButton = false;

    this->mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);

    if ((mouseButtons & SDL_BUTTON_LMASK) != 0) {
        mouseLeftButton = true;
    }

    if ((mouseButtons & SDL_BUTTON_RMASK) != 0) {
        mouseRightButton = true;
    }

    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(window->getRenderer(), &windowWidth, &windowHeight);
    relativeRendererMouseX = (EngineSetup::get()->screenWidth * mouseX ) / windowWidth;
    relativeRendererMouseY = (EngineSetup::get()->screenHeight * mouseY) / windowHeight;
}

bool ComponentInput::isClickLeft() const {
    return mouseLeftButton;
}

bool ComponentInput::isClickRight() const {
    return mouseRightButton;
}

int ComponentInput::getRelativeRendererMouseX() const {
    return relativeRendererMouseX;
}

int ComponentInput::getRelativeRendererMouseY() const {
    return relativeRendererMouseY;
}

bool ComponentInput::isMouseMotion() const {
    return mouseMotion;
}

void ComponentInput::updateGamePadStates()
{
    if (gameController == nullptr) return;

    controllerButtonA = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_A);
    controllerButtonB = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_B);
    controllerButtonX = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_X);
    controllerButtonY = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_Y);

    controllerAxisLeftX = (float) SDL_GameControllerGetAxis(gameController, (SDL_GameControllerAxis)0)/32768.0f;
    controllerAxisLeftY = (float) SDL_GameControllerGetAxis(gameController, (SDL_GameControllerAxis)1)/32768.0f;

    controllerAxisRightX = (float) SDL_GameControllerGetAxis(gameController, (SDL_GameControllerAxis)2)/32768.0f;
    controllerAxisRightY = (float) SDL_GameControllerGetAxis(gameController, (SDL_GameControllerAxis)3)/32768.0f;

    controllerAxisTriggerLeft = (float) SDL_GameControllerGetAxis(gameController, (SDL_GameControllerAxis)4) / 32768.0f;
    controllerAxisTriggerRight = (float) SDL_GameControllerGetAxis(gameController, (SDL_GameControllerAxis)5) / 32768.0f;
}

bool ComponentInput::isAnyControllerButtonPressed() const
{
    if (controllerButtonA || controllerButtonB || controllerButtonX || controllerButtonY) {
        return true;
    }

    return false;
}

void ComponentInput::initJoystick()
{
    if ( SDL_NumJoysticks() < 1 ) {
        Logging::Message("[WARNING] No game pad controller connected!" );
    } else {
        gameController = SDL_GameControllerOpen( 0 );

        if (gameController == nullptr) {
            printf( "Warning: Unable to open game pad controller! SDL Error: %s\n", SDL_GetError() );
            return;
        }
        Logging::Message("Game Pad Controller Name: %s\n", SDL_JoystickNameForIndex(0));
    }
}

float ComponentInput::getControllerAxisTriggerLeft() const {
    return controllerAxisTriggerLeft;
}

float ComponentInput::getControllerAxisTriggerRight() const {
    return controllerAxisTriggerRight;
}

Uint8 *ComponentInput::getKeyboard() const {
    return keyboard;
}

Uint8 ComponentInput::getControllerButtonA() const {
    return controllerButtonA;
}

Uint8 ComponentInput::getControllerButtonB() const {
    return controllerButtonB;
}

Uint8 ComponentInput::getControllerButtonX() const {
    return controllerButtonX;
}

Uint8 ComponentInput::getControllerButtonY() const {
    return controllerButtonY;
}

float ComponentInput::getControllerAxisLeftX() const {
    return controllerAxisLeftX;
}

float ComponentInput::getControllerAxisLeftY() const {
    return controllerAxisLeftY;
}

float ComponentInput::getControllerAxisRightX() const {
    return controllerAxisRightX;
}

float ComponentInput::getControllerAxisRightY() const {
    return controllerAxisRightY;
}

void ComponentInput::handleToggleImGui(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN) {
        if (keyboard[SDL_SCANCODE_F1]) {
            EngineSetup::get()->IMGUI_ENABLED = !EngineSetup::get()->IMGUI_ENABLED;
        }
    }
}

bool ComponentInput::isCharPressed(const char *character)
{

    if (character == nullptr)
        return false;

    SDL_Scancode keyCode = SDL_GetScancodeFromName(character);

    if (keyCode != SDL_SCANCODE_UNKNOWN && keyboard[keyCode])
        return true;

    return false;
}

bool ComponentInput::isKeyEventDown() const
{
    return keyDownEvent;
}

bool ComponentInput::isKeyEventUp() const
{
    return keyUpEvent;
}

void ComponentInput::updateKeyboardStates(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN) {
        keyDownEvent = true;
    }

    if (event->type == SDL_KEYUP) {
        keyUpEvent = true;
    }
}

_SDL_GameController *ComponentInput::getGameController() const {
    return gameController;
}

void ComponentInput::handleCheckPadConnection(SDL_Event *pEvent)
{
    if (pEvent->type == SDL_CONTROLLERDEVICEADDED ) {
        initJoystick();
    }

    if (pEvent->type == SDL_CONTROLLERDEVICEREMOVED ) {
        if (gameController != nullptr) {
            Logging::Message("Removing Controller Device...");
            SDL_GameControllerClose(gameController);
            gameController = nullptr;
        }
    }
}