#include "../../headers/Render/Engine.h"
#include "../../headers/Objects/Mesh3D.h"
#include "../../headers/Render/EngineBuffers.h"
#include "../../headers/Render/Frustum.h"
#include "../../headers/Objects/Object3D.h"
#include "../../headers/Render/Timer.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Objects/LightPoint3D.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Objects/SpriteDirectional3D.h"
#include "../../headers/Objects/Sprite3D.h"
#include "../../headers/Objects/Weapon3D.h"
#include "../../headers/Objects/BSPEntity3D.h"
#include "../../headers/Render/Maths.h"
#include <chrono>
#include <iostream>

#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "LinearMath/btTransform.h"

Engine::Engine()
{
    // GUI engine
    gui_engine = new GUI_Engine();

    // Link GUI_log with Logging singleton
    Logging::getInstance()->setGUILog(gui_engine->gui_log);

    //The window we'll be rendering to
    window = NULL;

    // used to finish all
    finish = false;

    // gameobjects
    this->gameObjects = new Object3D *[EngineSetup::getInstance()->ENGINE_MAX_GAMEOBJECTS];
    this->numberGameObjects = 0;

    // lights
    this->lightPoints = new LightPoint3D *[EngineSetup::getInstance()->ENGINE_MAX_GAMEOBJECTS];
    this->numberLightPoints = 0;

    // cam
    camera = new Camera3D();

    // input controller
    controller = new Controller();

    IMGUI_CHECKVERSION();
    imgui_context = ImGui::CreateContext();
    fps = 0;

    this->initPhysics();


}

bool Engine::initWindow()
{
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    } else {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

        //Create window
        window = SDL_CreateWindow(
            EngineSetup::getInstance()->ENGINE_TITLE.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            EngineSetup::getInstance()->SCREEN_WIDTH,
            EngineSetup::getInstance()->SCREEN_HEIGHT,
            SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE
        );
        // | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE

        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return false;
        } else {
            gl_context = SDL_GL_CreateContext(window);
            EngineBuffers::getInstance()->setScreenSurface(SDL_CreateRGBSurface(0, EngineSetup::getInstance()->SCREEN_WIDTH, EngineSetup::getInstance()->SCREEN_HEIGHT, 32, 0, 0, 0, 0));
            renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

            SDL_GL_SetSwapInterval(1); // Enable vsync

            ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
            ImGui_ImplOpenGL2_Init();

            ImGuiIO& io = ImGui::GetIO();
            io.WantCaptureMouse = false;
            io.WantCaptureKeyboard = false;

            // Setup style
            ImGui::StyleColorsDark();
            ImGuiStyle& style = ImGui::GetStyle();
            style.FrameBorderSize = 1.0f;

            ImGui_ImplOpenGL2_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);

            drawGUI();

            Logging::getInstance()->Log(
                "Window size: " + std::to_string(EngineSetup::getInstance()->SCREEN_WIDTH) + " x " + std::to_string(EngineSetup::getInstance()->SCREEN_HEIGHT), "INFO"
            );

            SDL_GL_SwapWindow(window);

            // Init TTF
            initFontsTTF();
        }
    }

    return true;
}

void Engine::initFontsTTF()
{
    Logging::getInstance()->Log("Initializating TTF...", "INFO");

    // global font
    if (TTF_Init() < 0) {
        Logging::getInstance()->Log(TTF_GetError(), "INFO");
    } else {
        Logging::getInstance()->Log("Loading ../fonts/arial.ttf.", "INFO");

        font = TTF_OpenFont( "../fonts/arial.ttf", EngineSetup::getInstance()->TEXT_3D_SIZE );
        if(!font) {
            Logging::getInstance()->Log(TTF_GetError(), "INFO");
        }
    }
}

