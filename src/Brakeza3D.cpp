
#include "../headers/Brakeza3D.h"
#include "../headers/Render/Drawable.h"
#include "../headers/Render/Maths.h"
#include "../headers/Render/EngineBuffers.h"

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

    engineTimer.start();
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
            SDL_SetSurfaceBlendMode(Brakeza3D::get()->screenSurface, SDL_BLENDMODE_NONE);
            renderer = SDL_CreateRenderer(Brakeza3D::get()->window, -1, SDL_RENDERER_ACCELERATED );

            SDL_GL_SetSwapInterval(1); // Enable vsync

            ImGui_ImplSDL2_InitForOpenGL(Brakeza3D::get()->window, gl_context);
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

            SDL_GL_SwapWindow(Brakeza3D::get()->window);
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
        std::string pathFont = "../fonts/doom.ttf";
        Logging::getInstance()->Log("Loading FONT: " + pathFont, "INFO");

        font = TTF_OpenFont( pathFont.c_str(), EngineSetup::getInstance()->TEXT_3D_SIZE );
        if(!font) {
            Logging::getInstance()->Log(TTF_GetError(), "INFO");
        }
    }
}

bool Brakeza3D::initSound()
{
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
    }

    Mix_Volume(-1, (int) EngineSetup::getInstance()->SOUND_VOLUME);
    Mix_VolumeMusic((int) EngineSetup::getInstance()->SOUND_VOLUME);

    EngineBuffers::getInstance()->loadWAVs();
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
    sceneObjects.push_back(obj);
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
}


float Brakeza3D::getDeltaTime()
{
    return this->deltaTime/1000;
}


void Brakeza3D::processFPS()
{
    if (!EngineSetup::getInstance()->DRAW_FPS) return;

    ++fpsFrameCounter;
    if (Brakeza3D::get()->frameTime > 1000) {
        fps = fpsFrameCounter;
        frameTime = 0;
        fpsFrameCounter = 0;
    }

    //int renderer_w, renderer_h;
    //SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);
    //Tools::writeText(Engine::renderer, Engine::font, renderer_w-150, 14, Color::yellow(), std::to_string(fps) +"fps");
}

void Brakeza3D::drawMenuScreen()
{
    //this->waterShader();
    Brakeza3D::get()->getMenuManager()->drawOptions(Brakeza3D::get()->screenSurface);
    Drawable::drawFireShader();
}


void Brakeza3D::waterShader()
{
    //BSP LAVA EFFECT
    float LAVA_CLOSENESS = 2.35;
    float LAVA_INTENSITY = 0.45;
    float LAVA_SPEED = 2.55;
    float LAVA_SCALE = 2.35;

    // Default config is used in menu mode
    float intensity_r = 1;
    float intensity_g = 1;
    float intensity_b = 1;

    //water = -3 |mud = -4 | lava = -5
    switch(Brakeza3D::get()->getBSP()->currentLeaf->type) {
        default:
        case -3:
            break;
        case -4:
            intensity_r = 0.5;
            intensity_g = 1;
            intensity_b = 0.5;
            break;
        case -5:
            intensity_r = 1;
            intensity_g = 0.5;
            intensity_b = 0.5;
            break;
    }

    Uint32 *newVideoBuffer = new Uint32[EngineBuffers::getInstance()->sizeBuffers];

    for (int y = 0; y < EngineSetup::getInstance()->screenHeight; y++) {
        for (int x = 0; x < EngineSetup::getInstance()->screenWidth; x++) {
            Uint32 currentPixelColor = EngineBuffers::getInstance()->getVideoBuffer(x, y);

            int r_light = (int) (Tools::getRedValueFromColor(currentPixelColor)   * intensity_r);
            int g_light = (int) (Tools::getGreenValueFromColor(currentPixelColor) * intensity_g);
            int b_light = (int) (Tools::getBlueValueFromColor(currentPixelColor)  * intensity_b);

            currentPixelColor = Tools::createRGB( r_light, g_light, b_light );

            float cache1 = x / LAVA_CLOSENESS;
            float cache2 = y / LAVA_CLOSENESS;

            int nx = (cache1 + LAVA_INTENSITY * sin(LAVA_SPEED * Brakeza3D::get()->executionTime + cache2) ) * LAVA_SCALE;
            int ny = (cache2 + LAVA_INTENSITY * sin(LAVA_SPEED * Brakeza3D::get()->executionTime + cache1) ) * LAVA_SCALE;

            int bufferIndex = nx + ny * EngineSetup::getInstance()->screenWidth;

            if(Tools::isPixelInWindow(nx, ny)) {
                newVideoBuffer[bufferIndex] = currentPixelColor;
            }
        }
    }

    memcpy (&EngineBuffers::getInstance()->videoBuffer, &newVideoBuffer, sizeof(newVideoBuffer));
}


