
#include "../headers/Brakeza3D.h"
#include "../headers/Render/Drawable.h"
#include "../headers/Render/Maths.h"
#include "../headers/EngineBuffers.h"
#include "../headers/Misc/Parallells.h"
#include <thread>

Brakeza3D* Brakeza3D::instance = 0;

Brakeza3D::Brakeza3D()
{
}

Brakeza3D* Brakeza3D::get()
{
    if (instance == 0) {
        instance = new Brakeza3D();
    }

    return instance;
}

void Brakeza3D::setController(InputController *ic)
{
    this->controllerManager = ic;
}

void Brakeza3D::start()
{
    ImGui::CreateContext();
    guiManager = new GUIManager();
    Logging::getInstance()->setGUILog(guiManager->guiLog);

    initWindow();
    initSound();
    initFontsTTF();

    controllerManager = new InputController();
    weaponManager     = new WeaponsManager();
    collisionsManager = new CollisionsManager();
    bsp               = new BSPMap();
    camera            = new Camera3D();
    menuManager       = new MenuManager();

    splashCounter = new Counter(1);

    setup  = EngineSetup::getInstance();
    buffer = EngineBuffers::getInstance();


    engineTimer.start();
    this->drawSplash(screenSurface);
}

bool Brakeza3D::initWindow()
{
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    } else {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

        //Create window
        window = SDL_CreateWindow(
                EngineSetup::getInstance()->ENGINE_TITLE.c_str(),
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                EngineSetup::getInstance()->screenWidth,
                EngineSetup::getInstance()->screenHeight,
                SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE
        );
        // | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE

        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return false;
        } else {
            gl_context = SDL_GL_CreateContext(window);
            screenSurface = SDL_CreateRGBSurface(0, EngineSetup::getInstance()->screenWidth, EngineSetup::getInstance()->screenHeight, 32, 0, 0, 0, 0);
            SDL_SetSurfaceBlendMode(screenSurface, SDL_BLENDMODE_NONE);
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED );

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

            Logging::getInstance()->Log(
                    "Window size: " + std::to_string(EngineSetup::getInstance()->screenWidth) + " x " + std::to_string(EngineSetup::getInstance()->screenHeight), "INFO"
            );

            SDL_GL_SwapWindow(window);
        }
    }

    return true;
}

void Brakeza3D::initFontsTTF()
{
    Logging::getInstance()->Log("Initializating TTF...", "INFO");

    // global font
    if (TTF_Init() < 0) {
        Logging::getInstance()->Log(TTF_GetError(), "INFO");
    } else {
        std::string pathFont = EngineSetup::getInstance()->FONTS_FOLDER + "octin.ttf";
        Logging::getInstance()->Log("Loading FONT: " + pathFont, "INFO");

        fontDefault = TTF_OpenFont( pathFont.c_str(), 50 );
        fontSmall   = TTF_OpenFont( pathFont.c_str(), 25 );
        fontMedium  = TTF_OpenFont( pathFont.c_str(), 70 );
        fontBig     = TTF_OpenFont( pathFont.c_str(), 200 );

        if (!fontDefault) Logging::getInstance()->Log(TTF_GetError(), "INFO");
        if (!fontSmall) Logging::getInstance()->Log(TTF_GetError(), "INFO");
        if (!fontMedium) Logging::getInstance()->Log(TTF_GetError(), "INFO");
        if (!fontBig) Logging::getInstance()->Log(TTF_GetError(), "INFO");
    }
}

bool Brakeza3D::initSound()
{
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
    }

    Mix_Volume(EngineSetup::SoundChannels::SND_MENU, EngineSetup::getInstance()->SOUND_VOLUME_MENU);
    Mix_Volume(EngineSetup::SoundChannels::SND_PLAYER, EngineSetup::getInstance()->SOUND_VOLUME_PLAYER);
    Mix_Volume(EngineSetup::SoundChannels::SND_ENVIRONMENT, EngineSetup::getInstance()->SOUND_VOLUME_ENVIRONMENT);
    Mix_VolumeMusic(EngineSetup::getInstance()->SOUND_VOLUME_MUSIC);

}

