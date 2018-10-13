#include "Game.h"
#include "headers/Drawable.h"
#include "headers/EngineBuffers.h"
#include "headers/M3.h"
#include "headers/Render.h"
#include "headers/Core/Logging.h"
#include "headers/BillboardDirectional.h"
#include "headers/Sprite3D.h"


enum SpriteDoom2SoldierAnimations {
    IDLE = 0,
    WALK = 1,
    JUMP = 2,
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

    LightPoint *lp1 = new LightPoint();
    lp1->setEnabled(false);
    lp1->setLabel("LightPoint1");
    lp1->setPosition(Vertex(1, 1.5f, -1));
    lp1->setColor( 255, 0, 0 );
    this->addLightPoint(lp1, "l1");

    LightPoint *lp2 = new LightPoint();
    lp2->setEnabled(false);
    lp2->setLabel("LightPoint2");
    lp2->setPosition(Vertex(-0.4, 1, -1));
    lp2->setColor( 0, 255, 0 );
    this->addLightPoint(lp2, "l2");

    LightPoint *lp3 = new LightPoint();
    lp3->setEnabled(false);
    lp3->setLabel("LightPoint3");
    lp3->setPosition(Vertex(2, 1, -1));
    lp3->setColor( 0, 0, 255 );
    this->addLightPoint(lp3, "l3");

    // mono
    Mesh *mono = new Mesh();
    mono->setEnabled(false);
    mono->setLightPoints(Engine::lightPoints, Engine::numberLightPoints);
    mono->rotation.x = 180;
    mono->setPosition( Vertex(1, 1, -1) );
    mono->loadOBJBlender("../models/mono.obj");
    mono->setShadowCaster(true);
    this->addObject3D(mono, "mono");

    // cubo
    Mesh *cubo = new Mesh();
    cubo->scale = 500;
    cubo->setEnabled(false);
    cubo->setLightPoints(Engine::lightPoints, Engine::numberLightPoints);
    cubo->rotation.x = 180;
    cubo->setPosition( Vertex(1, 1, 20) );
    cubo->loadOBJBlender("../models/cubo.obj");
    this->addObject3D(cubo, "cubo");

    // q3 map
    Mesh *q3map = new Mesh();
    q3map->setEnabled(false);
    q3map->setLightPoints(Engine::lightPoints, Engine::numberLightPoints);
    q3map->setPosition( Vertex(1, 1, 5) );
    q3map->loadQ3Map("../pak0/maps/q3dm17.bsp");
    this->addObject3D(q3map, "q3map");

    // triangle
    Mesh *triangle = new Mesh();
    triangle->setEnabled(false);
    triangle->rotation.x-=90;
    triangle->setLightPoints(Engine::lightPoints, Engine::numberLightPoints);
    triangle->setPosition( Vertex(1, 1, 5) );
    triangle->loadOBJBlender("../models/triangle_2uv.obj");
    this->addObject3D(triangle, "triangle");

    // sprite
    Sprite3D *sprite = new Sprite3D();
    sprite->setEnabled(true);
    sprite->setPosition( Vertex(1, 1, 5) );
    sprite->setTimer(Engine::getTimer());

    sprite->getBillboard()->addAnimationDirectional2D("marine/idle", 1);
    sprite->getBillboard()->addAnimationDirectional2D("marine/walk", 4);
    sprite->getBillboard()->addAnimationDirectional2D("marine/jump", 1);
    sprite->getBillboard()->setAnimation(SpriteDoom2SoldierAnimations::WALK);

    this->addObject3D(sprite, "marine");
}

void Game::mainLoop()
{
    fpsTimer.start();

    ImGuiIO& io = ImGui::GetIO();

    while(!finish) {
        while (SDL_PollEvent(&e)) {
            // GUI Events
            ImGui_ImplSDL2_ProcessEvent(&e);

            // Keyboard Reading
            Engine::getController()->updateKeyboardRead(&e);

            // Camera Update (Mouse & Keyboard)
            Engine::cameraUpdate();

            this->onUpdateEvent();
        }

        this->onUpdate();

        Engine::windowUpdate();
        Engine::processFPS();
    }

    fpsTimer.stop();
}

void Game::onUpdateEvent()
{
    Engine::onUpdateEvent();
}

void Game::onUpdate()
{
    Engine::onUpdate();

    //Mesh *q3map= (Mesh*) getObjectByLabel("q3map");

}

void Game::onEnd()
{
    Engine::onEnd();
}