void Brakeza3D::initBSP(const char *bspFilename, std::vector<Triangle*> *frameTriangles)
{
    const char *paletteFilename = "palette.lmp";
    Logging::getInstance()->Log("Loading BSP Quake map: " + std::string(bspFilename));

    EngineSetup::getInstance()->BULLET_STEP_SIMULATION = true;

    Brakeza3D::get()->getBSP()->Initialize(bspFilename, paletteFilename, frameTriangles);

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

    Brakeza3D::get()->getCamera()->setPosition(bspOriginalPosition);

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

    for (int y = t->minY ; y < t->maxY ; y++) {
        int w0 = w0_row;
        int w1 = w1_row;
        int w2 = w2_row;

        for (int x = t->minX ; x < t->maxX ; x++) {

            if ((w0 | w1 | w2) > 0) {
                if (Tools::isPixelInWindow(x, y))  {
                    alpha = w0 * t->reciprocalFullArea;
                    theta = w1 * t->reciprocalFullArea;
                    gamma = 1 - alpha - theta;

                    depth = alpha * (t->An.z) + theta * (t->Bn.z) + gamma * (t->Cn.z);

                    int bufferIndex = ( y * EngineSetup::getInstance()->screenWidth ) + x;

                    if (t->parent->isDecal()) {
                        depth-=1;
                    }
                    if (EngineSetup::getInstance()->TRIANGLE_RENDER_DEPTH_BUFFER && depth < EngineBuffers::getInstance()->getDepthBuffer( bufferIndex )) {
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
    if (EngineSetup::getInstance()->TRIANGLE_MODE_COLOR_SOLID) {
        pixelColor = (Uint32) Tools::createRGB(w0 * 255, w1 * 255, w2 * 255);
    }

    // Texture
    if (EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED && t->getTexture() != NULL) {
        if (t->getTexture()->liquid && EngineSetup::getInstance()->TRIANGLE_TEXTURES_ANIMATED ) {
            float cache1 = texu / EngineSetup::getInstance()->LAVA_CLOSENESS;
            float cache2 = texv / EngineSetup::getInstance()->LAVA_CLOSENESS;
            texu = (cache1 + EngineSetup::getInstance()->LAVA_INTENSITY * sin(EngineSetup::getInstance()->LAVA_SPEED *
                                                                              Brakeza3D::get()->executionTime + cache2) ) * EngineSetup::getInstance()->LAVA_SCALE;
            texv = (cache2 + EngineSetup::getInstance()->LAVA_INTENSITY * sin(EngineSetup::getInstance()->LAVA_SPEED *
                                                                              Brakeza3D::get()->executionTime + cache1) ) * EngineSetup::getInstance()->LAVA_SCALE;
        }

        if ( t->parent->isDecal() ) {
            if ((texu < 0 || texu > 1) || (texv < 0 || texv > 1) ) {
                return;
            }
        }

        pixelColor = t->processPixelTexture(texu, texv);

        Uint8 red, green, blue, alpha;
        SDL_GetRGBA(pixelColor, t->texture->getSurface(t->lod)->format, &red, &green, &blue, &alpha);

        if (alpha == 0) {
            return;
        } else {
            Uint32 existingColor = EngineBuffers::getInstance()->getVideoBuffer(x, y);
            pixelColor = Maths::alphaBlend(existingColor, pixelColor, alpha);
        }

        if (!t->parent->isDecal() && t->getLightmap()->isLightMapped() && EngineSetup::getInstance()->ENABLE_LIGHTMAPPING) {
            pixelColor = t->processPixelLightmap(pixelColor, lightu, lightv);
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

    EngineBuffers::getInstance()->setDepthBuffer(bufferIndex, z);
    EngineBuffers::getInstance()->setVideoBuffer(bufferIndex, pixelColor);
}


void Brakeza3D::softwareRasterizerForTile(Triangle *t, int minTileX, int minTileY, int maxTileX, int maxTileY)
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

                const int bufferIndex = ( y * EngineSetup::getInstance()->screenWidth ) + x;

                if (EngineSetup::getInstance()->TRIANGLE_RENDER_DEPTH_BUFFER && depth < EngineBuffers::getInstance()->getDepthBuffer( bufferIndex )) {
                    affine_uv = 1 / ( alpha * (t->persp_correct_Az) + theta * (t->persp_correct_Bz) + gamma * (t->persp_correct_Cz) );
                    texu   = ( alpha * (t->tex_u1_Ac_z)   + theta * (t->tex_u2_Bc_z)   + gamma * (t->tex_u3_Cc_z) )   * affine_uv;
                    texv   = ( alpha * (t->tex_v1_Ac_z)   + theta * (t->tex_v2_Bc_z)   + gamma * (t->tex_v3_Cc_z) )   * affine_uv;

                    lightu = ( alpha * (t->light_u1_Ac_z) + theta * (t->light_u2_Bc_z) + gamma * (t->light_u3_Cc_z) ) * affine_uv;
                    lightv = ( alpha * (t->light_v1_Ac_z) + theta * (t->light_v2_Bc_z) + gamma * (t->light_v3_Cc_z) ) * affine_uv;

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