void Engine::initPhysics()
{
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    this->collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    this->dispatcher = new btCollisionDispatcher(collisionConfiguration);

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    this->overlappingPairCache = new btDbvtBroadphase();

    btVector3 worldMin(-1000,-1000,-1000);
    btVector3 worldMax(1000,1000,1000);
    btAxisSweep3* sweepBP = new btAxisSweep3(worldMin,worldMax);
    this->overlappingPairCache = sweepBP;

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    this->solver = new btSequentialImpulseConstraintSolver;

    /// Debug drawer
    this->debugDraw = new PhysicsDebugDraw(this->camera);

    this->dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    this->dynamicsWorld->setGravity(btVector3(0, EngineSetup::getInstance()->gravity.y, 0));

    this->overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

    this->camera->charCon->setGravity( dynamicsWorld->getGravity() );

    this->dynamicsWorld->addCollisionObject(this->camera->m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter);
    this->dynamicsWorld->addAction(this->camera->charCon);

    this->dynamicsWorld->setDebugDrawer(debugDraw);
    this->dynamicsWorld->getDebugDrawer()->setDebugMode(PhysicsDebugDraw::DBG_DrawWireframe);

    this->triggerCamera = new Mesh3DPhysic();
    triggerCamera->setLabel("triggerCamera");
    triggerCamera->setEnabled(true);
    triggerCamera->setLightPoints(Engine::lightPoints, Engine::numberLightPoints);
    triggerCamera->setPosition(*camera->getPosition());

    // triggerCamera
    btTransform trans;
    trans.setIdentity();
    Vertex3D pos = *this->camera->getPosition();
    trans.setOrigin(btVector3(52, 2, 65));
    triggerCamera->getGhostObject()->setCollisionShape(camera->charCon->getGhostObject()->getCollisionShape());
    triggerCamera->getGhostObject()->setWorldTransform(trans);

    triggerCamera->getGhostObject()->setUserPointer(triggerCamera);
    dynamicsWorld->addCollisionObject(triggerCamera->getGhostObject(), btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::SensorTrigger);


    //dynamicsWorld->setInternalTickCallback(myTickCallback);
}

void Engine::drawGUI()
{
    ImGui::NewFrame();

    //bool open = true;
    //ImGui::ShowDemoWindow(&open);

    gui_engine->setFps(fps);

    gui_engine->draw(
        finish,
        gameObjects, numberGameObjects,
        lightPoints, numberLightPoints,
        camera
    );

    ImGui::Render();
}

void Engine::windowUpdate()
{
    EngineBuffers::getInstance()->flipVideoBuffer( EngineBuffers::getInstance()->getScreenSurface() );

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);

    drawGUI();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);

    SDL_Texture *engine = SDL_CreateTextureFromSurface(renderer, EngineBuffers::getInstance()->getScreenSurface());
    SDL_RenderCopy(renderer, engine, NULL, NULL);

    SDL_DestroyTexture(engine);
    //SDL_RenderPresent( renderer );
    //SDL_RenderClear(renderer);
    //SDL_UpdateWindowSurface( window );
    //SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
}

void Engine::onStart()
{
    EngineBuffers::getInstance()->engineTimer.start();

    Engine::camera->setPosition( EngineSetup::getInstance()->CameraPosition );

    Engine::camera->velocity.vertex1 = *Engine::camera->getPosition();
    Engine::camera->velocity.vertex2 = *Engine::camera->getPosition();
}

void Engine::preUpdate()
{
    // Posición inicial del vector velocidad que llevará la cámara
    camera->velocity.vertex1 = *camera->getPosition();
    camera->velocity.vertex2 = *camera->getPosition();

    // Determinamos VPS
    if (bsp_map) {
        bspleaf_t *leaf = bsp_map->FindLeaf( camera );
        bsp_map->setVisibleSet(leaf);
    }
}

