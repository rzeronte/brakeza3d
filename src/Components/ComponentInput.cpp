#include <cmath>
#include "../../include/Components/ComponentInput.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"
#include "../../imgui/ImGuizmo.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/AddOns/GUIAddonMenu.h"
#include "../../include/Render/JSONSerializerRegistry.h"
#include "../../include/Serializers/Mesh3DSerializer.h"

void ComponentInput::onStart()
{
    Component::onStart();

    setEnabled(true);
    InitJoystick();
}

void ComponentInput::preUpdate()
{
    Component::preUpdate();

    wantCaptureKeyboard = Config::get()->ENABLE_IMGUI && ImGui::GetIO().WantCaptureKeyboard;
    wantCaptureMouse    = Config::get()->ENABLE_IMGUI && ImGui::GetIO().WantCaptureMouse;

    ResetKeyboardMapping();
    ResetMouseMapping();

    keyDownEvent           = false;
    keyUpEvent             = false;
    leftClickConsumedByUI  = false;
}

void ComponentInput::onUpdate()
{
    Component::onUpdate();

    if (!isEnabled()) return;

    if (keyboardEnabled  && !ImGui::GetIO().WantCaptureKeyboard) {
        HandleKeyboardMovingCamera();
    }

    if (mouseEnabled) {
        ApplySmoothedMouseLook();
    }
}

void ComponentInput::postUpdate()
{
    Component::postUpdate();

    keyboardEvents.clear();
    mouseRightButtonUp = false;
    mouseButtonDown    = false;
    mouseButtonUp      = false;
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

    if (event->type == SDL_MOUSEWHEEL && event->wheel.y != 0) {
        auto camera = Components::get()->Camera()->getCamera();
        camera->MoveForward((float)event->wheel.y * Config::get()->WALKING_SPEED * 3.0f);
    }

    // Reset accumulator + filter on middle-button-down so each drag starts clean
    // (no residual from previous drag pushing the camera when you re-engage).
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_MIDDLE) {
        pendingMouseLookDx = pendingMouseLookDy = 0.0f;
        smoothedMouseLookDx = smoothedMouseLookDy = 0.0f;
    }

    // Only accumulate raw deltas here — actual camera update happens per-frame in ApplySmoothedMouseLook.
    if (event->type == SDL_MOUSEMOTION && isMiddleMouseButtonPressed()) {
        pendingMouseLookDx += (float)event->motion.xrel;
        pendingMouseLookDy += (float)event->motion.yrel;
    }
}

void ComponentInput::ApplySmoothedMouseLook()
{
    if (!Config::get()->MOUSE_LOOK) return;
    if (Config::get()->ENABLE_IMGUI && ImGui::GetIO().WantCaptureMouse) {
        pendingMouseLookDx = pendingMouseLookDy = 0.0f;
        return;
    }

    // Per-frame low-pass. Larger alpha = snappier; smaller = smoother/more lag.
    constexpr float kAlpha = 0.5f;
    smoothedMouseLookDx = kAlpha * pendingMouseLookDx + (1.0f - kAlpha) * smoothedMouseLookDx;
    smoothedMouseLookDy = kAlpha * pendingMouseLookDy + (1.0f - kAlpha) * smoothedMouseLookDy;
    pendingMouseLookDx = pendingMouseLookDy = 0.0f;

    // Ignore sub-pixel residues from the decaying filter tail once the drag ends.
    if (std::fabs(smoothedMouseLookDx) < 0.05f && std::fabs(smoothedMouseLookDy) < 0.05f) {
        smoothedMouseLookDx = smoothedMouseLookDy = 0.0f;
        return;
    }

    if (!isMiddleMouseButtonPressed()) return;

    auto camera = Components::get()->Camera()->getCamera();
    const bool isShift = keyboard[SDL_SCANCODE_LSHIFT] || keyboard[SDL_SCANCODE_RSHIFT];

    if (isShift) {
        // Blender-style pan: mouse right → view content moves right → camera translates left.
        // getRotation() is world→camera; its transpose gives camera-basis in world space
        // (same convention as getGLMMat4ViewMatrix uses to derive forward).
        const float panSpeed = Config::get()->MOUSE_SENSITIVITY * 2.0f;
        auto invRot = camera->getRotation().getTranspose();
        const auto right = invRot * Vertex3D(1, 0, 0);
        const auto up    = invRot * Vertex3D(0, 1, 0);
        camera->setPosition(camera->getPosition()
            + right.getScaled(-smoothedMouseLookDx * panSpeed)
            + up.getScaled(    smoothedMouseLookDy * panSpeed));
    } else {
        camera->Yaw(-smoothedMouseLookDx * Config::get()->MOUSE_SENSITIVITY);
        camera->Pitch(smoothedMouseLookDy * Config::get()->MOUSE_SENSITIVITY);

        camera->setRotation(M3::getMatrixRotationForEulerAngles(
            camera->getPitch(),
            camera->getYaw(),
            camera->getRoll()
        ));
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
            camera->MoveForward(Config::get()->WALKING_SPEED);
        }
    }

    if (keyboard[SDL_SCANCODE_DOWN]) {
        if (isShiftPressed) {
            camera->MoveVertical(-Config::get()->WALKING_SPEED);
        } else {
            camera->MoveBackward(Config::get()->WALKING_SPEED);
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
    if (event->type == SDL_MOUSEWHEEL) {
        mouseWheelY = event->wheel.y;
    }

    if (event->type == SDL_MOUSEMOTION) {
        mouseMotion = true;
        mouseMotionXRel = (float) event->motion.xrel;
        mouseMotionYRel = (float) event->motion.yrel;
        if (mouseLeftButton)  drag      = true;
        if (mouseRightButton) rightDrag = true;
    }

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        mouseButtonDown = true;
        if (event->button.button == SDL_BUTTON_RIGHT) rightDrag = false;
    }

    if (event->type == SDL_MOUSEBUTTONUP) {
        if (event->button.button == SDL_BUTTON_LEFT) mouseButtonUp = true;
        drag = false;
    }

    if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_RIGHT) {
        mouseRightButtonUp = true;
    }
}

