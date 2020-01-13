
#include "../../headers/Input/InputController.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/EngineBuffers.h"
#include "../../headers/Brakeza3D.h"

InputController::InputController()
{
}

void InputController::handleMouse(SDL_Event *event)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    // Camera rotation
    if (MouseMotion) {
        MouseMotion = false;
        if (event->type == SDL_MOUSEMOTION) {
            //Brakeza3D::get()->getCamera3D()->Yaw(event->motion.xrel);
            //Brakeza3D::get()->getCamera3D()->Pitch(event->motion.yrel);
        }
    }

    // Firing
    if ( MousePressed ) {
    }
}

void InputController::handleMovingCamera(SDL_Event *, bool &)
{
    /*if (keyboard[SDL_SCANCODE_W]) {
        Brakeza3D::get()->getCamera3D()->MoveForward();
    }
    if (keyboard[SDL_SCANCODE_S]) {
        Brakeza3D::get()->getCamera3D()->MoveBackward();
    }
    if (keyboard[SDL_SCANCODE_A]) {
        Brakeza3D::get()->getCamera3D()->StrafeLeft();
    }
    if (keyboard[SDL_SCANCODE_D]) {
        Brakeza3D::get()->getCamera3D()->StrafeRight();
    }
    if (keyboard[SDL_SCANCODE_RIGHT]) {
        Brakeza3D::get()->getCamera3D()->TurnRight();
    }
    if (keyboard[SDL_SCANCODE_LEFT]) {
        Brakeza3D::get()->getCamera3D()->TurnLeft();
    }
    if (keyboard[SDL_SCANCODE_DOWN]) {
        Brakeza3D::get()->getCamera3D()->PitchUp();
    }
    if (keyboard[SDL_SCANCODE_UP]) {
        Brakeza3D::get()->getCamera3D()->PitchDown();
    }*/
}

void InputController::handleWindowEvents(SDL_Event *e, bool &end)
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

void InputController::updateKeyboardMapping()
{
    this->keyboard = (unsigned char *) SDL_GetKeyboardState(NULL);
}

void InputController::updateMouseStates(SDL_Event *event)
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