void Engine::postUpdate() {
    // update collider forces
    camera->UpdateVelocity();

    // update deltaTime
    EngineBuffers::getInstance()->updateTimer();

    // set visibles surfaces
    if (this->bsp_map) {
        this->bsp_map->PhysicsLeafVisibleSet(camera, this->dynamicsWorld);
    }

    Vertex3D vel = Engine::camera->velocity.getComponent();

    btVector3 bulletVelocity = btVector3(vel.x, vel.y, vel.z);

    if (this->camera->charCon->onGround()) {
        bulletVelocity = btVector3(vel.x, vel.y, vel.z);
    } else {
        bulletVelocity = btVector3(vel.x, vel.y, vel.z) / EngineSetup::getInstance()->AIR_RESISTANCE;
    }


    this->camera->charCon->setWalkDirection(bulletVelocity);
    Vertex3D finalVelocity;
    if (EngineSetup::getInstance()->BULLET_STEP_SIMULATION) {
        // Bullet Step Simulation
        dynamicsWorld->stepSimulation(EngineBuffers::getInstance()->deltaTime, 5);

        // Physics for meshes
        this->updatePhysicObjects();
        btTransform trans = this->camera->charCon->getGhostObject()->getWorldTransform();

        btVector3 pos = trans.getOrigin();
        float BSP_YOffset = 1;
        finalVelocity = Vertex3D(pos.getX(), pos.getY() - BSP_YOffset, pos.getZ());
    } else {
        finalVelocity = this->camera->velocity.vertex2;
    }

    this->camera->setPosition(finalVelocity);
    this->cameraUpdate();
    this->updateWeaponPosition();

    this->test(this->dynamicsWorld);


    int numManifolds = this->dynamicsWorld->getDispatcher()->getNumManifolds();
    Logging::getInstance()->getInstance()->Log("getNumManifolds: " + std::to_string(numManifolds));
    for (int i = 0; i < numManifolds; i++) {
        btPersistentManifold *contactManifold = this->dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject *obA = contactManifold->getBody0();
        const btCollisionObject *obB = contactManifold->getBody1();

        Object3D *brkObjectA = (Object3D *) obA->getUserPointer();
        Object3D *brkObjectB = (Object3D *) obB->getUserPointer();
        if (contactManifold->getNumContacts() > 0) {
            const btCollisionObject *obA = contactManifold->getBody0();
            const btCollisionObject *obB = contactManifold->getBody1();

            Object3D *brkObjectA = (Object3D *) obA->getUserPointer();
            Object3D *brkObjectB = (Object3D *) obB->getUserPointer();

            Logging::getInstance()->getInstance()->Log("contacto entre " + std::to_string(i) + " - " + brkObjectA->getLabel() + " y " + brkObjectB->getLabel());

        }
    }

    /*if (Mesh3DPhysic::CheckGhost( camera->charCon->getGhostObject()) ) {
        Logging::getInstance()->getInstance()->Log("paez que si");
    }*/
    /*btGhostObject *m_ghostObject = this->camera->charCon->getGhostObject();

    for (int i = 0; i < m_ghostObject->getNumOverlappingObjects(); i++) {
        btCollisionObject *obj = m_ghostObject->getOverlappingObject(i);
        Object3D *brkObjectA = (Object3D *) obj->getUserPointer();
        Logging::getInstance()->getInstance()->Log("contacto n: " + std::to_string(i) + "player y " + brkObjectA->getLabel());

    }*/

    /*btPairCachingGhostObject *m_ghostObject = this->camera->charCon->getGhostObject();

    btVector3 minAabb, maxAabb;
    btCollisionShape* m_convexShape = camera->charCon->getGhostObject()->getCollisionShape();
    m_convexShape->getAabb(m_ghostObject->getWorldTransform(), minAabb, maxAabb);
    dynamicsWorld->getBroadphase()->setAabb(m_ghostObject->getBroadphaseHandle(),
                                            minAabb,
                                            maxAabb,
                                            dynamicsWorld->getDispatcher());

    dynamicsWorld->getDispatcher()->dispatchAllCollisionPairs(m_ghostObject->getOverlappingPairCache(), dynamicsWorld->getDispatchInfo(), dynamicsWorld->getDispatcher());

    int numOverlappingObjects = this->camera->charCon->getGhostObject()->getNumOverlappingObjects();
    Logging::getInstance()->getInstance()->Log("getNumManifolds: " + std::to_string(numOverlappingObjects));

    for (int i = 0; i < this->camera->charCon->getGhostObject()->getNumOverlappingObjects(); i++) {
        btBroadphasePair *collisionPair = &this->camera->charCon->getGhostObject()->getOverlappingPairCache()->getOverlappingPairArray()[i];
        btCollisionObject *co = this->camera->charCon->getGhostObject()->getOverlappingObject(i);
        Object3D *brkObjectA = (Object3D *) co->getUserPointer();
        Logging::getInstance()->getInstance()->Log("contacto player y " + brkObjectA->getLabel());


    }*/

    /*btPairCachingGhostObject *m_ghostObject = this->camera->charCon->getGhostObject();

    btVector3 minAabb, maxAabb;
    btCollisionShape* m_convexShape = camera->charCon->getGhostObject()->getCollisionShape();
    m_convexShape->getAabb(m_ghostObject->getWorldTransform(), minAabb, maxAabb);
    dynamicsWorld->getBroadphase()->setAabb(m_ghostObject->getBroadphaseHandle(),
                                            minAabb,
                                            maxAabb,
                                            dynamicsWorld->getDispatcher());

    dynamicsWorld->getDispatcher()->dispatchAllCollisionPairs(m_ghostObject->getOverlappingPairCache(), dynamicsWorld->getDispatchInfo(), dynamicsWorld->getDispatcher());

    btManifoldArray m_manifoldArray;
    for (int i = 0; i < m_ghostObject->getOverlappingPairCache()->getNumOverlappingPairs(); i++) {
        m_manifoldArray.resize(0);
        btBroadphasePair* collisionPair = &m_ghostObject->getOverlappingPairCache()->getOverlappingPairArray()[i];

        if (collisionPair->m_algorithm)
            collisionPair->m_algorithm->getAllContactManifolds(m_manifoldArray);

        for (int j = 0; j < m_manifoldArray.size(); j++)
        {
            btPersistentManifold* contactManifold = m_manifoldArray[j];
            if (contactManifold->getNumContacts() > 0) {
                const btCollisionObject *obA = contactManifold->getBody0();
                const btCollisionObject *obB = contactManifold->getBody1();

                Object3D *brkObjectA = (Object3D *) obA->getUserPointer();
                Object3D *brkObjectB = (Object3D *) obB->getUserPointer();

                Logging::getInstance()->getInstance()->Log("contacto entre " + std::to_string(i) + " - " + brkObjectA->getLabel() + " y " + brkObjectB->getLabel());

            }

        }
    }*/

    if (this->bsp_map) {
        this->dynamicsWorld->removeCollisionObject(this->bsp_map->bspRigidBody);
    }
}

