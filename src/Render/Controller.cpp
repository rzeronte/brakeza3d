
#include <iostream>
#include "../../headers/Render/Controller.h"
#include "../../headers/Objects/Mesh3D.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Objects/Object3D.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Maths.h"

Controller::Controller()
{
}

void Controller::handleMouse(SDL_Event *event, Camera3D *camera)
{

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        MousePressed = true;
    }

    if (event->type == SDL_MOUSEBUTTONUP) {
        MousePressed = false;
    }

    if (event->type == SDL_MOUSEMOTION) {
        MouseMotion = true;
    }

    if (MouseMotion && MousePressed) {
        MouseMotion = false;
        if (event->type == SDL_MOUSEMOTION) {
            camera->Yaw(event->motion.xrel);
            camera->Pitch(event->motion.yrel);
        }
    }
}

void Controller::handleKeyboard(Camera3D *camera, bool &done)
{
    this->keyboard = (unsigned char *) SDL_GetKeyboardState(NULL);

    if (keyboard[SDL_SCANCODE_W]) {
        camera->MoveForward();
    }
    if (keyboard[SDL_SCANCODE_S]) {
        camera->MoveBackward();
    }
    if (keyboard[SDL_SCANCODE_A]) {
        camera->StrafeLeft();
    }
    if (keyboard[SDL_SCANCODE_D]) {
        camera->StrafeRight();
    }
    if (keyboard[SDL_SCANCODE_RIGHT]) {
        camera->TurnRight();
    }
    if (keyboard[SDL_SCANCODE_LEFT]) {
        camera->TurnLeft();
    }
    if (keyboard[SDL_SCANCODE_DOWN]) {
        camera->PitchUp();
    }
    if (keyboard[SDL_SCANCODE_UP]) {
        camera->PitchDown();
    }
    if (keyboard[SDL_SCANCODE_ESCAPE]) {
        done = true;
    }
}
