#include "../../include/Components/ComponentInput.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"
#include "../../imgui/ImGuizmo.h"

void ComponentInput::onStart()
{
    Component::onStart();

    setEnabled(true);
    InitJoystick();
}

void ComponentInput::preUpdate()
{
    Component::preUpdate();

    ResetKeyboardMapping();
    ResetMouseMapping();

    keyDownEvent = false;
    keyUpEvent = false;
}

void ComponentInput::onUpdate()
{
    Component::onUpdate();

    if (!isEnabled()) return;

    if (keyboardEnabled  && !ImGui::GetIO().WantCaptureKeyboard) {
        HandleKeyboardMovingCamera();
    }
}

void ComponentInput::postUpdate()
{
    Component::postUpdate();

    keyboardEvents.clear();
}

void ComponentInput::onEnd()
{
}

void ComponentInput::onSDLPollEvent(SDL_Event *e, bool &finish)
{
    UpdateMouseStates(e);
    HandleCheckPadConnection(e);
    UpdateGamePadStates();
    UpdateKeyboardStates(e);
    HandleWindowEvents(e, finish);

    bool isExecuting = Components::get()->Scripting()->isExecuting();
    bool editorModifier = keyboard[SDL_SCANCODE_RCTRL];  // Right Ctrl enables editor shortcuts during execution

    // Shortcuts work when: not executing OR RCtrl is held during execution
    if (!isExecuting || editorModifier) {
        HandleGUIShortCuts(e);
    }

    if (!isEnabled()) return;

    if (mouseEnabled) {
        HandleMouseLook(e);
    }
}

void ComponentInput::HandleMouseLook(SDL_Event *event)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    if (!Config::get()->MOUSE_LOOK) return;

    if (mouseMotion && isRightMouseButtonPressed()) {
        if (event->type == SDL_MOUSEMOTION) {
            auto camera = Components::get()->Camera()->getCamera();


            camera->Yaw(-event->motion.xrel * Config::get()->MOUSE_SENSITIVITY);
            camera->Pitch(event->motion.yrel * Config::get()->MOUSE_SENSITIVITY);

            camera->setRotation(M3::getMatrixRotationForEulerAngles(
                camera->getPitch(),
                camera->getYaw(),
                camera->getRoll()
            ));
        }
    }

    if (event->type == SDL_MOUSEWHEEL) {
        if (event->wheel.y > 0) {
            Components::get()->Camera()->getCamera()->MoveForward(-Config::get()->WALKING_SPEED * 5);
        } else if (event->wheel.y < 0) {
            Components::get()->Camera()->getCamera()->MoveBackward(-Config::get()->WALKING_SPEED * 5);
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

void ComponentInput::HandleKeyboardMovingCamera() const
{
    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }

    auto camera = Components::get()->Camera()->getCamera();
    bool isShiftPressed = keyboard[SDL_SCANCODE_LSHIFT] || keyboard[SDL_SCANCODE_RSHIFT];

    if (keyboard[SDL_SCANCODE_UP]) {
        if (isShiftPressed) {
            camera->MoveVertical(Config::get()->WALKING_SPEED);
        } else {
            camera->MoveForward(-Config::get()->WALKING_SPEED);
        }
    }

    if (keyboard[SDL_SCANCODE_DOWN]) {
        if (isShiftPressed) {
            camera->MoveVertical(-Config::get()->WALKING_SPEED);
        } else {
            camera->MoveBackward(-Config::get()->WALKING_SPEED);
        }
    }

    if (keyboard[SDL_SCANCODE_LEFT]) {
        camera->StrafeLeft();
    }

    if (keyboard[SDL_SCANCODE_RIGHT]) {
        camera->StrafeRight();
    }
}

void ComponentInput::HandleWindowEvents(SDL_Event *e, bool &end)
{
    if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_CLOSE) {
        end = true;
    }
}

void ComponentInput::ResetKeyboardMapping()
{
    keyboard = (unsigned char *) SDL_GetKeyboardState(nullptr);
}

void ComponentInput::UpdateMouseStates(SDL_Event *event)
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

void ComponentInput::ResetMouseMapping()
{
    auto window = Components::get()->Window();

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

    relativeRendererMouseX = Config::get()->screenWidth * mouseX / window->getWidthRender();
    relativeRendererMouseY = Config::get()->screenHeight * mouseY / window->getHeightRender();

    mouseMotion = false;
    mouseMotionXRel = 0;
    mouseMotionYRel = 0;
}

void ComponentInput::UpdateGamePadStates()
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

