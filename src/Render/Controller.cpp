
#include <iostream>
#include "../../headers/Controller.h"
#include "../../headers/Mesh.h"
#include "../../headers/EngineSetup.h"
#include "../../headers/Object3D.h"

Controller::Controller()
{
}

void Controller::handleMouse(SDL_Event *event, Camera *camera)
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
        CameraYaw += MouseRelX * EngineSetup::getInstance()->MOUSE_SENSIBILITY;
        CameraPitch += MouseRelY * EngineSetup::getInstance()->MOUSE_SENSIBILITY;

        camera->rotation.x+= CameraPitch;
        camera->rotation.y+= CameraYaw;
    }
}

void Controller::handleKeyboard(SDL_Event *event, Camera *camera, bool &done)
{
    if  (event_type == SDL_KEYDOWN) {
        switch( key_pressed ) {
            // Camera position
            // Y
            case SDLK_q:
                camera->getPosition()->y += EngineSetup::getInstance()->CONTROLLER_SPEED_MOVEMENT_CAMERA;
                break;
            case SDLK_a:
                camera->getPosition()->y -= EngineSetup::getInstance()->CONTROLLER_SPEED_MOVEMENT_CAMERA;
                break;
                // X
            case SDLK_o:
                camera->getPosition()->x -= EngineSetup::getInstance()->CONTROLLER_SPEED_MOVEMENT_CAMERA;
                break;
            case SDLK_p:
                camera->getPosition()->x += EngineSetup::getInstance()->CONTROLLER_SPEED_MOVEMENT_CAMERA;
                break;
                // Z
            case SDLK_w:
                camera->getPosition()->z += EngineSetup::getInstance()->CONTROLLER_SPEED_MOVEMENT_CAMERA;
                break;
            case SDLK_s:
                camera->getPosition()->z -= EngineSetup::getInstance()->CONTROLLER_SPEED_MOVEMENT_CAMERA;
                break;
                // camera rotation
            case SDLK_UP:
                camera->getRotation()->x += EngineSetup::getInstance()->CONTROLLER_SPEED_ROTATION_CAMERA;
                break;
            case SDLK_DOWN:
                camera->getRotation()->x -= EngineSetup::getInstance()->CONTROLLER_SPEED_ROTATION_CAMERA;
                break;
            case SDLK_LEFT:
                camera->getRotation()->y -= EngineSetup::getInstance()->CONTROLLER_SPEED_ROTATION_CAMERA;
                break;
            case SDLK_RIGHT:
                camera->getRotation()->y += EngineSetup::getInstance()->CONTROLLER_SPEED_ROTATION_CAMERA;
                break;
            case SDLK_f:
                camera->getRotation()->z -= EngineSetup::getInstance()->CONTROLLER_SPEED_ROTATION_CAMERA;
                break;
            case SDLK_g:
                camera->getRotation()->z += EngineSetup::getInstance()->CONTROLLER_SPEED_ROTATION_CAMERA;
                break;
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

}

void Controller::handleKeyboardObject3D(SDL_Event *event, Object3D *object)
{
    if (key_pressed == SDLK_z) {
        object->getRotation()->x -= EngineSetup::getInstance()->CONTROLLER_SPEED_ROTATION;
    }

    if (key_pressed == SDLK_x) {
        object->getRotation()->x += EngineSetup::getInstance()->CONTROLLER_SPEED_ROTATION;
    }

    if (key_pressed == SDLK_c) {
        object->getRotation()->y -= EngineSetup::getInstance()->CONTROLLER_SPEED_ROTATION;
    }
    if (key_pressed == SDLK_v) {
        object->getRotation()->y += EngineSetup::getInstance()->CONTROLLER_SPEED_ROTATION;
    }
    if (key_pressed == SDLK_b) {
        object->getRotation()->z -= EngineSetup::getInstance()->CONTROLLER_SPEED_ROTATION;
    }
    if (key_pressed == SDLK_n) {
        object->getRotation()->z += EngineSetup::getInstance()->CONTROLLER_SPEED_ROTATION;
    }

    // position
    if (key_pressed == SDLK_1) {
        object->getPosition()->y += EngineSetup::getInstance()->CONTROLLER_SPEED_MOVEMENT;
    }
    if (key_pressed == SDLK_2) {
        object->getPosition()->y -= EngineSetup::getInstance()->CONTROLLER_SPEED_MOVEMENT;
    }
    if (key_pressed == SDLK_3) {
        object->getPosition()->x -= EngineSetup::getInstance()->CONTROLLER_SPEED_MOVEMENT;
    }
    if (key_pressed == SDLK_4) {
        object->getPosition()->x += EngineSetup::getInstance()->CONTROLLER_SPEED_MOVEMENT;
    }
    if (key_pressed == SDLK_5) {
        object->getPosition()->z -= EngineSetup::getInstance()->CONTROLLER_SPEED_MOVEMENT;
    }
    if (key_pressed == SDLK_6) {
        object->getPosition()->z+=EngineSetup::getInstance()->CONTROLLER_SPEED_MOVEMENT;
    }
}

void Controller::updateKeyboardRead(SDL_Event *event)
{
    this->key_pressed = event->key.keysym.sym;
    this->event_type = event->type;
}