void Brakeza3D::initTiles()
{
    if (EngineSetup::getInstance()->screenWidth % this->sizeTileWidth != 0) {
        printf("Bad sizetileWidth\r\n");
        exit(-1);
    }
    if (EngineSetup::getInstance()->screenHeight % this->sizeTileHeight != 0) {
        printf("Bad sizeTileHeight\r\n");
        exit(-1);
    }

    // Tiles Raster setup
    this->tilesWidth  = EngineSetup::getInstance()->screenWidth / this->sizeTileWidth;
    this->tilesHeight = EngineSetup::getInstance()->screenHeight / this->sizeTileHeight;
    this->numTiles = tilesWidth * tilesHeight;
    this->tilePixelsBufferSize = this->sizeTileWidth*this->sizeTileHeight;

    Logging::getInstance()->Log("Tiles: ("+std::to_string(tilesWidth)+"x"+std::to_string(tilesHeight)+"), Size: ("+std::to_string(sizeTileWidth)+"x"+std::to_string(sizeTileHeight)+") - bufferTileSize: " + std::to_string(sizeTileWidth*sizeTileHeight));

    for (int y = 0 ; y < EngineSetup::getInstance()->screenHeight; y+=this->sizeTileHeight) {
        for (int x = 0 ; x < EngineSetup::getInstance()->screenWidth; x+=this->sizeTileWidth) {

            Tile t;

            t.draw    = true;
            t.id_x    = (x/this->sizeTileWidth);
            t.id_y    = (y/this->sizeTileHeight);
            t.id      = t.id_y * this->tilesWidth + t.id_x;
            t.start_x = x;
            t.start_y = y;

            this->tiles.emplace_back(t);
            // Load up the vector with MyClass objects


            Logging::getInstance()->Log("Tiles: (id:" + std::to_string(t.id) + "), (offset_x: " + std::to_string(x)+", offset_y: " + std::to_string(y) + ")");
        }
    }

    // Create local buffers and openCL buffer pointers
    for (int i = 0; i < numTiles; i++) {

        this->tiles[i].buffer = new unsigned int[tilePixelsBufferSize];
        for (int j = 0; j < tilePixelsBufferSize ; j++) {
            this->tiles[i].buffer[j] = Color::red();
        }

        this->tiles[i].bufferDepth = new float[tilePixelsBufferSize];
        for (int j = 0; j < tilePixelsBufferSize ; j++) {
            this->tiles[i].bufferDepth[j] = 0;
        }
    }
}

void Brakeza3D::drawTilesGrid()
{
    for (int j = 0 ; j < (this->tilesWidth*this->tilesHeight) ; j++) {
        Tile *t = &this->tiles[j];
        Uint32 color = Color::white();

        if (t->numTriangles > 0) {
            color = Color::red();
        }

        Line2D top = Line2D(t->start_x, t->start_y, t->start_x+sizeTileWidth, t->start_y);
        Line2D left = Line2D(t->start_x, t->start_y, t->start_x, t->start_y+sizeTileHeight);
        Drawable::drawLine2D(top, color);
        Drawable::drawLine2D(left, color);
    }
}

Camera3D *Brakeza3D::getCamera()
{
    return camera;
}

std::vector<Object3D*> &Brakeza3D::getSceneObjects()
{
    return sceneObjects;
}

std::vector<LightPoint3D*> &Brakeza3D::getLightPoints()
{
    return lightPoints;
}

void Brakeza3D::addObject3D(Object3D *obj, std::string label)
{
    Logging::getInstance()->Log("Adding Object3D: '" + label + "'", "INFO");
    obj->setLabel(label);
    sceneObjects.emplace_back(obj);
}


Object3D* Brakeza3D::getObjectByLabel(std::string label)
{
    for (int i = 0; i < this->sceneObjects.size(); i++ ) {
        if (!sceneObjects[i]->getLabel().compare(label)) {
            return sceneObjects[i];
        }
    }
}

void Brakeza3D::addLightPoint(LightPoint3D *lightPoint, std::string label)
{
    Logging::getInstance()->Log("Adding LightPoint: '" + label + "'", "INFO");

    lightPoints.push_back(lightPoint);
}

InputController *Brakeza3D::getController()
{
    return controllerManager;
}

CollisionsManager *Brakeza3D::getCollisionManager()
{
    return collisionsManager;
}

BSPMap *Brakeza3D::getBSP()
{
    return bsp;
}

WeaponsManager *Brakeza3D::getWeaponsManager()
{
    return weaponManager;
}

GUIManager *Brakeza3D::getGUIManager()
{
    return guiManager;
}

MenuManager *Brakeza3D::getMenuManager()
{
    return menuManager;
}

Timer* Brakeza3D::getTimer()
{
    return &this->engineTimer;
}

