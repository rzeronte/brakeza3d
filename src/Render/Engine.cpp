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
#include <chrono>
#include <iostream>

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

void Engine::drawGUI()
{
    ImGui::NewFrame();

    ///bool open = true;
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
    //cam->setPosition( EngineSetup::getInstance()->CameraPosition );
    //cam->setRotation( Rotation3D(0, 0, 0) );
}

void Engine::preUpdate()
{
    // Posición inicial del vector velocidad que llevará la cámara
    camera->collider->basePoint = *camera->getPosition();

    // Si hay mapa BSP cargado, determinados su VPS
    if (bsp_map) {
        bspleaf_t *leaf = bsp_map->FindLeaf( camera );
        bsp_map->setVisibleSet(leaf);
    }
}

void Engine::postUpdate()
{
    if ( EngineSetup::getInstance()->BSP_COLLISIONS_ENABLED) {
        this->collideAndSlide(
            Vector3D(camera->collider->basePoint, *camera->getPosition()).getComponent(),
            EngineSetup::getInstance()->gravity
        );
    } else {
        this->cameraUpdate();
    }
}

void Engine::cameraUpdate()
{
    camera->UpdateRotation();
    camera->UpdatePosition();
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
    }
}

void Engine::checkCollisionsMesh()
{
    // draw meshes
    for (int i = 0; i < this->numberGameObjects; i++) {
        Mesh3D *oMesh = dynamic_cast<Mesh3D*> (this->gameObjects[i]);
        if (oMesh != NULL) {
            if (oMesh->isEnabled()) {
                for (int j = 0; j< oMesh->n_triangles; j++) {
                    oMesh->model_triangles[j].isCollisionWithSphere(camera->collider, EngineSetup::getInstance()->PLAYER_SPHERE_RADIUS, camera);
                }
            }
        }
    }
}

void Engine::checkCollisionsBSP()
{
    if (bsp_map) {
        bsp_map->PhysicsLeafVisibleSet( camera );
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
            };
        }
    }
}

void Engine::drawLightPoints()
{
    for (int i = 0; i < this->numberLightPoints; i++) {
        LightPoint3D *oLight= this->lightPoints[i];
        if (oLight != NULL) {
            if (oLight->isEnabled()) {
                oLight->billboard->updateUnconstrainedQuad( 0.3, 0.3, oLight, camera->AxisUp(), camera->AxisRight() );
                if (EngineSetup::getInstance()->DRAW_LIGHTPOINTS_BILLBOARD) {
                    Drawable::drawBillboard(oLight->billboard, Engine::camera);
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
            };

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
            };
            oSprite->updateTrianglesCoordinatesAndTexture(camera);
            oSprite->draw(camera);

            if (EngineSetup::getInstance()->TEXT_ON_OBJECT3D) {
                Tools::writeText3D(Engine::renderer, camera, Engine::font, *oSprite->getPosition(), EngineSetup::getInstance()->TEXT_3D_COLOR, oSprite->getLabel());
            }
        }

        // Weapon 3D
        Weapon3D *oWeapon = dynamic_cast<Weapon3D*> (this->gameObjects[i]);
        if (oWeapon != NULL) {
            if (!oWeapon->isEnabled()) {
                continue;
            };
            oWeapon->setWeaponPosition(camera);
            oWeapon->updateTrianglesCoordinatesAndTexture(camera);
            oWeapon->draw(camera);

            if (EngineSetup::getInstance()->TEXT_ON_OBJECT3D) {
                Tools::writeText3D(Engine::renderer, camera, Engine::font, *oWeapon->getPosition(), EngineSetup::getInstance()->TEXT_3D_COLOR, oWeapon->getLabel());
            }
        }

    }
}

void Engine::onEnd() {

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
    return &this->engineTimer;
}

void Engine::loadBSP(const char *bspFilename, const char *paletteFilename)
{
    this->bsp_map = new BSPMap();
    this->bsp_map->Initialize(bspFilename, paletteFilename);
    this->bsp_map->InitializeSurfaces();
    this->bsp_map->InitializeTextures();
    this->bsp_map->InitializeTriangles(this->camera);
}

