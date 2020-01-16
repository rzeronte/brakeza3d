//
// Created by darkhead on 8/1/20.
//

#include "../../headers/Components/ComponentInput.h"
#include "../../imgui/imgui.h"
#include "../../imgui/examples/imgui_impl_sdl.h"
#include "../../headers/ComponentsManager.h"
#include <iostream>

ComponentInput::ComponentInput() {
    setEnabled( true );
}

void ComponentInput::onStart() {
    std::cout << "ComponentInput onStart" << std::endl;
}

void ComponentInput::preUpdate() {

}

void ComponentInput::onUpdate()
{
    if ( !isEnabled() ) return;
    handleKeyboardMovingCamera();
}

void ComponentInput::postUpdate() {

}

void ComponentInput::onEnd() {

}

void ComponentInput::onSDLPollEvent(SDL_Event *e, bool &finish)
{
    ImGui_ImplSDL2_ProcessEvent(e);

    if ( !isEnabled() ) return;

    updateKeyboardMapping();
    updateMouseStates(e);
    handleMouse(e);
    handleWindowEvents(e, finish);
}


void ComponentInput::handleMouse(SDL_Event *event)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    // Camera rotation
    if (MouseMotion) {
        MouseMotion = false;
        if (event->type == SDL_MOUSEMOTION) {
            ComponentsManager::get()->getComponentCamera()->getCamera()->Yaw(event->motion.xrel);
            ComponentsManager::get()->getComponentCamera()->getCamera()->Pitch(event->motion.yrel);
        }
    }

    // Firing
    if ( MousePressed ) {
    }
}

void ComponentInput::handleKeyboardMovingCamera()
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

void ComponentInput::handleWindowEvents(SDL_Event *e, bool &end)
{
    if (e->type == SDL_WINDOWEVENT) {
        switch (e->window.event) {
            case SDL_WINDOWEVENT_SHOWN:
                break;
            case SDL_WINDOWEVENT_HIDDEN:
                break;
            case SDL_WINDOWEVENT_EXPOSED:
                break;
            case SDL_WINDOWEVENT_MOVED:
                break;
            case SDL_WINDOWEVENT_RESIZED:
                break;
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:
                break;
            case SDL_WINDOWEVENT_RESTORED:
                break;
            case SDL_WINDOWEVENT_ENTER:
                break;
            case SDL_WINDOWEVENT_LEAVE:
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                break;
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

void ComponentInput::updateKeyboardMapping()
{
    this->keyboard = (unsigned char *) SDL_GetKeyboardState(NULL);
}

void ComponentInput::updateMouseStates(SDL_Event *event)
{
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        MousePressed = true;
    }
    if (event->button.button == SDL_BUTTON_LEFT) {
        leftButton = true;
        rightButton = false;
    }

    if (event->button.button == SDL_BUTTON_RIGHT) {
        rightButton = true;
        leftButton = false;
    }

    if (event->type == SDL_MOUSEBUTTONUP) {
        MousePressed = false;
    }

    if (event->type == SDL_MOUSEMOTION) {
        MouseMotion = true;
    }
}

bool ComponentInput::isEnabled() const {
    return enabled;
}

void ComponentInput::setEnabled(bool enabled) {
    ComponentInput::enabled = enabled;
}

