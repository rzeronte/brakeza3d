#include "../../include/Components/ComponentInput.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Render/Logging.h"

ComponentInput::ComponentInput()
:
    mouseMotion(false),
    mouseMotionXRel(0),
    mouseMotionYRel(0),
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
    keyDownEvent(false),
    drag(false)
{
}

void ComponentInput::onStart()
{
    Logging::Message("ComponentInput onStart");
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
    keyboardEvents.clear();
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
    handleToggleKeys(e);

    if (!isEnabled()) return;

    handleDeleteSelectedObject(e);
    handleMouse(e);
    handleProjectileDemo(e);
}

void ComponentInput::handleMouse(SDL_Event *event)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    if (!EngineSetup::get()->MOUSE_LOOK) return;

    if (mouseMotion && isRightMouseButtonPressed()) {
        if (event->type == SDL_MOUSEMOTION) {
            auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();
            camera->Yaw((float) -event->motion.xrel);
            camera->Pitch((float) event->motion.yrel);
            camera->setRotation(M3::getMatrixRotationForEulerAngles(camera->getPitch(), camera->getYaw(), camera->getRoll()));
        }
    }

    if (event->type == SDL_MOUSEWHEEL) {
        if (event->wheel.y > 0) {
            ComponentsManager::get()->getComponentCamera()->getCamera()->MoveForward(-EngineSetup::get()->WALKING_SPEED * 5);
        } else if (event->wheel.y < 0) {
            ComponentsManager::get()->getComponentCamera()->getCamera()->MoveBackward(-EngineSetup::get()->WALKING_SPEED * 5);
        }
    }

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        mouseButtonDown = true;
    }
    if (event->type == SDL_MOUSEBUTTONUP) {
        mouseButtonUp = true;
        drag = false;
    }

}

void ComponentInput::handleKeyboardMovingCamera() const
{
    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }

    auto *camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    bool isShiftPressed = keyboard[SDL_SCANCODE_LSHIFT] || keyboard[SDL_SCANCODE_RSHIFT];

    if (keyboard[SDL_SCANCODE_UP]) {
        if (isShiftPressed) {
            camera->MoveVertical(EngineSetup::get()->WALKING_SPEED);
        } else {
            camera->MoveForward(-EngineSetup::get()->WALKING_SPEED);
        }
    }

    if (keyboard[SDL_SCANCODE_DOWN]) {
        if (isShiftPressed) {
            camera->MoveVertical(-EngineSetup::get()->WALKING_SPEED);
        } else {
            camera->MoveBackward(-EngineSetup::get()->WALKING_SPEED);
        }
    }

    if (keyboard[SDL_SCANCODE_LEFT]) {
        camera->StrafeLeft();
    }

    if (keyboard[SDL_SCANCODE_RIGHT]) {
        camera->StrafeRight();
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
        mouseMotionXRel = (float) event->motion.xrel;
        mouseMotionYRel = (float) event->motion.yrel;
        if (mouseLeftButton) {
            drag = true;
        }
    }
}

void ComponentInput::handleProjectileDemo(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN) {
        if (keyboard[SDL_SCANCODE_F9]) {
            int type = Tools::random(0, 5);
            ComponentCollisions::demoProjectile(type);
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
    mouseButtonDown = false;
    mouseButtonUp = false;

    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

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

    mouseMotion = false;
    mouseMotionXRel = 0;
    mouseMotionYRel = 0;
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

    controllerPadUp = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_UP);
    controllerPadDown = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    controllerPadLeft = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    controllerPadRight = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

    controllerShoulderLeft = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
    controllerShoulderRight = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

    controllerButtonBack = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_BACK);
    controllerButtonGuide = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_GUIDE);
    controllerButtonStart = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_START);

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

void ComponentInput::handleToggleKeys(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN) {
        auto scripting = ComponentsManager::get()->getComponentScripting();

        if (keyboard[SDL_SCANCODE_F1]) {
            if (scripting->getStateLUAScripts() == EngineSetup::LuaStateScripts::LUA_STOP) {
                scripting->playLUAScripts();
            } else {
                scripting->stopLUAScripts();
            }
        }
        if (keyboard[SDL_SCANCODE_F2]) {
            scripting->reloadLUAScripts();
        }
        if (keyboard[SDL_SCANCODE_F3]) {
            SceneLoader::clearScene();
        }
        if (keyboard[SDL_SCANCODE_F4]) {
            EngineSetup::get()->IMGUI_ENABLED = !EngineSetup::get()->IMGUI_ENABLED;
        }

        auto *window = ComponentsManager::get()->getComponentWindow();

        if (keyboard[SDL_SCANCODE_F5]) {
            window->ImGuiConfigChanged = ImGUIConfigs::DEFAULT;
        }
        if (keyboard[SDL_SCANCODE_F6]) {
            window->ImGuiConfigChanged = ImGUIConfigs::CODING;
        }
        if (keyboard[SDL_SCANCODE_F7]) {
            window->ImGuiConfigChanged = ImGUIConfigs::DESIGN;
        }

        if (keyboard[SDL_SCANCODE_F11]) {
            EngineSetup::get()->FULLSCREEN = !EngineSetup::get()->FULLSCREEN;
            ComponentsManager::get()->getComponentWindow()->toggleFullScreen();
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

bool ComponentInput::isCharFirstEventDown(const char *character)
{
    if (character == nullptr)
        return false;

    SDL_Keycode keyCode = SDL_GetKeyFromScancode(SDL_GetScancodeFromName(character));

    auto it = keyboardEvents.find(keyCode);

    return (it != keyboardEvents.end()) && it->second;
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

    if (event->type == SDL_KEYDOWN && event->key.repeat == 0) {
        keyboardEvents[event->key.keysym.sym] = true;
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

float ComponentInput::getMouseMotionXRel() const {
    return mouseMotionXRel;
}

float ComponentInput::getMouseMotionYRel() const {
    return mouseMotionYRel;
}

bool ComponentInput::isMouseButtonUp() const {
    return mouseButtonUp;
}

bool ComponentInput::isMouseButtonDown() const {
    return mouseButtonDown;
}

bool ComponentInput::isDrag() const {
    return drag;
}

void ComponentInput::handleDeleteSelectedObject(SDL_Event *e)
{
    if (e->type == SDL_KEYDOWN) {
        if (keyboard[SDL_SCANCODE_DELETE]) {
            auto o = ComponentsManager::get()->getComponentRender()->getSelectedObject();
            o->setRemoved(true);
            ComponentsManager::get()->getComponentRender()->setSelectedObject(nullptr);
        }
    }
}

bool ComponentInput::isGameControllerEnabled()
{
    if (gameController != nullptr) return true;

    return false;
}

Uint8 ComponentInput::getControllerPadUp() const {
    return controllerPadUp;
}

Uint8 ComponentInput::getControllerPadDown() const {
    return controllerPadDown;
}

Uint8 ComponentInput::getControllerPadLeft() const {
    return controllerPadLeft;
}

Uint8 ComponentInput::getControllerPadRight() const {
    return controllerPadRight;
}

Uint8 ComponentInput::getControllerShoulderLeft() const {
    return controllerShoulderLeft;
}

Uint8 ComponentInput::getControllerShoulderRight() const {
    return controllerShoulderRight;
}

Uint8 ComponentInput::getControllerButtonBack() const {
    return controllerButtonBack;
}

Uint8 ComponentInput::getControllerButtonGuide() const {
    return controllerButtonGuide;
}

Uint8 ComponentInput::getControllerButtonStart() const {
    return controllerButtonStart;
}
