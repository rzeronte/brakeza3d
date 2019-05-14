
#include <iostream>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <LinearMath/btDefaultMotionState.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
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

    this->keyboard = (unsigned char *) SDL_GetKeyboardState(NULL);

}

void Controller::handleKeyboard(Camera3D *camera, bool &done, btDiscreteDynamicsWorld* dynamicsWorld )
{
    if (keyboard[SDL_SCANCODE_ESCAPE]) {
        done = true;
    }

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

    if (keyboard[SDL_SCANCODE_SPACE]) {
        camera->Jump();
     }

    if (keyboard[SDL_SCANCODE_RETURN]) {
        camera->Fire();
        btTransform trans;
        trans.setIdentity();

        Vertex3D pos = *camera->getPosition();
        trans.setOrigin(btVector3(pos.x , pos.y, pos.z));

        btScalar mass(1.);
        btVector3 localInertia(0, 0, 0);
        btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
        btCollisionShape* shape = new btSphereShape(1.0f);
        btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
        btRigidBody *bRB = new btRigidBody(cInfo);
        Vertex3D dir = camera->velocity.getComponent();
        bRB->applyCentralImpulse( btVector3( dir.x, dir.y, 40 ) );
        dynamicsWorld->addRigidBody(bRB);
    }
}
