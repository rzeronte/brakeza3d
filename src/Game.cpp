#include "../headers/Game.h"
#include "../headers/Render/Maths.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../headers/Physics/Sprite3DBody.h"
#include "../headers/Brakeza3D.h"
#include "../headers/Render/EngineBuffers.h"

Game* Game::instance = 0;

Game::Game()
{
}

Game* Game::get()
{
    if (instance == 0) {
        instance = new Game();
    }

    return instance;
}

void Game::start()
{
    // Start Engine cycle
    onStart();
    mainLoop();
    onEnd();
}

void Game::onStart()
{
    Engine::onStart();
}

void Game::mainLoop()
{
    ImGuiIO& io = ImGui::GetIO();
    while(!finish) {

        this->preUpdate();

        this->onUpdateInputController();

        if (!finish) {
            // game level update
            this->onUpdate();

            // Update window
            Engine::updateWindow();
        }
    }
}

void Game::onUpdate()
{
    // Core onUpdate
    Engine::onUpdate();

    onUpdateIA();
}

void Game::preUpdate()
{
    // Core preUpdate
    Engine::preUpdate();
}

void Game::onEnd()
{
    Engine::onEnd();
    Close();
}

void Game::onUpdateInputController()
{
    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if (EngineSetup::getInstance()->CAMERA_MOUSE_ROTATION) {
            Brakeza3D::get()->getController()->handleMouse(&this->e);
        }
        Brakeza3D::get()->getController()->handleKeyboard(&this->e, this->finish);
    }

    // Check array Uint8 *keyboard
    Brakeza3D::get()->getController()->handleKeyboardContinuous(&this->e, this->finish);
}

void Game::onUpdateIA()
{
    std::vector<Object3D *>::iterator itObject3D;
    for ( itObject3D = Brakeza3D::get()->getSceneObjects().begin(); itObject3D !=
                                                                    Brakeza3D::get()->getSceneObjects().end(); itObject3D++) {
        Object3D *object = *(itObject3D);

        if (!Brakeza3D::get()->getCamera()->frustum->isPointInFrustum(*object->getPosition())) {
            continue;
        }

        NPCEnemyBody *enemy = dynamic_cast<NPCEnemyBody*> (object);
        if (enemy != NULL) {
            if (enemy->isDead()) continue;

            Vertex3D A = *object->getPosition();
            Vertex3D B = *Brakeza3D::get()->getCamera()->getPosition();

            enemy->points.clear();
            Brakeza3D::get()->getBSP()->recastWrapper->getPathBetween(A, B, enemy->points );
            enemy->evalStatusMachine(
                    Brakeza3D::get()->getBSP()->recastWrapper->rayCasting(A, B),
                    Vector3D(A, B).getComponent().getModule(),
                    Brakeza3D::get()->getCamera(),
                    Brakeza3D::get()->getCollisionManager()->getDynamicsWorld(),
                    Brakeza3D::get()->getSceneObjects()
            );
        }
    }
}