void ComponentInput::ResetMouseMapping()
{
    auto window = Components::get()->Window();

    mouseLeftButton = false;
    mouseRightButton = false;
    mouseMiddleButton = false;
    mouseWheelY = 0;

    // Siempre actualizar posición raw y botón central, independientemente de ImGui
    unsigned int rawButtons = SDL_GetMouseState(&rawMouseX, &rawMouseY);
    if ((rawButtons & SDL_BUTTON_MMASK) != 0) {
        mouseMiddleButton = true;
    }

    if (Config::get()->ENABLE_IMGUI && ImGui::GetIO().WantCaptureMouse) {
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
            SceneLoader::ClearWorld();
        }
        if (keyboard[SDL_SCANCODE_F4]) {
            SceneLoader::CleanWorld();
        }

        if (keyboard[SDL_SCANCODE_F5]) {
            Config::get()->ENABLE_IMGUI = !Config::get()->ENABLE_IMGUI;
        }

        auto *window = Components::get()->Window();

        if (!Brakeza::get()->GUI()->isSoloActive()) {
            if (keyboard[SDL_SCANCODE_F6]) {
                window->setImGuiConfig(Config::ImGUIConfigs::DEFAULT);
            }
            if (keyboard[SDL_SCANCODE_F7]) {
                window->setImGuiConfig(Config::ImGUIConfigs::CODING);
            }
            if (keyboard[SDL_SCANCODE_F8]) {
                window->setImGuiConfig(Config::ImGUIConfigs::DESIGN);
            }
        }
        if (keyboard[SDL_SCANCODE_F9]) {
            Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::SCENE_OBJECTS)->isOpen = !Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::SCENE_OBJECTS)->isOpen;
        }
        if (keyboard[SDL_SCANCODE_F10]) {
            Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::OBJECT_PROPS)->isOpen = !Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::OBJECT_PROPS)->isOpen;
        }
        if (keyboard[SDL_SCANCODE_F11]) {
            Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::PROJECT_SETTINGS)->isOpen = !Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::PROJECT_SETTINGS)->isOpen;
        }
        if (keyboard[SDL_SCANCODE_F12]) {
            Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::BROWSER)->isOpen = !Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::BROWSER)->isOpen;
        }

        bool ctrlHeld  = keyboard[SDL_SCANCODE_LCTRL] || keyboard[SDL_SCANCODE_RCTRL];
        bool lctrlHeld = keyboard[SDL_SCANCODE_LCTRL];
        if (ctrlHeld && keyboard[SDL_SCANCODE_P]) {
            Config::get()->ENABLE_POST_PROCESSING_CHAIN = !Config::get()->ENABLE_POST_PROCESSING_CHAIN;
        }
        if (lctrlHeld && keyboard[SDL_SCANCODE_GRAVE]) {
            Brakeza::get()->GUI()->ToggleSoloFocusedWindow();
        }

        if (event->type == SDL_KEYDOWN && event->key.repeat == 0) {
            auto* sel = Components::get()->Render()->getSelectedObject();
            if (sel != nullptr) {
                auto* camera = Components::get()->Camera()->getCamera();
                float r = GUIAddonMenu::snapRadius;
                Vertex3D pos = sel->getPosition();

                if (event->key.keysym.scancode == SDL_SCANCODE_INSERT) {
                    camera->setPosition(Vertex3D(pos.x, pos.y, pos.z + r));
                    camera->setRotationFromEulerAngles(0.f, 0.f, 0.f);
                }
                if (event->key.keysym.scancode == SDL_SCANCODE_HOME) {
                    camera->setPosition(Vertex3D(pos.x, pos.y + r, pos.z));
                    camera->setRotationFromEulerAngles(89.f, 0.f, 0.f);
                }
                if (event->key.keysym.scancode == SDL_SCANCODE_PAGEUP) {
                    camera->setPosition(Vertex3D(pos.x + r, pos.y, pos.z));
                    camera->setRotationFromEulerAngles(0.f, -90.f, 0.f);
                }
            }
        }

        if (event->type == SDL_KEYDOWN &&
            event->key.repeat == 0 &&
            event->key.keysym.scancode == SDL_SCANCODE_RETURN &&
            (event->key.keysym.mod & KMOD_ALT))
        {
            Config::get()->FULLSCREEN = !Config::get()->FULLSCREEN;
            Components::get()->Window()->ToggleFullScreen();
        }

        // Transformation shortcuts (T, R, S) y clone (Shift+D) — solo con un objeto seleccionado
        auto selectedObject = Components::get()->Render()->getSelectedObject();
        if (selectedObject != nullptr) {
            if (keyboard[SDL_SCANCODE_T]) {
                window->setGuiZmoOperation(ImGuizmo::OPERATION::TRANSLATE);
            }
            if (keyboard[SDL_SCANCODE_R]) {
                window->setGuiZmoOperation(ImGuizmo::OPERATION::ROTATE);
            }
            if (keyboard[SDL_SCANCODE_S]) {
                window->setGuiZmoOperation(ImGuizmo::OPERATION::SCALE);
            }

            bool shiftHeld = keyboard[SDL_SCANCODE_LSHIFT] || keyboard[SDL_SCANCODE_RSHIFT];
            if (shiftHeld && keyboard[SDL_SCANCODE_D] && event->key.repeat == 0) {
                auto* asMesh = dynamic_cast<Mesh3D*>(selectedObject);
                if (asMesh) {
                    auto* cloned = Mesh3DSerializer::CloneMesh3D(asMesh);
                    Components::get()->Render()->setSelectedObject(cloned);
                } else {
                    cJSON* json = JSONSerializerRegistry::instance().serialize(selectedObject);
                    cJSON_AddNumberToObject(json, "type", (int)selectedObject->getTypeObject());
                    Object3D* cloned = JSONSerializerRegistry::instance().deserialize(json);
                    cJSON_Delete(json);
                    if (cloned) {
                        Vertex3D p = cloned->getPosition();
                        cloned->setPosition(Vertex3D(p.x + 1.0f, p.y, p.z + 1.0f));
                        cloned->setName(Brakeza::UniqueObjectLabel(cloned->getName().c_str()));
                        Components::get()->Render()->setSelectedObject(cloned);
                    }
                }
            }
        }

        // Delete (X / Supr) y deselect (ESC) — funcionan con cualquier número de seleccionados
        auto render = Components::get()->Render();
        if (!render->getSelectedObjects().empty()) {
            if (keyboard[SDL_SCANCODE_X]) {
                // Copia local para evitar problemas si clearSelection invalida la referencia
                const auto toDelete = render->getSelectedObjects();
                for (auto* o : toDelete) {
                    o->setRemoved(true);
                }
                render->clearSelection();
            }
            if (keyboard[SDL_SCANCODE_ESCAPE]) {
                render->clearSelection();
            }
        }
    }
}

bool ComponentInput::isCharPressed(const char *character) const
{
    if (character == nullptr || wantCaptureKeyboard) return false;

    SDL_Scancode keyCode = SDL_GetScancodeFromName(character);

    if (keyCode != SDL_SCANCODE_UNKNOWN && keyboard[keyCode]) return true;

    return false;
}

bool ComponentInput::isCharFirstEventDown(const char *character)
{
    if (character == nullptr || wantCaptureKeyboard)
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