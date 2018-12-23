#include "Game.h"
#include "headers/Render/Drawable.h"
#include "headers/Render/EngineBuffers.h"
#include "headers/Render/M3.h"
#include "headers/Render/Transforms.h"
#include "headers/Render/Logging.h"
#include "headers/Objects/SpriteDirectional3D.h"
#include "headers/Objects/Sprite3D.h"
#include "headers/Objects/Weapon3D.h"
#include "headers/Render/Maths.h"
#include "WAD/WAD.h"
#include "headers/Render/BSPMap.h"

enum SpriteDoom2SoldierAnimations {
    IDLE = 0,
    WALK = 1,
    JUMP = 2,
};

enum SpriteShotgunAnimations {
    READY,
    RELOAD,
    SHOT
};

enum SpriteGuyAnimations {
    NORMAL,
};

void Game::run()
{
    this->onStart();
    this->mainLoop();
    this->onEnd();

    Close();
}


void Game::onStart()
{
    Engine::onStart();

    Engine::camera->setPosition(Vertex3D(544, -32, 233));

    /*LightPoint3D *lp1 = new LightPoint3D();
    lp1->setEnabled(false);
    lp1->setLabel("LightPoint1");
    lp1->setPosition(Vertex3D(1, 1.5f, -1));
    lp1->setColor( 255, 0, 0 );
    this->addLightPoint(lp1, "l1");

    LightPoint3D *lp2 = new LightPoint3D();
    lp2->setEnabled(false);
    lp2->setLabel("LightPoint2");
    lp2->setPosition(Vertex3D(-0.4, 1, -1));
    lp2->setColor( 0, 255, 0 );
    this->addLightPoint(lp2, "l2");

    LightPoint3D *lp3 = new LightPoint3D();
    lp3->setEnabled(false);
    lp3->setLabel("LightPoint3");
    lp3->setPosition(Vertex3D(2, 1, -1));
    lp3->setColor( 0, 0, 255 );
    this->addLightPoint(lp3, "l3");
    */

    // mono
    Mesh3D *mono = new Mesh3D();
    mono->setEnabled(false);
    mono->setLightPoints(Engine::lightPoints, Engine::numberLightPoints);
    mono->loadOBJBlender("../assets/models/mono.obj");
    mono->setShadowCaster(true);
    this->addObject3D(mono, "mono");

    // ball
    Mesh3D *ball = new Mesh3D();
    ball->setEnabled(false);
    ball->setLightPoints(Engine::lightPoints, Engine::numberLightPoints);
    ball->loadOBJBlender("../assets/models/Wolf.obj");
    ball->setShadowCaster(true);
    this->addObject3D(ball, "box");

    // cubo
    Mesh3D *cubo = new Mesh3D();
    cubo->setEnabled(false);
    cubo->setLightPoints(Engine::lightPoints, Engine::numberLightPoints);
    cubo->setPosition( Vertex3D(1, 1, 20) );
    cubo->loadOBJBlender("../assets/models/cubo.obj");
    this->addObject3D(cubo, "cubo");

    // triangle
    Mesh3D *triangle = new Mesh3D();
    triangle->setEnabled(true);
    triangle->setLightPoints(Engine::lightPoints, Engine::numberLightPoints);
    triangle->setPosition( Vertex3D(1, 0.4, 5) );
    triangle->setRotation( M3(-90, -45, 0) );
    triangle->loadOBJBlender("../assets/models/triangle_2uv.obj");
    this->addObject3D(triangle, "triangle");

    // plane
    Mesh3D *plane = new Mesh3D();
    plane->setEnabled(false);
    plane->setLightPoints(Engine::lightPoints, Engine::numberLightPoints);
    plane->setPosition( Vertex3D(2, 0.4, 5) );
    plane->setRotation( M3(-90, -45, 0) );
    plane->loadOBJBlender("../assets/models/plane.obj");
    this->addObject3D(plane, "plane");


    // marine (sprite directional)
    SpriteDirectional3D *marine = new SpriteDirectional3D();
    marine->setEnabled(false);
    marine->setPosition( Vertex3D(1, 1, 5) );
    marine->setTimer(Engine::getTimer());
    marine->addAnimationDirectional2D("marine/idle", 1);
    marine->addAnimationDirectional2D("marine/walk", 4);
    marine->addAnimationDirectional2D("marine/jump", 1);
    marine->setAnimation(SpriteDoom2SoldierAnimations::WALK);
    this->addObject3D(marine, "marine");

    // marine ( sprite )
    /*Sprite3D *guy = new Sprite3D();
    guy->setEnabled(true);
    guy->setPosition( Vertex3D(2, 1, 5) );
    guy->setTimer(Engine::getTimer());
    guy->addAnimation("guy/face", 3);
    guy->setAnimation(SpriteGuyAnimations::NORMAL);
    this->addObject3D(guy, "guy");
     */

    // weapon
    Weapon3D *weapon = new Weapon3D();
    weapon->setEnabled(false);
    weapon->setPosition( Vertex3D(1, 1, 5) );
    weapon->setTimer(Engine::getTimer());
    weapon->addAnimation("gun/ready", 1);
    weapon->addAnimation("gun/reload", 3);
    weapon->addAnimation("gun/shot", 2);
    weapon->setAnimation(SpriteShotgunAnimations::RELOAD);
    this->addObject3D(weapon, "weapon");


    // WAD Parser
    /*
    char* wadLocation = "../models/freedoom1.wad";
    WAD* testWad = new WAD(wadLocation);
    testWad->loadMap("E1M1");
    testWad->draw2D();
    testWad->render();
    */


    loadBSP("start.bsp", "palette.lmp");
}

void Game::mainLoop()
{
    fpsTimer.start();

    ImGuiIO& io = ImGui::GetIO();
    while(!finish) {
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);

            if (EngineSetup::getInstance()->CAMERA_MOUSE_ROTATION) { controller->handleMouse(&this->e, this->camera); }
        }

        // Check aray Uint8 *keyboard
        controller->handleKeyboard(this->camera, this->finish);

        // update camera position and rotation
        camera->UpdatePosition();
        camera->UpdateRotation();

        // align frustum to camera
        camera->syncFrustum();

        // game level update
        this->onUpdate();

        // Update window
        Engine::windowUpdate();

        // FPS calculation
        Engine::processFPS();
    }

    fpsTimer.stop();
}

void Game::onUpdate()
{
    // Core onUpdate
    Engine::onUpdate();

    //Mesh3D *marine= (Mesh3D*) getObjectByLabel("marine");
    //marine->rotation.y+=0.5f;

}

void Game::onEnd()
{
    Engine::onEnd();
}
