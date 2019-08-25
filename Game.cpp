#include "Game.h"
#include "headers/Render/Drawable.h"
#include "headers/Objects/Sprite3D.h"
#include "headers/Render/Maths.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "src/Decal.h"

Game::Game() {}

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

    //this->loadDemoObjects();
}

void Game::mainLoop()
{
    ImGuiIO& io = ImGui::GetIO();
    while(!finish) {

        this->preUpdate();

        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (EngineSetup::getInstance()->CAMERA_MOUSE_ROTATION) {
                controller->handleMouse(&this->e, this->camera, collisionsManager->getDynamicsWorld(), Engine::gameObjects, Engine::getTimer(), menu, weaponManager);
            }
            controller->handleKeyboard(&this->e, this->camera, this->finish, collisionsManager->getDynamicsWorld(), Engine::gameObjects, Engine::getTimer(), menu, weaponManager);
        }

        // Check array Uint8 *keyboard
        controller->handleKeyboardContinuous(&this->e, this->camera, this->finish, collisionsManager->getDynamicsWorld(), Engine::gameObjects, Engine::getTimer(), menu, weaponManager);

        // Checks pre update frame
        this->postUpdate();

        // game level update
        this->onUpdate();

        // Update window
        Engine::windowUpdate();
    }
}

void Game::onUpdate()
{
    // Core onUpdate
    Engine::onUpdate();

    /*Vertex3D startPoint = Vertex3D(45, -2, 40);
    Vertex3D endPoint   = Vertex3D(70, -2, 40);
    Drawable::drawLightning(camera, startPoint, endPoint);*/

    //Mesh3D *triangle = (Mesh3D*) getObjectByLabel("triangle");
    //EngineSetup::getInstance()->TESTING_INT+=1;
}

void Game::preUpdate()
{
    // Core preUpdate
    Engine::preUpdate();
}

void Game::onEnd()
{
    Engine::onEnd();
}