Vertex3D Engine::collideWithWorld( Vertex3D pos, Vertex3D vel, int &collisionRecursionDepth, Collider *collider)
{
    const float unitsPerMeter = 100.0f;

    // All hard-coded distances in this function is
    // scaled to fit the setting above..
    float unitScale = unitsPerMeter / 100.0f;
    float veryCloseDistance = 0.005f * unitScale;

    // do we need to worry?
    if (collisionRecursionDepth > 5) {
        return pos;
    }

    // Ok, we need to worry:
    collider->velocity = vel;
    collider->normalizedVelocity = vel.getNormalize();
    collider->basePoint = pos;
    collider->foundCollision = false;

    // Check for collision (calls the collision routines)
    // Application specific!!
    this->checkCollisionsBSP();
    this->checkCollisionsMesh();

    // If no collision we just move along the velocity
    if (!collider->foundCollision) {
        return pos + vel;
    }

    // *** Collision occured ***
    // The original destination point
    Vertex3D destinationPoint = pos + vel;
    Vertex3D newBasePoint = pos;

    // only update if we are not already very close
    // and if so we only move very close to intersection..not
    // to the exact spot.
    if (collider->nearestDistance >= veryCloseDistance) {
        Vertex3D V = vel;
        V.setLength(collider->nearestDistance - veryCloseDistance);
        newBasePoint = collider->basePoint + V;

        // Adjust polygon intersection point (so sliding
        // plane will be unaffected by the fact that we
        // move slightly less than collision tells us)

        V = V.getNormalize();
        collider->intersectionPoint = collider->intersectionPoint - V.getScaled(veryCloseDistance);
    }

    // Determine the sliding plane
    Vertex3D slidePlaneOrigin = collider->intersectionPoint;
    Vertex3D slidePlaneNormal = newBasePoint - collider->intersectionPoint;
    slidePlaneNormal = slidePlaneNormal.getNormalize();
    Plane slidingPlane(slidePlaneOrigin,slidePlaneNormal);

    // Again, sorry about formatting.. but look carefully ;)
    float d = slidingPlane.distance(destinationPoint);
    Vertex3D newDestinationPoint = destinationPoint - slidePlaneNormal.getScaled(d);

    // Generate the slide vector, which will become our new
    // velocity vector for the next iteration
    Vertex3D newVelocityVector = newDestinationPoint - collider->intersectionPoint;

    // Recurse:
    // dont recurse if the new velocity is very small
    if (newVelocityVector.getModule() < veryCloseDistance) {
        return newBasePoint;
    }

    collisionRecursionDepth++;
    return collideWithWorld(newBasePoint, newVelocityVector, collisionRecursionDepth, collider);
}

void Engine::collideAndSlide(Vertex3D vel, Vertex3D gravity)
{
    // slide
    int collisionRecursionDepth = 0;
    Vertex3D finalPosition = collideWithWorld(camera->collider->basePoint, vel, collisionRecursionDepth, camera->collider);

    if (camera->collider->jumping) {
        // jumping
        collisionRecursionDepth = 0;
        camera->collider->jumpVelocity = camera->collider->jumpVelocity - EngineSetup::getInstance()->gravity.getScaled(camera->collider->startJumpTime);
        camera->collider->startJumpTime += 1/this->getDeltaTime();

        if (camera->collider->jumpVelocity.y <= 0) {
            camera->collider->jumping = false;
        }

        camera->collider->basePoint = finalPosition;
        camera->collider->velocity = camera->collider->jumpVelocity;
        camera->collider->normalizedVelocity = camera->collider->jumpVelocity.getNormalize();

        finalPosition = collideWithWorld(finalPosition, camera->collider->jumpVelocity.getInverse(), collisionRecursionDepth, camera->collider);

    } else {
        // gravity
        camera->collider->basePoint = finalPosition;
        camera->collider->velocity = gravity;
        camera->collider->normalizedVelocity = gravity.getNormalize();

        collisionRecursionDepth = 0;
        finalPosition = collideWithWorld(finalPosition, gravity, collisionRecursionDepth, camera->collider);
    }

    camera->setPosition(finalPosition);
    cameraUpdate();
}

void Engine::updateTimer()
{
    this->current_ticks = this->engineTimer.getTicks();
    this->deltaTime = this->current_ticks - this->last_ticks;
    this->last_ticks = this->current_ticks;

    this->timerCurrent+= this->deltaTime/1000.f;

    //float step = (float) 1 / 1;

    if (timerCurrent >= 1) {
        timerCurrent = 0;
    }
}

float Engine::getDeltaTime()
{
    return this->deltaTime;
}

void Engine::processFPS()
{
    this->updateTimer();

    /*countedFrames2++;

    if (timerCurrent == 0) {
        fps = countedFrames2;
        countedFrames2 = 0;
        Logging::getInstance()->Log(std::to_string(fps), "INFO");
    }*/

    fps = countedFrames / ( engineTimer.getTicks() / 1000.f );
    if( fps > 2000000 ) { fps = 0; }
    ++countedFrames;
}