void Brakeza3D::updateTimer()
{
    current_ticks = engineTimer.getTicks();
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;

    frameTime += deltaTime;
    executionTime += deltaTime / 1000.f;

    splashCounter->update();
}


float Brakeza3D::getDeltaTime()
{
    return this->deltaTime/1000;
}


void Brakeza3D::updateFPS()
{
    if (!this->setup->DRAW_FPS) return;

    ++fpsFrameCounter;
    if (Brakeza3D::get()->frameTime > 1000) {
        fps = fpsFrameCounter;
        frameTime = 0;
        fpsFrameCounter = 0;
    }

    Tools::writeTextCenterHorizontal(renderer, fontSmall, Color::yellow(), std::to_string(fps) +"fps", 20);
}

void Brakeza3D::initBSP(const char *bspFilename, std::vector<Triangle*> *frameTriangles)
{
    this->loadingBSP = new std::thread(ParallellInitBSP, bspFilename, frameTriangles);
}

void Brakeza3D::setCameraInBSPStartPosition()
{
    // Load start position from BSP
    Vertex3D bspOriginalPosition = Brakeza3D::get()->getBSP()->getStartMapPosition();

    int entityID = Brakeza3D::get()->getBSP()->getIndexOfFirstEntityByClassname("info_player_start");
    btTransform initialTransform;
    initialTransform.setOrigin( btVector3(bspOriginalPosition.x, bspOriginalPosition.y, bspOriginalPosition.z) );

    char *angle = Brakeza3D::get()->getBSP()->getEntityValue(entityID, "angle");
    int angleInt = atoi( std::string(angle).c_str() );

    Brakeza3D::get()->getCamera()->yaw   = 90 - angleInt;
    Brakeza3D::get()->getCamera()->pitch = 0;
    Brakeza3D::get()->getCamera()->roll  = 0;

    Brakeza3D::get()->getCamera()->setPosition( bspOriginalPosition );

    btPairCachingGhostObject *kinematicGhost = Brakeza3D::get()->getCamera()->kinematicController->getGhostObject();
    kinematicGhost->setWorldTransform(initialTransform);
}

void Brakeza3D::triangleRasterizer(Triangle *t)
{
    // LOD determination
    t->lod = t->processLOD();

    // Triangle setup
    int A01 = (int) -(t->As.y - t->Bs.y);
    int A12 = (int) -(t->Bs.y - t->Cs.y);
    int A20 = (int) -(t->Cs.y - t->As.y);

    int B01 = (int) -(t->Bs.x - t->As.x);
    int B12 = (int) -(t->Cs.x - t->Bs.x);
    int B20 = (int) -(t->As.x - t->Cs.x);

    Point2D startP(t->minX, t->minY);
    int w0_row = Maths::orient2d(t->Bs, t->Cs, startP);
    int w1_row = Maths::orient2d(t->Cs, t->As, startP);
    int w2_row = Maths::orient2d(t->As, t->Bs, startP);

    float alpha, theta, gamma, depth, affine_uv, texu, texv, lightu, lightv;

    int screenWidth = setup->screenWidth;

    for (int y = t->minY ; y < t->maxY ; y++) {
        int w0 = w0_row;
        int w1 = w1_row;
        int w2 = w2_row;

        for (int x = t->minX ; x < t->maxX ; x++) {

            if ((w0 | w1 | w2) > 0) {
                alpha = w0 * t->reciprocalFullArea;
                theta = w1 * t->reciprocalFullArea;
                gamma = 1 - alpha - theta;

                depth = alpha * (t->An.z) + theta * (t->Bn.z) + gamma * (t->Cn.z);

                int bufferIndex = ( y * screenWidth ) + x;

                if (t->parent->isDecal()) {
                    depth-=1;
                }

                if ( depth < buffer->getDepthBuffer( bufferIndex )) {
                    affine_uv = 1 / ( alpha * (t->persp_correct_Az) + theta * (t->persp_correct_Bz) + gamma * (t->persp_correct_Cz) );
                    texu   = ( alpha * (t->tex_u1_Ac_z)   + theta * (t->tex_u2_Bc_z)   + gamma * (t->tex_u3_Cc_z) )   * affine_uv;
                    texv   = ( alpha * (t->tex_v1_Ac_z)   + theta * (t->tex_v2_Bc_z)   + gamma * (t->tex_v3_Cc_z) )   * affine_uv;

                    lightu = ( alpha * (t->light_u1_Ac_z) + theta * (t->light_u2_Bc_z) + gamma * (t->light_u3_Cc_z) ) * affine_uv;
                    lightv = ( alpha * (t->light_v1_Ac_z) + theta * (t->light_v2_Bc_z) + gamma * (t->light_v3_Cc_z) ) * affine_uv;


                    this->processPixel(
                            t,
                            bufferIndex,
                            x, y,
                            alpha, theta, gamma,
                            depth,
                            texu, texv,
                            lightu, lightv
                    );

                }
            }

            // edge function increments
            w0 += A12;
            w1 += A20;
            w2 += A01;
        }

        w0_row += B12;
        w1_row += B20;
        w2_row += B01;
    }
}

