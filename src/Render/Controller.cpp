
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
    this->key_pressed = event->key.keysym.sym;
    this->event_type = event->type;

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    if (event_type == SDL_MOUSEBUTTONDOWN) {
        MousePressed = true;
    }

    if (event_type == SDL_MOUSEBUTTONUP) {
        MousePressed = false;
    }

    if (event_type == SDL_MOUSEMOTION) {
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

void Controller::handleKeyboard(SDL_Event *event, Camera3D *camera, bool &done)
{
    this->key_pressed = event->key.keysym.sym;
    this->event_type = event->type;

    if  (event_type == SDL_KEYDOWN) {
        if (key_pressed == SDLK_w ) {
            camera->MoveForward();
        }
        if (key_pressed == SDLK_UP ) {
            camera->PitchUp();
        }
        if (key_pressed == SDLK_s ) {
            camera->MoveBackward();
        }
        if (key_pressed == SDLK_DOWN ) {
            camera->PitchDown();
        }
        if (key_pressed == SDLK_RIGHT ) {
            camera->TurnRight();
        }
        if (key_pressed == SDLK_LEFT ) {
            camera->TurnLeft();
        }
        if (key_pressed == SDLK_d ) {
            camera->StrafeRight();
        }
        if (key_pressed == SDLK_a ) {
            camera->StrafeLeft();
        }
        if (key_pressed == SDLK_ESCAPE) {
            done = true;
        }
    }
}
