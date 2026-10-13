#include <algorithm>
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
    setRelativeMouseMode(false);   // nunca dejar el ratón capturado al cerrar
}

void ComponentInput::setRelativeMouseMode(bool enabled)
{
    SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
}

bool ComponentInput::isRelativeMouseMode() const
{
    return SDL_GetRelativeMouseMode() == SDL_TRUE;
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

bool ComponentInput::IsEditorMouseCameraAllowed() const
{
    if (!Components::get()->Scripting()->isExecuting()) return true;
    return keyboard[SDL_SCANCODE_RCTRL] != 0;
}

void ComponentInput::HandleMouseLook(SDL_Event *event)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    if (!IsEditorMouseCameraAllowed()) return;

    if (event->type == SDL_MOUSEWHEEL && event->wheel.y != 0) {
        auto camera = Components::get()->Camera()->getCamera();
        const float step = (float)event->wheel.y * Config::get()->WALKING_SPEED * 3.0f;
        cameraFocusDistance = std::max(0.1f, cameraFocusDistance - step);

        if (Config::get()->ZOOM_TO_CURSOR) {
            // Zoom direccional: avanzar por el rayo que atraviesa el cursor. El centro de la pantalla
            // coincide con el avance normal; cuanto más lejos del centro, más se va hacia ese lado.
            int mx = 0, my = 0;
            SDL_GetMouseState(&mx, &my);
            const auto window = Components::get()->Window();
            const float w = (float) std::max(1, window->getWidth());
            const float h = (float) std::max(1, window->getHeight());
            const glm::mat4 proj = Camera3D::getGLMMat4ProjectionMatrix();
            const float ndcX = 2.0f * (float) mx / w - 1.0f;
            const float ndcY = 1.0f - 2.0f * (float) my / h;
            const Vertex3D dirCam = Vertex3D(ndcX / proj[0][0], ndcY / proj[1][1], -1.0f).getNormalize();
            const Vertex3D dirWorld = camera->getRotation().getTranspose() * dirCam;
            camera->setPosition(camera->getPosition() + dirWorld.getScaled(step));
        } else {
            camera->MoveForward(step);
        }
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

float ComponentInput::GetCameraFocusDistance() const
{
    // Con un objeto seleccionado se enfoca a él (profundidad a lo largo de la dirección de la cámara)
    if (const auto *selected = Components::get()->Render()->getSelectedObject(); selected != nullptr) {
        auto camera = Components::get()->Camera()->getCamera();
        const Vertex3D fwd = camera->getRotation().getTranspose() * Vertex3D(0, 0, -1);
        const float depth = (selected->getPosition() - camera->getPosition()) * fwd;
        if (depth > 0.1f) return depth;
    }
    return cameraFocusDistance;
}

void ComponentInput::ApplySmoothedMouseLook()
{
    if (!IsEditorMouseCameraAllowed()) return;
    if (Config::get()->ENABLE_IMGUI && ImGui::GetIO().WantCaptureMouse) {
        pendingMouseLookDx = pendingMouseLookDy = 0.0f;
        return;
    }

    // Deltas sin filtrar de este frame: el pan los usa tal cual para que la escena siga al cursor
    const float rawDx = pendingMouseLookDx;
    const float rawDy = pendingMouseLookDy;

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
        //
        // Unidades de mundo por píxel a la distancia de enfoque: así el punto que hay bajo el cursor
        // lo sigue exactamente (1 px de ratón = 1 px de pantalla) y el movimiento se afina solo al
        // acercarse. Antes era una constante por píxel (0.15 u), imposible de controlar de cerca.
        const float h = (float) std::max(1, Components::get()->Window()->getHeight());
        const float tanHalfV = 1.0f / Camera3D::getGLMMat4ProjectionMatrix()[1][1];
        const float worldPerPixel = 2.0f * GetCameraFocusDistance() * tanHalfV / h;
        const float panSpeed = worldPerPixel * Config::get()->MOUSE_PAN_SENSITIVITY;
        auto invRot = camera->getRotation().getTranspose();
        const auto right = invRot * Vertex3D(1, 0, 0);
        const auto up    = invRot * Vertex3D(0, 1, 0);
        camera->setPosition(camera->getPosition()
            + right.getScaled(-rawDx * panSpeed)
            + up.getScaled(    rawDy * panSpeed));
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
    // Alt+Tab / clic en otra aplicación a mitad de arrastre: ni la captura del ratón salva la suelta,
    // que ocurre ya fuera. Cancelar lo que estuviera en curso (arrastre, selección por recuadro).
    if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
        CancelMouseInteraction();
    }
}

void ComponentInput::CancelMouseInteraction()
{
    drag               = false;
    rightDrag          = false;
    mouseLeftButton    = false;
    mouseRightButton   = false;
    mouseMiddleButton  = false;
    mouseButtonDown    = false;
    mouseButtonUp      = false;
    mouseRightButtonUp = false;
    mouseButtonsSuspended = true;
    Components::get()->Render()->getSelectionManager().cancelRectSelection();
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
        mouseButtonsSuspended = false;   // pulsación nueva real: vuelve a valer el estado de SDL
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

    // ¿Cursor realmente sobre nuestra ventana y con foco? (SDL mantiene el foco de ratón con los
    // eventos ENTER/LEAVE; mientras hay captura -botón pulsado- se conserva aunque salga).
    SDL_Window* sdlWindow = window->getWindow();
    mouseInWindow = sdlWindow != nullptr
                 && SDL_GetMouseFocus()    == sdlWindow
                 && SDL_GetKeyboardFocus() == sdlWindow;

    // Botones suspendidos tras perder el foco (ver CancelMouseInteraction): el estado de SDL puede
    // ser un "pulsado" rancio. Se levanta en cuanto SDL deja de marcar botones (vuelve a ser fiable)
    // o con una pulsación nueva (SDL_MOUSEBUTTONDOWN, en UpdateMouseStates).
    const unsigned int anyButton = SDL_BUTTON_LMASK | SDL_BUTTON_RMASK | SDL_BUTTON_MMASK;
    if (mouseButtonsSuspended && (rawButtons & anyButton) == 0) mouseButtonsSuspended = false;

    // Salirse de la ventana con un botón pulsado cancela el arrastre / la selección por recuadro
    // (a petición: que no siga "arrastrando" fuera ni al volver). Con el ratón capturado (botón
    // pulsado) SDL no garantiza el evento LEAVE, así que se compara la posición GLOBAL del cursor
    // (válida aunque esté capturado) con el rectángulo de cliente de la ventana.
    if (!mouseButtonsSuspended && (rawButtons & anyButton) != 0 && sdlWindow != nullptr) {
        int gx, gy, wx, wy, ww, wh;
        SDL_GetGlobalMouseState(&gx, &gy);
        SDL_GetWindowPosition(sdlWindow, &wx, &wy);
        SDL_GetWindowSize(sdlWindow, &ww, &wh);
        if (gx < wx || gy < wy || gx >= wx + ww || gy >= wy + wh) {
            CancelMouseInteraction();
        }
    }

    if (!mouseButtonsSuspended && (rawButtons & SDL_BUTTON_MMASK) != 0) {
        mouseMiddleButton = true;
    }

    if (Config::get()->ENABLE_IMGUI && ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    this->mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);

    if (!mouseButtonsSuspended && (mouseButtons & SDL_BUTTON_LMASK) != 0) {
        mouseLeftButton = true;
    }

    if (!mouseButtonsSuspended && (mouseButtons & SDL_BUTTON_RMASK) != 0) {
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
        // F2: ventana de Logs/Console. Recargar scripts y limpiar la escena (Clear / Clean) ya no tienen tecla:
        // están en el menú de scripts
        if (keyboard[SDL_SCANCODE_F2]) {
            Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::LOGGING)->isOpen = !Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::LOGGING)->isOpen;
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

        // Delete (solo X, como en Blender: Supr no borra a propósito) y deselect (ESC) — funcionan con cualquier número de seleccionados
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