void Brakeza3D::processPixel(Triangle *t, int bufferIndex, int x, int y, float w0, float w1, float w2, float z, float texu, float texv, float lightu, float lightv)
{
    Uint32 pixelColor = NULL;

    // Gradient
    if (setup->TRIANGLE_MODE_COLOR_SOLID) {
        pixelColor = (Uint32) Tools::createRGB(w0 * 255, w1 * 255, w2 * 255);
    }

    // Texture
    if (setup->TRIANGLE_MODE_TEXTURIZED && t->getTexture() != NULL) {
        if (t->getTexture()->liquid && setup->TRIANGLE_TEXTURES_ANIMATED ) {
            float cache1 = texu / setup->LAVA_CLOSENESS;
            float cache2 = texv / setup->LAVA_CLOSENESS;
            texu = (cache1 + setup->LAVA_INTENSITY * sin(setup->LAVA_SPEED * this->executionTime + cache2) ) * setup->LAVA_SCALE;
            texv = (cache2 + setup->LAVA_INTENSITY * sin(setup->LAVA_SPEED * this->executionTime + cache1) ) * setup->LAVA_SCALE;
        }

        if ( t->parent->isDecal() ) {
            if ((texu < 0 || texu > 1) || (texv < 0 || texv > 1) ) {
                return;
            }
        }

        t->processPixelTexture(pixelColor, texu, texv );

        Uint8 red, green, blue, alpha;
        SDL_GetRGBA(pixelColor, t->texture->getSurface(t->lod)->format, &red, &green, &blue, &alpha);

        if (alpha == 0) {
            return;
        } else {
            pixelColor = Tools::alphaBlend( buffer->getVideoBuffer( x, y ), pixelColor, alpha );
        }

        if (!t->parent->isDecal() && t->getLightmap()->isLightMapped() && setup->ENABLE_LIGHTMAPPING) {
            t->processPixelLightmap(pixelColor, lightu, lightv);
        }
    }

    if (EngineSetup::getInstance()->ENABLE_FOG) {

        float nZ = Maths::normalizeToRange(z, 0, setup->FOG_DISTANCE);

        if (nZ >= 1) {
            pixelColor = setup->FOG_COLOR;
        } else {
            pixelColor = Tools::mixColor(pixelColor, setup->FOG_COLOR, nZ * setup->FOG_INTENSITY);
        }

    }

    /*if (EngineSetup::getInstance()->ENABLE_LIGHTS) {
        Vertex3D D;

        if (this->numberLightPoints > 0) {
            // Coordenadas del punto que estamos procesando en el mundo (object space)
            float x3d = w0 * Ao.x + w1 * Bo.x + w2 * Co.x;
            float y3d = w0 * Ao.y + w1 * Bo.y + w2 * Co.y;
            float z3d = w0 * Ao.z + w1 * Bo.z + w2 * Co.z;

            D = Vertex3D( x3d, y3d, z3d ); // Object space
        }

        for (int i = 0; i < this->numberLightPoints; i++) {
            if (!this->lightPoints[i]->isEnabled()) {
                continue;
            }

            // Color light apply
            float d = Maths::distanteBetweenpoints( *this->lightPoints[i]->getPosition(), D );
            pixelColor = Maths::mixColor(pixelColor, d, this->lightPoints[i], D);

            if (EngineSetup::get()->ENABLE_SHADOW_CASTING) {
                Mesh3D *isMesh = dynamic_cast<Mesh3D*> (parent);

                if (isMesh != NULL && isMesh->isShadowCaster()) {
                    // Shadow test
                    Vertex3D Dl = Transforms::cameraSpace(D, this->lightPoints[i]->cam);
                    Dl = Transforms::NDCSpace(Dl, this->lightPoints[i]->cam);
                    const Point2D DP = Transforms::screenSpace(Dl, this->lightPoints[i]->cam);

                    if (Tools::isPixelInWindow(DP.x, DP.y)) {
                        float buffer_shadowmapping_z = this->lightPoints[i]->getShadowMappingBuffer(DP.x, DP.y);
                        if ( Dl.z > buffer_shadowmapping_z) {
                            pixelColor = Color::red();
                        }
                    }
                }
            }
        }
    }*/

    buffer->setDepthBuffer(bufferIndex, z);
    buffer->setVideoBuffer(bufferIndex, pixelColor);
}


