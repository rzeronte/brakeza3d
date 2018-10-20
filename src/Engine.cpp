#include "../headers/Engine.h"
#include "../headers/Mesh.h"
#include "../headers/EngineBuffers.h"
#include "../headers/Frustum.h"
#include "../headers/Object3D.h"
#include "../headers/LTimer.h"
#include "../headers/Drawable.h"
#include "../headers/Render.h"
#include "../headers/LightPoint.h"
#include "../headers/Logging.h"
#include "../headers/SpriteDirectionalObject3D.h"
#include "../headers/SpriteObject3D.h"
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
    this->lightPoints = new LightPoint *[EngineSetup::getInstance()->ENGINE_MAX_GAMEOBJECTS];
    this->numberLightPoints = 0;

    // cam
    cam = new Camera();
    cam->setLabel("Camera");

    // input controller
    cont = new Controller();

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

void Engine::cameraUpdate()
{
    if (EngineSetup::getInstance()->CAMERA_MOUSE_ROTATION) {
        cont->handleMouse(&this->e, this->cam);
    }

    cont->handleKeyboard(&this->e, this->cam, this->finish);
    cam->syncFrustum();
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
        lightPoints, numberLightPoints
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

    SDL_RenderCopy(renderer, SDL_CreateTextureFromSurface(renderer, EngineBuffers::getInstance()->getScreenSurface()), NULL, NULL);

    //SDL_RenderPresent( renderer );
    //SDL_RenderClear(renderer);
    //SDL_UpdateWindowSurface( window );
    //SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
}

void Engine::onStart()
{
    cam->setPosition( EngineSetup::getInstance()->CameraPosition );
    cam->setRotation( Vertex(0, 0, 0) );
}

void Engine::onUpdateEvent()
{
    // handle Keyboard
    for (int i = 0; i < this->numberGameObjects; i++) {
        if ( this->gameObjects[i]->isHandleKeyboard() ) {
            this->cont->handleKeyboardObject3D(&this->e, this->gameObjects[i]);
        }
    }

    for (int i = 0; i < this->numberLightPoints; i++) {
        this->lightPoints[i]->syncFrustum();
        if ( this->lightPoints[i]->isHandleKeyboard() ) {
            this->cont->handleKeyboardObject3D(&this->e, this->lightPoints[i]);
        }
    }
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

    this->drawMeshes();
    this->drawLightPoints();
    this->drawSprites();

    if (EngineSetup::getInstance()->DRAW_FRUSTUM) {
        Drawable::drawFrustum(cam->frustum, cam, true, true, true);
    }

    Drawable::drawMainAxis( cam );

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

        Mesh *oMesh = dynamic_cast<Mesh*> (this->gameObjects[i]);
        if (oMesh != NULL) {
            for (int j = 0; j < this->numberLightPoints; j++) {
                if (oMesh->isShadowCaster()) {
                    oMesh->shadowMapping(this->lightPoints[j]);
                }
            }
        }
    }
}

void Engine::drawMeshes()
{
    // draw meshes
    for (int i = 0; i < this->numberGameObjects; i++) {
        Mesh *oMesh = dynamic_cast<Mesh*> (this->gameObjects[i]);
        if (oMesh != NULL) {
            if (!oMesh->isEnabled()) {
                continue;
            };
            oMesh->draw(cam);
            if (EngineSetup::getInstance()->TEXT_ON_OBJECT3D) {
                Tools::writeText3D(Engine::renderer, cam, Engine::font, oMesh->position, EngineSetup::getInstance()->TEXT_3D_COLOR, oMesh->getLabel());
            }
        }
    }
}

void Engine::drawLightPoints()
{
    for (int i = 0; i < this->numberLightPoints; i++) {
        LightPoint *oLight= this->lightPoints[i];
        if (oLight != NULL) {
            if (!oLight->isEnabled()) {
                continue;
            }

            oLight->billboard->updateUnconstrainedQuad( 0.3, 0.3, oLight, cam->upVector(), cam->rightVector() );
            if (EngineSetup::getInstance()->DRAW_LIGHTPOINTS_BILLBOARD) {
                Drawable::drawBillboard(oLight->billboard, Engine::cam);
            }
            if (EngineSetup::getInstance()->DRAW_LIGHTPOINTS_AXIS) {
                Drawable::drawObject3DAxis(oLight, cam, true, true, true);
            }
        }
    }
}

void Engine::drawSprites()
{
    for (int i = 0; i < this->numberGameObjects; i++) {
        // Sprite Directional 3D
        SpriteDirectionalObject3D *oSpriteDirectional = dynamic_cast<SpriteDirectionalObject3D*> (this->gameObjects[i]);
        if (oSpriteDirectional != NULL) {
            if (!oSpriteDirectional->isEnabled()) {
                continue;
            };

            oSpriteDirectional->updateTrianglesCoordinates(cam);
            oSpriteDirectional->draw(cam);

            if (EngineSetup::getInstance()->TEXT_ON_OBJECT3D) {
                Tools::writeText3D(Engine::renderer, cam, Engine::font, oSpriteDirectional->position, EngineSetup::getInstance()->TEXT_3D_COLOR, oSpriteDirectional->getLabel());
            }

        }

        // Sprite 3D
        SpriteObject3D *oSprite = dynamic_cast<SpriteObject3D*> (this->gameObjects[i]);
        if (oSprite != NULL) {
            if (!oSprite->isEnabled()) {
                continue;
            };
            oSprite->updateTrianglesCoordinatesAndTexture(cam);
            oSprite->draw(cam);

            if (EngineSetup::getInstance()->TEXT_ON_OBJECT3D) {
                Tools::writeText3D(Engine::renderer, cam, Engine::font, oSprite->position, EngineSetup::getInstance()->TEXT_3D_COLOR, oSprite->getLabel());
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

void Engine::addLightPoint(LightPoint *lightPoint, std::string label)
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

Controller *Engine::getController() const {
    return cont;
}

void Engine::processFPS() {
    fps = countedFrames / ( fpsTimer.getTicks() / 1000.f );
    if( fps > 2000000 ) { fps = 0; }
    ++countedFrames;
}

LTimer* Engine::getTimer()
{
    return &this->fpsTimer;
}