void Engine::cameraUpdate()
{
    camera->UpdateRotation();
    camera->UpdateFrustum();
}

void Engine::onUpdate()
{
    EngineBuffers::getInstance()->clearDepthBuffer();
    EngineBuffers::getInstance()->clearVideoBuffer();

    if (EngineSetup::getInstance()->ENABLE_SHADOW_CASTING) {
        // Clear every light's shadow mapping
        this->clearLightPointsShadowMappings();

        // Fill ShadowMapping FOR every object (mesh)
        this->objects3DShadowMapping();
    }

    this->drawBSP();
    this->drawMeshes();
    this->drawLightPoints();
    this->drawSprites();
    this->drawObjectsBillboard();

    if (EngineSetup::getInstance()->BULLET_DEBUG_MODE) {
        dynamicsWorld->debugDrawWorld();
    }

    if (EngineSetup::getInstance()->DRAW_FRUSTUM) {
        Drawable::drawFrustum(camera->frustum, camera, true, true, true);
    }

    Drawable::drawMainAxis( camera );
}

void Engine::clearLightPointsShadowMappings()
{
    for (int i = 0; i < this->numberLightPoints; i++) {
        if (!this->lightPoints[i]->isEnabled()) {
            continue;
        }

        this->lightPoints[i]->clearShadowMappingBuffer();
    }
}

