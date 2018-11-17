
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

    if (event_type == SDL_MOUSEBUTTONDOWN) {
        MousePressed = true;
    }

    if (event_type == SDL_MOUSEBUTTONUP) {
        MousePressed = false;
    }

    if (event_type == SDL_MOUSEMOTION) {
        MouseMotion = true;
        if (!Mousefirst) {
            MouseRelX = event->motion.xrel;
            MouseRelY = event->motion.yrel;
        } else {
            Mousefirst = false;
            MouseRelX = 0;
            MouseRelY = 0;
        }
    }

    float CameraYaw = 0;
    float CameraPitch = 0;

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
        if (key_pressed == SDLK_PAGEUP ) {
            camera->PitchUp();
        }
        if (key_pressed == SDLK_PAGEDOWN ) {
            camera->PitchDown();
        }

        switch( key_pressed ) {

            case SDLK_k:
                camera->horizontal_fov--;
                printf("Camera HFOV: %f\r\n", camera->horizontal_fov);
                break;

            case SDLK_l:
                camera->horizontal_fov++;
                printf("Camera HFOV: %f\r\n", camera->horizontal_fov);
                break;

            case SDLK_ESCAPE:
                done = true;
                break;
            case SDL_WINDOWEVENT_CLOSE:
                done = true;
                break;
            case SDL_QUIT:
                done = true;
                break;
        }
    }
    //camera->getPosition()->addVertex( Vertex3D(mov_x, mov_y, mov_z) );
    //camera->getRotation()->addRotation( Rotation3D(rot_x, rot_y, rot_z) );

}

void Controller::updateKeyboardRead(SDL_Event *event)
{
    this->key_pressed = event->key.keysym.sym;
    this->event_type = event->type;
}