void Brakeza3D::drawTileTriangles(int i, std::vector<Triangle*> &visibleTriangles)
{
    for (int j = 0 ; j < this->tiles[i].numTriangles ; j++) {
        int triangleId = this->tiles[i].triangleIds[j];
        Tile *t = &this->tiles[i];
        Triangle *tr = visibleTriangles[triangleId];

        this->softwareRasterizerForTile(
                tr,
                t->start_x,
                t->start_y,
                t->start_x+sizeTileWidth,
                t->start_y+sizeTileHeight
        );
    }
}


void Brakeza3D::drawTilesTriangles(std::vector<Triangle*> *visibleTriangles)
{
    std::vector<std::thread> threads;
    for (int i = 0 ; i < Brakeza3D::get()->numTiles ; i++) {
        if (!Brakeza3D::get()->tiles[i].draw) continue;

        if (EngineSetup::getInstance()->BASED_TILE_RENDER_THREADED) {
            threads.emplace_back( std::thread(ParallellDrawTileTriangles, i, visibleTriangles) );
        } else {
            Brakeza3D::get()->drawTileTriangles(i, *visibleTriangles);
        }
    }

    if (EngineSetup::getInstance()->BASED_TILE_RENDER_THREADED) {
        for (std::thread & th : threads) {
            if (th.joinable()) {
                th.join();
            }
        }
    }
}

void Brakeza3D::handleTrianglesToTiles(std::vector<Triangle*> &visibleTriangles) {
    for (int i = 0; i < this->numTiles; i++) {
        this->tiles[i].numTriangles = 0;
    }

    for (int i = 0; i < visibleTriangles.size(); i++) {
        int tileStartX = std::max((float) (visibleTriangles[i]->minX / this->sizeTileWidth), 0.0f);
        int tileEndX = std::min((float) (visibleTriangles[i]->maxX / this->sizeTileWidth), (float) this->tilesWidth - 1);

        int tileStartY = std::max((float) (visibleTriangles[i]->minY / this->sizeTileHeight), 0.0f);
        int tileEndY = std::min((float) (visibleTriangles[i]->maxY / this->sizeTileHeight), (float) this->tilesHeight - 1);

        for (int y = tileStartY; y <= tileEndY; y++) {
            for (int x = tileStartX; x <= tileEndX; x++) {
                int tileOffset = y * tilesWidth + x;
                this->tiles[tileOffset].triangleIds[this->tiles[tileOffset].numTriangles] = i;
                this->tiles[tileOffset].numTriangles++;
            }
        }
    }
}