void Engine::objects3DShadowMapping()
{
    for (int i = 0; i < this->numberGameObjects; i++) {
        if (!this->gameObjects[i]->isEnabled()) {
            continue;
        }

        Mesh3D *oMesh = dynamic_cast<Mesh3D*> (this->gameObjects[i]);
        if (oMesh != NULL) {
            for (int j = 0; j < this->numberLightPoints; j++) {
                if (oMesh->isShadowCaster()) {
                    oMesh->shadowMapping(this->lightPoints[j]);
                }
            }
        }
    }
}


void Engine::drawBSP()
{
    if (bsp_map) {
        bsp_map->DrawLeafVisibleSet( camera );
        if (EngineSetup::getInstance()->DRAW_BSP_HULLS) {
            bsp_map->DrawHulls( camera );
        }
    }
}

void Engine::drawMeshes()
{
    // draw meshes
    for (int i = 0; i < this->numberGameObjects; i++) {
        Mesh3D *oMesh = dynamic_cast<Mesh3D*> (this->gameObjects[i]);
        if (oMesh != NULL) {
            if (oMesh->isEnabled()) {
                oMesh->draw(camera);
                if (EngineSetup::getInstance()->TEXT_ON_OBJECT3D) {
                    Tools::writeText3D(Engine::renderer, camera, Engine::font, *oMesh->getPosition(), EngineSetup::getInstance()->TEXT_3D_COLOR, oMesh->getLabel());
                }
            }
        }
    }
}

void Engine::drawLightPoints()
{
    for (int i = 0; i < this->numberLightPoints; i++) {
        LightPoint3D *oLight= this->lightPoints[i];
        if (oLight != NULL) {
            if (oLight->isEnabled()) {
                oLight->getBillboard()->updateUnconstrainedQuad(
                    oLight,
                    camera->AxisUp(),
                    camera->AxisRight()
                );

                oLight->getBillboard()->reassignTexture();
                if (EngineSetup::getInstance()->DRAW_LIGHTPOINTS_BILLBOARD) {
                    Drawable::drawBillboard(oLight->getBillboard(), Engine::camera);
                }
                if (EngineSetup::getInstance()->DRAW_LIGHTPOINTS_AXIS) {
                    Drawable::drawObject3DAxis(oLight, camera, true, true, true);
                }
            }
        }
    }
}

void Engine::drawSprites()
{
    for (int i = 0; i < this->numberGameObjects; i++) {
        // Sprite Directional 3D
        SpriteDirectional3D *oSpriteDirectional = dynamic_cast<SpriteDirectional3D*> (this->gameObjects[i]);
        if (oSpriteDirectional != NULL) {
            if (!oSpriteDirectional->isEnabled()) {
                continue;
            }

            oSpriteDirectional->updateTrianglesCoordinates(camera);
            oSpriteDirectional->draw(camera);

            if (EngineSetup::getInstance()->TEXT_ON_OBJECT3D) {
                Tools::writeText3D(Engine::renderer, camera, Engine::font, *oSpriteDirectional->getPosition(), EngineSetup::getInstance()->TEXT_3D_COLOR, oSpriteDirectional->getLabel());
            }
        }

        // Sprite 3D
        Sprite3D *oSprite = dynamic_cast<Sprite3D*> (this->gameObjects[i]);
        if (oSprite != NULL) {
            if (!oSprite->isEnabled()) {
                continue;
            }

            oSprite->updateTrianglesCoordinatesAndTexture(camera);
            oSprite->draw(camera);

            if (EngineSetup::getInstance()->TEXT_ON_OBJECT3D) {
                Tools::writeText3D(Engine::renderer, camera, Engine::font, *oSprite->getPosition(), EngineSetup::getInstance()->TEXT_3D_COLOR, oSprite->getLabel());
            }
        }
    }
}