void Game::loadDemoObjects()
{
    LightPoint3D *lp1 = new LightPoint3D();
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

    // mono
    Mesh3D *monkey = new Mesh3D();
    monkey->setEnabled(true);
    monkey->setDecal(false);
    monkey->setPosition(Vertex3D(1, 1, 28));
    //monkey->setLightPoints(Engine::lightPoints);
    monkey->loadOBJBlender("../assets/models/mono.obj");
    monkey->setShadowCaster(true);
    this->addObject3D(monkey, "monkey");

    // mono
    Mesh3D *hammer = new Mesh3D();
    hammer->setEnabled(false);
    hammer->setPosition(Vertex3D(5, 5, 30));
    hammer->setLightPoints(Engine::lightPoints);
    hammer->loadOBJBlender("../assets/models/hammer.obj");
    hammer->setShadowCaster(true);
    this->addObject3D(hammer, "hammer");

    // ball
    Mesh3D *wolf = new Mesh3D();
    wolf->setEnabled(false);
    wolf->setLightPoints(Engine::lightPoints);
    wolf->loadOBJBlender("../assets/models/Wolf.obj");
    wolf->setShadowCaster(true);
    this->addObject3D(wolf, "wolf");

    // cubo
    Mesh3D *cube = new Mesh3D();
    cube->setEnabled(false);
    cube->setLightPoints(Engine::lightPoints);
    cube->setPosition(*camera->getPosition());
    cube->loadOBJBlender("../assets/models/cubo.obj");
    this->addObject3D(cube, "cube");

    // decal
    Decal *decal = new Decal(Vertex3D(1, 1, 25));
    decal->texture->loadTGA( std::string(EngineSetup::getInstance()->IMAGES_FOLDER + "gore1.png").c_str(), 1 );
    decal->frustum->setup(
            *decal->getPosition(),
            Vertex3D(0, 0, 1),
            EngineSetup::getInstance()->up,
            EngineSetup::getInstance()->right,
            1,
            decal->h, decal->h,
            decal->w,
            decal->h, decal->h
    );
    this->addObject3D(decal, "decal");

    // triangle
    Mesh3D *triangle = new Mesh3D();
    triangle->setScale(0.1);
    triangle->setDecal(false);
    triangle->setEnabled(true);
    triangle->setLightPoints(Engine::lightPoints);
    triangle->setPosition(Vertex3D(5, 5, 30));
    triangle->setRotation( M3(90, 0, 3) );
    triangle->loadOBJBlender("../assets/models/triangle_2uv.obj");
    this->addObject3D(triangle, "triangle");

    // plane
    Mesh3D *plane = new Mesh3D();
    plane->setEnabled(false);
    plane->setLightPoints(Engine::lightPoints);
    plane->setPosition(Vertex3D(544, -32, 613));
    plane->setRotation( M3(-90, -45, 0) );
    plane->loadOBJBlender("../assets/models/plane.obj");
    this->addObject3D(plane, "plane");

    // marine (sprite directional)
    SpriteDirectional3D *marine = new SpriteDirectional3D();
    marine->setEnabled(false);
    marine->setPosition(Vertex3D(10, 0, -10));
    marine->setTimer(Engine::getTimer());
    marine->addAnimationDirectional2D("soldier/walk", 4, false, -1);
    marine->addAnimationDirectional2D("soldier/fire", 2, false, -1);
    marine->addAnimationDirectional2D("soldier/injuried", 1, false, -1);
    marine->addAnimationDirectional2D("soldier/dead", 5, true, 1);
    marine->addAnimationDirectional2D("soldier/explosion", 8, true, 1);
    marine->setAnimation(EngineSetup::getInstance()->SpriteDoom2SoldierAnimations::SOLDIER_WALK);
    this->addObject3D(marine, "marine");

    // skull (sprite directional)
    SpriteDirectional3D *skull = new SpriteDirectional3D();
    skull->setEnabled(false);
    skull->setPosition(Vertex3D(5, 0, -10));
    skull->setTimer(Engine::getTimer());
    skull->addAnimationDirectional2D("skull/idle", 5, false, -1);
    skull->setAnimation(EngineSetup::getInstance()->SpriteDoom2SoldierAnimations::SOLDIER_WALK);
    this->addObject3D(skull, "skull");

    // cacodemon (sprite directional)
    SpriteDirectional3D *cacodemon = new SpriteDirectional3D();
    cacodemon->setEnabled(false);
    cacodemon->setPosition(Vertex3D(20, 0, -10));
    cacodemon->setTimer(Engine::getTimer());
    cacodemon->addAnimationDirectional2D("cacodemon/walk", 6, false, -1);
    cacodemon->addAnimationDirectional2D("cacodemon/dead", 6, false, -1);
    cacodemon->setAnimation(EngineSetup::getInstance()->SpriteDoom2CacodemonAnimations::FLY);
    this->addObject3D(cacodemon, "cacodemon");

    // marine ( sprite )
    Sprite3D *doomFace = new Sprite3D();
    doomFace->setEnabled(false);
    doomFace->setPosition( Vertex3D(2, 1, 15) );
    doomFace->setTimer(Engine::getTimer());
    doomFace->addAnimation("doom_face/face", 3, 10);
    doomFace->setAnimation(EngineSetup::getInstance()->SpriteGuyAnimations::NORMAL);
    doomFace->getBillboard()->setDimensions(1, 1);
    this->addObject3D(doomFace, "doomFace");

    // cubo physics
    Mesh3DBody *cuboPhysic = new Mesh3DBody();
    cuboPhysic->setEnabled(false);
    cuboPhysic->setLightPoints(Engine::lightPoints);
    cuboPhysic->setPosition(Vertex3D(54, -16, 87));
    cuboPhysic->loadOBJBlender("../assets/models/cubo.obj");
    cuboPhysic->makeRigidBody(1.0f, Engine::gameObjects, Engine::camera, collisionsManager->getDynamicsWorld(), false);
    this->addObject3D(cuboPhysic, "cuboPhysic");

    /*Mesh3DGhost *cuboPhysicGhost = new Mesh3DGhost();
    cuboPhysicGhost->setEnabled(true);
    cuboPhysicGhost->setLightPoints(Engine::lightPoints, Engine::numberLightPoints);
    cuboPhysicGhost->setPosition(Vertex3D(52, -0.2, 87));
    cuboPhysicGhost->loadOBJBlender("../assets/models/cubo.obj");
    cuboPhysicGhost->makeGhostBody(Engine::camera, this->dynamicsWorld, true);
    this->addObject3D(cuboPhysicGhost, "cuboPhysicGhost");*/
}