void Brakeza3D::softwareRasterizerForTile(Triangle *t, int minTileX, int minTileY, int maxTileX, int maxTileY)
{
    EngineSetup*   engineSetup   = EngineSetup::getInstance();
    EngineBuffers* engineBuffers = EngineBuffers::getInstance();

    // LOD determination
    t->lod = t->processLOD();

    // Triangle setup
    int A01 = (int) -(t->As.y - t->Bs.y);
    int A12 = (int) -(t->Bs.y - t->Cs.y);
    int A20 = (int) -(t->Cs.y - t->As.y);

    int B01 = (int) -(t->Bs.x - t->As.x);
    int B12 = (int) -(t->Cs.x - t->Bs.x);
    int B20 = (int) -(t->As.x - t->Cs.x);

    Point2D startP(t->minX, t->minY);
    int w0_row = Maths::orient2d(t->Bs, t->Cs, startP);
    int w1_row = Maths::orient2d(t->Cs, t->As, startP);
    int w2_row = Maths::orient2d(t->As, t->Bs, startP);

    float alpha, theta, gamma, depth, affine_uv, texu, texv, lightu, lightv;

    for (int y = t->minY ; y < t->maxY ; y++) {
        int w0 = w0_row;
        int w1 = w1_row;
        int w2 = w2_row;

        for (int x = t->minX ; x < t->maxX ; x++) {

            if ((w0 | w1 | w2) > 0) {

                alpha = w0 * t->reciprocalFullArea;
                theta = w1 * t->reciprocalFullArea;
                gamma = 1 - alpha - theta;

                depth = alpha * t->An.z + theta * t->Bn.z + gamma * t->Cn.z;

                const int bufferIndex = ( y * engineSetup->screenWidth ) + x;

                if ( depth < engineBuffers->getDepthBuffer( bufferIndex )) {
                    affine_uv = 1 / ( alpha * (t->persp_correct_Az) + theta * (t->persp_correct_Bz) + gamma * (t->persp_correct_Cz) );
                    texu   = ( (alpha * t->tex_u1_Ac_z)   + (theta * t->tex_u2_Bc_z)   + (gamma * t->tex_u3_Cc_z) )   * affine_uv;
                    texv   = ( (alpha * t->tex_v1_Ac_z)   + (theta * t->tex_v2_Bc_z)   + (gamma * t->tex_v3_Cc_z) )   * affine_uv;

                    lightu = ( (alpha * t->light_u1_Ac_z) + (theta * t->light_u2_Bc_z) + (gamma * t->light_u3_Cc_z) ) * affine_uv;
                    lightv = ( (alpha * t->light_v1_Ac_z) + (theta * t->light_v2_Bc_z) + (gamma * t->light_v3_Cc_z) ) * affine_uv;

                    if (! ((x < minTileX || x > maxTileX) || (y < minTileY || y > maxTileY )) ) {
                        this->processPixel(
                                t,
                                bufferIndex,
                                x, y,
                                alpha, theta, gamma,
                                depth,
                                texu, texv,
                                lightu, lightv
                        );
                    }
                }
            }

            // edge function increments
            w0 += A12;
            w1 += A20;
            w2 += A01;
        }

        w0_row += B12;
        w1_row += B20;
        w2_row += B01;
    }
}

void Brakeza3D::processTriangle(Triangle *t)
{
    EngineSetup *setup = EngineSetup::getInstance();

    // degradate
    if (t->getTexture() != NULL && setup->TRIANGLE_MODE_TEXTURIZED) {
        triangleRasterizer(t);
    }

    // texture
    if (setup->TRIANGLE_MODE_COLOR_SOLID) {
        triangleRasterizer(t);
    }

    // wireframe
    if (setup->TRIANGLE_MODE_WIREFRAME || (t->parent->isDecal() && setup->DRAW_DECAL_WIREFRAMES)) {
        drawWireframe(t);
    }

    // Pixels
    if (setup->TRIANGLE_MODE_PIXELS ) {
        Drawable::drawVertex(t->Co, getCamera(), Color::red());
        Drawable::drawVertex(t->Bo, getCamera(), Color::green());
        Drawable::drawVertex(t->Co, getCamera(), Color::blue());
    }

    t->drawed = true;
}


void Brakeza3D::drawWireframe(Triangle *t)
{
    Drawable::drawLine2D(Line2D(t->As.x, t->As.y, t->Bs.x, t->Bs.y), Color::red());
    Drawable::drawLine2D(Line2D(t->Bs.x, t->Bs.y, t->Cs.x, t->Cs.y), Color::green());
    Drawable::drawLine2D(Line2D(t->Cs.x, t->Cs.y, t->As.x, t->As.y), Color::blue());
}

void Brakeza3D::drawWireframeColor(Triangle *t, Uint32 c)
{
    Drawable::drawLine2D(Line2D(t->As.x, t->As.y, t->Bs.x, t->Bs.y), c);
    Drawable::drawLine2D(Line2D(t->Bs.x, t->Bs.y, t->Cs.x, t->Cs.y), c);
    Drawable::drawLine2D(Line2D(t->Cs.x, t->Cs.y, t->As.x, t->As.y), c);
}


void Brakeza3D::drawFrameTriangles(std::vector<Triangle*> &visibleTriangles)
{
    std::vector<Triangle *>::iterator it;
    for ( it = visibleTriangles.begin(); it != visibleTriangles.end(); it++) {
        Triangle *triangle = *(it);
        processTriangle( triangle );
    }
}

void Brakeza3D::drawSplash(SDL_Surface *surface)
{
    EngineSetup::getInstance()->FADEIN = true;
    SDL_BlitSurface(surface, NULL, Brakeza3D::get()->screenSurface, NULL);
}