void Engine::drawObjectsBillboard()
{
    if (EngineSetup::getInstance()->DRAW_OBJECT3D_BILLBOARD) {

        Vertex3D u = camera->getRotation().getTranspose() * EngineSetup::getInstance()->up;
        Vertex3D r = camera->getRotation().getTranspose() * EngineSetup::getInstance()->right;

        // draw meshes
        for (int i = 0; i < this->numberGameObjects; i++) {
            if (this->gameObjects[i]->isDrawBillboard()) {

                this->gameObjects[i]->getBillboard()->updateUnconstrainedQuad(this->gameObjects[i], u, r );
                this->gameObjects[i]->getBillboard()->reassignTexture();
                Drawable::drawBillboard(this->gameObjects[i]->getBillboard(), Engine::camera);
            }
        }
    }
}

void Engine::onEnd()
{
    EngineBuffers::getInstance()->engineTimer.stop();
}

void Engine::addObject3D(Object3D *obj, std::string label)
{
    Logging::getInstance()->Log("Adding Object3D: '" + label + "'", "INFO");
    gameObjects[numberGameObjects] = obj;
    gameObjects[numberGameObjects]->setLabel(label);
    numberGameObjects++;
}

void Engine::addLightPoint(LightPoint3D *lightPoint, std::string label)
{
    Logging::getInstance()->Log("Adding LightPoint: '" + label + "'", "INFO");

    lightPoints[numberLightPoints] = lightPoint;
    numberLightPoints++;
}

Object3D* Engine::getObjectByLabel(std::string label)
{
    for (int i = 0; i < numberGameObjects; i++ ) {
        if (!gameObjects[i]->getLabel().compare(label)) {
            return gameObjects[i];
        }
    }
}

void Engine::Close()
{
    TTF_CloseFont( font );
    SDL_DestroyWindow( window );
    SDL_Quit();
}

Timer* Engine::getTimer()
{
    return &EngineSetup::getInstance()->engineTimer;
}

void Engine::loadBSP(const char *bspFilename, const char *paletteFilename)
{
    this->bsp_map = new BSPMap();

    this->bsp_map->Initialize(bspFilename, paletteFilename);
    this->bsp_map->InitializeSurfaces();
    this->bsp_map->InitializeTextures();
    this->bsp_map->InitializeLightmaps();
    this->bsp_map->InitializeTriangles();
    this->bsp_map->bindTrianglesLightmaps();
    this->bsp_map->InitializeEntities();
    this->bsp_map->createMesh3DPhysicFromHulls();

    // Load start position from BSP
    Vertex3D bspOriginalPosition = this->bsp_map->getStartMapPosition();
    this->camera->setPosition(bspOriginalPosition);

    btTransform initialTransform;
    initialTransform.setOrigin( btVector3(bspOriginalPosition.x, bspOriginalPosition.y, bspOriginalPosition.z) );

    this->camera->charCon->getGhostObject()->setWorldTransform(initialTransform);


}

void Engine::processFPS()
{
    /*countedFrames2++;

    if (timerCurrent == 0) {
        fps = countedFrames2;
        countedFrames2 = 0;
        Logging::getInstance()->Log(std::to_string(fps), "INFO");
    }*/

    fps = countedFrames / ( EngineBuffers::getInstance()->engineTimer.getTicks() / 1000.f );
    if( fps > 2000000 ) { fps = 0; }
    ++countedFrames;
}

void Engine::updatePhysicObjects()
{
    std::vector<Mesh3DPhysic *>::iterator it;
    for (it = meshPhysics.begin() ; it!= meshPhysics.end() ; it++) {
        (*it)->integrate( );
    }

    // Sync position
    btTransform t ;
    t = camera->charCon->getGhostObject()->getWorldTransform();
    Vertex3D pos = *this->camera->getPosition();
    btVector3 origin = t.getOrigin();
    t.setOrigin(btVector3(origin.x(), origin.y()-0.5, origin.z()));
    triggerCamera->getGhostObject()->setWorldTransform(t);
}

Mesh3D *Engine::getWeapon() const {
    return weapon;
}

void Engine::setWeapon(Mesh3D *weapon) {
    Engine::weapon = weapon;
}