void ComponentInput::InitJoystick()
{
    if ( SDL_NumJoysticks() < 1 ) {
        LOG_WARNING("[Input] No gamepad controller connected!");
    } else {
        gameController = SDL_GameControllerOpen(0);

        if (gameController == nullptr) {
            LOG_ERROR("[Input] Unable to open game pad controller: %s", SDL_GetError());
            return;
        }
        LOG_MESSAGE("[Input] Game Pad Controller Name: %s\n", SDL_JoystickNameForIndex(0));
    }
}

void ComponentInput::HandleGUIShortCuts(SDL_Event *event) const
{
    if (event->type == SDL_KEYDOWN) {
        auto scripting = Components::get()->Scripting();

        if (keyboard[SDL_SCANCODE_F1]) {
            if (scripting->isExecuting()) {
                scripting->StopLUAScripts();
            } else {
                scripting->PlayLUAScripts();
            }
        }
        if (keyboard[SDL_SCANCODE_F2]) {
            scripting->ReloadLUAScripts();
        }
        if (keyboard[SDL_SCANCODE_F3]) {
            SceneLoader::ClearScene();
        }
        if (keyboard[SDL_SCANCODE_F4]) {
            SceneLoader::CleanScene();
        }

        if (keyboard[SDL_SCANCODE_F5]) {
            Config::get()->ENABLE_IMGUI = !Config::get()->ENABLE_IMGUI;
        }

        auto *window = Components::get()->Window();

        if (keyboard[SDL_SCANCODE_F6]) {
            window->setImGuiConfig(Config::ImGUIConfigs::DEFAULT);
        }
        if (keyboard[SDL_SCANCODE_F7]) {
            window->setImGuiConfig(Config::ImGUIConfigs::CODING);
        }
        if (keyboard[SDL_SCANCODE_F8]) {
            window->setImGuiConfig(Config::ImGUIConfigs::DESIGN);
        }

        if (keyboard[SDL_SCANCODE_F11]) {
            Config::get()->FULLSCREEN = !Config::get()->FULLSCREEN;
            Components::get()->Window()->ToggleFullScreen();
        }

        // Transformation shortcuts (T, R, S), delete (X) and deselect (ESC) - only when an object is selected
        auto selectedObject = Components::get()->Render()->getSelectedObject();
        if (selectedObject != nullptr) {
            if (keyboard[SDL_SCANCODE_T]) {
                window->setGuiZmoOperation(ImGuizmo::OPERATION::TRANSLATE);
            }
            if (keyboard[SDL_SCANCODE_R]) {
                window->setGuiZmoOperation(ImGuizmo::OPERATION::ROTATE);
            }
            if (keyboard[SDL_SCANCODE_S]) {
                window->setGuiZmoOperation(ImGuizmo::OPERATION::SCALE_X);
            }
            if (keyboard[SDL_SCANCODE_X]) {
                selectedObject->setRemoved(true);
                Components::get()->Render()->setSelectedObject(nullptr);
            }
            if (keyboard[SDL_SCANCODE_ESCAPE]) {
                Components::get()->Render()->setSelectedObject(nullptr);
            }
        }
    }
}

bool ComponentInput::isCharPressed(const char *character) const
{
    if (character == nullptr) return false;

    SDL_Scancode keyCode = SDL_GetScancodeFromName(character);

    if (keyCode != SDL_SCANCODE_UNKNOWN && keyboard[keyCode]) return true;

    return false;
}

bool ComponentInput::isCharFirstEventDown(const char *character)
{
    if (character == nullptr)
        return false;

    SDL_Keycode keyCode = SDL_GetKeyFromScancode(SDL_GetScancodeFromName(character));

    auto it = keyboardEvents.find(keyCode);

    return it != keyboardEvents.end() && it->second;
}

void ComponentInput::UpdateKeyboardStates(SDL_Event *event)
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

void ComponentInput::HandleCheckPadConnection(SDL_Event *pEvent)
{
    if (pEvent->type == SDL_CONTROLLERDEVICEADDED ) {
        InitJoystick();
    }

    if (pEvent->type == SDL_CONTROLLERDEVICEREMOVED ) {
        if (gameController != nullptr) {
            LOG_MESSAGE("Removing Controller Device...");
            SDL_GameControllerClose(gameController);
            gameController = nullptr;
        }
    }
}

void ComponentInput::HandleDeleteSelectedObject(SDL_Event *e) const
{
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (e->type == SDL_KEYDOWN) {
        if (keyboard[SDL_SCANCODE_X]) {
            auto o = Components::get()->Render()->getSelectedObject();
            if (o != nullptr) {
                o->setRemoved(true);
                Components::get()->Render()->setSelectedObject(nullptr);
            }
        }
    }
}


void ComponentInput::setKeyboardEnabled(bool value)
{
    keyboardEnabled = value;
}

void ComponentInput::setMouseEnabled(bool value)
{
    mouseEnabled = value;
}

void ComponentInput::setPadEnabled(bool value)
{
    padEnabled = value;
}