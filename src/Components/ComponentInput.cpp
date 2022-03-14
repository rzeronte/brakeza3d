#include "../../include/Components/ComponentInput.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ComponentInput::ComponentInput() {
    setEnabled(true);
}

void ComponentInput::onStart() {
    Logging::Log("ComponentInput onStart", "ComponentInput");
}

void ComponentInput::preUpdate() {
    updateKeyboardMapping();
    updateMouseMapping();

}

void ComponentInput::onUpdate() {
    if (!isEnabled()) return;
    handleKeyboardMovingCamera();
}

void ComponentInput::postUpdate() {

}

void ComponentInput::onEnd() {

}

void ComponentInput::onSDLPollEvent(SDL_Event *e, bool &finish) {
    updateMouseStates(e);
    handleWindowEvents(e, finish);

    controllerButtonA = SDL_GameControllerGetButton(ComponentsManager::get()->getComponentWindow()->gameController, SDL_CONTROLLER_BUTTON_A);
    controllerButtonB = SDL_GameControllerGetButton(ComponentsManager::get()->getComponentWindow()->gameController, SDL_CONTROLLER_BUTTON_B);
    controllerButtonX = SDL_GameControllerGetButton(ComponentsManager::get()->getComponentWindow()->gameController, SDL_CONTROLLER_BUTTON_X);
    controllerButtonY = SDL_GameControllerGetButton(ComponentsManager::get()->getComponentWindow()->gameController, SDL_CONTROLLER_BUTTON_Y);

    if (!isEnabled()) return;

    handleMouse(e);
    handleProjectileDemo(e);
}


void ComponentInput::handleMouse(SDL_Event *event) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    // Camera rotation
    if (mouseMotion && isLeftMouseButtonPressed()) {
        if (event->type == SDL_MOUSEMOTION) {
            ComponentsManager::get()->getComponentCamera()->getCamera()->Yaw(event->motion.xrel);
            ComponentsManager::get()->getComponentCamera()->getCamera()->Pitch(event->motion.yrel);
        }
    }
}

void ComponentInput::handleKeyboardMovingCamera() const {
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
    if (e->type == SDL_WINDOWEVENT) {
        switch (e->window.event) {
            case SDL_WINDOWEVENT_CLOSE:
                end = true;
                break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
            case SDL_WINDOWEVENT_TAKE_FOCUS:
                break;
            case SDL_WINDOWEVENT_HIT_TEST:
                break;
#endif
            default:
                break;
        }
    }
}

void ComponentInput::updateKeyboardMapping() {
    this->keyboard = (unsigned char *) SDL_GetKeyboardState(nullptr);
}

void ComponentInput::updateMouseStates(SDL_Event *event) {
    if (event->type == SDL_MOUSEMOTION) {
        mouseMotion = true;
    } else {
        mouseMotion = false;
    }
}

bool ComponentInput::isEnabled() const {
    return enabled;
}

void ComponentInput::setEnabled(bool enabled) {
    ComponentInput::enabled = enabled;
}

void ComponentInput::handleProjectileDemo(SDL_Event *event) {
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

bool ComponentInput::isLeftMouseButtonPressed() const {
    return mouseLeftButton;
}

bool ComponentInput::isRightMouseButtonPressed() const {
    return mouseRightButton;
}

void ComponentInput::updateMouseMapping() {
    auto *window = ComponentsManager::get()->getComponentWindow();

    mouseLeftButton = false;
    mouseRightButton = false;

    this->mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);

    if ((mouseButtons & SDL_BUTTON_LMASK) != 0) {
        mouseLeftButton = true;
        mousePressed = true;
    }

    if ((mouseButtons & SDL_BUTTON_RMASK) != 0) {
        mouseRightButton = true;
    }

    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(window->renderer, &windowWidth, &windowHeight);
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


void ComponentInput::handleJostickAxis() {
    const int JOYSTICK_DEAD_ZONE = 1000;


}