void Engine::updateWeaponPosition()
{
    Vertex3D direction = camera->getRotation().getTranspose() * EngineSetup::getInstance()->forward;
    Vertex3D up        = camera->getRotation().getTranspose() * EngineSetup::getInstance()->up;
    Vertex3D right     = camera->getRotation().getTranspose() * EngineSetup::getInstance()->right;

    float farDist = 1;
    float YOffset = 0.5;
    float XOffset = -0.5;

    this->getWeapon()->setRotation(camera->getRotation().getTranspose());

    Vertex3D p = *camera->getPosition();
    p.x = p.x + direction.x * farDist + (up.x * YOffset) - (right.x * XOffset);
    p.y = p.y + direction.y * farDist + (up.y * YOffset) - (right.y * XOffset);
    p.z = p.z + direction.z * farDist + (up.z * YOffset) - (right.z * XOffset);

    this->getWeapon()->setPosition(p);
}

bool Engine::test(btCollisionWorld* collisionWorld)
{
    // Here we must refresh the overlapping paircache as the penetrating movement itself or the
    // previous recovery iteration might have used setWorldTransform and pushed us into an object
    // that is not in the previous cache contents from the last timestep, as will happen if we
    // are pushed into a new AABB overlap. Unhandled this means the next convex sweep gets stuck.
    //
    // Do this by calling the broadphase's setAabb with the moved AABB, this will update the broadphase
    // paircache and the ghostobject's internal paircache at the same time.    /BW

    btVector3 minAabb, maxAabb;
    camera->charCon->getGhostObject()->getCollisionShape()->getAabb(camera->charCon->getGhostObject()->getWorldTransform(), minAabb, maxAabb);
    collisionWorld->getBroadphase()->setAabb(camera->charCon->getGhostObject()->getBroadphaseHandle(),
                                             minAabb,
                                             maxAabb,
                                             collisionWorld->getDispatcher());

    bool penetration = false;

    collisionWorld->getDispatcher()->dispatchAllCollisionPairs(camera->charCon->getGhostObject()->getOverlappingPairCache(), collisionWorld->getDispatchInfo(), collisionWorld->getDispatcher());


    for (int i = 0; i < camera->charCon->getGhostObject()->getOverlappingPairCache()->getNumOverlappingPairs(); i++)
    {
        m_manifoldArray.resize(0);

        btBroadphasePair* collisionPair = &camera->charCon->getGhostObject()->getOverlappingPairCache()->getOverlappingPairArray()[i];

        btCollisionObject* obj0 = static_cast<btCollisionObject*>(collisionPair->m_pProxy0->m_clientObject);
        btCollisionObject* obj1 = static_cast<btCollisionObject*>(collisionPair->m_pProxy1->m_clientObject);

        if ((obj0 && !obj0->hasContactResponse()) || (obj1 && !obj1->hasContactResponse()))
            continue;

        if (!needsCollision(obj0, obj1))
            continue;

        if (collisionPair->m_algorithm)
            collisionPair->m_algorithm->getAllContactManifolds(m_manifoldArray);

        for (int j = 0; j < m_manifoldArray.size(); j++)
        {

            btPersistentManifold* manifold = m_manifoldArray[j];
            btScalar directionSign = manifold->getBody0() == camera->charCon->getGhostObject() ? btScalar(-1.0) : btScalar(1.0);

            if (manifold->getNumContacts() > 0) {
                Logging::getInstance()->Log("contacto");

            }
            //manifold->clearManifold();
        }
    }

    return penetration;
}

bool Engine::needsCollision(const btCollisionObject* body0, const btCollisionObject* body1)
{
    bool collides = (body0->getBroadphaseHandle()->m_collisionFilterGroup & body1->getBroadphaseHandle()->m_collisionFilterMask) != 0;
    collides = collides && (body1->getBroadphaseHandle()->m_collisionFilterGroup & body0->getBroadphaseHandle()->m_collisionFilterMask);
    return collides;
}