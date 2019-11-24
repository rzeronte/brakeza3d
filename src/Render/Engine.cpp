#include "../../headers/Render/Engine.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Objects/BSPEntity3D.h"
#include <chrono>
#include <iostream>
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "LinearMath/btTransform.h"
#include "../../headers/Brakeza3D.h"
#include "../../headers/Render/Maths.h"
#include "../../headers/Render/EngineBuffers.h"
#include <OpenCL/opencl.h>
#include <SDL_image.h>

#define MAX_SOURCE_SIZE (0x100000)

Engine::Engine()
{
    IMGUI_CHECKVERSION();
    this->initOpenCL();
    Brakeza3D::get()->initTiles();
    this->initCollisionManager();
}

void Engine::initOpenCL()
{
    platform_id = NULL;
    device_cpu_id = NULL;

    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms) ;
    if (ret != CL_SUCCESS) {
        printf("Unable to get platform_id");
    }

    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, &device_cpu_id, &ret_num_devices);
    if (ret != CL_SUCCESS) {
        printf("Unable to get device_cpu_id");
    }

    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, &device_gpu_id, &ret_num_devices);
    if (ret != CL_SUCCESS) {
        printf("Unable to get device_gpu_id");
    }

    cl_context_properties properties[3];
    properties[0] = CL_CONTEXT_PLATFORM;
    properties[1] = (cl_context_properties) platform_id;
    properties[2] = 0;

    // Create an OpenCL context
    contextCPU = clCreateContext(properties, 1, &device_cpu_id, NULL, NULL, &ret);
    contextGPU = clCreateContext(properties, 1, &device_gpu_id, NULL, NULL, &ret);

    command_queue_rasterizer = clCreateCommandQueue(contextCPU, device_cpu_id, NULL, &ret);
    command_queue_transforms = clCreateCommandQueue(contextGPU, device_gpu_id, CL_QUEUE_PROFILING_ENABLE, &ret);

    // Load the kernel source code into the array source_str
    size_t source_size;
    char *source_str = Tools::readFile("../opencl.cl", source_size);

    // Create a program from the kernel source
    programCPU = clCreateProgramWithSource(contextCPU, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    programGPU = clCreateProgramWithSource(contextGPU, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

    // Build the program
    clBuildProgram(programCPU, 1, &device_cpu_id, NULL, NULL, NULL);
    clBuildProgram(programGPU, 1, &device_gpu_id, NULL, NULL, NULL);

    // Create the OpenCL kernel
    processAllTriangles       = clCreateKernel(programCPU, "rasterizerFrameTrianglesKernel", &ret);
    processTileTriangles      = clCreateKernel(programCPU, "rasterizerFrameTileTrianglesKernel", &ret);
    processTransformTriangles = clCreateKernel(programGPU, "transformTrianglesKernel", &ret);

    opencl_buffer_depth = clCreateBuffer(contextCPU, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, EngineBuffers::getInstance()->sizeBuffers * sizeof(float), EngineBuffers::getInstance()->depthBuffer, &ret);
    opencl_buffer_video = clCreateBuffer(contextCPU, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, EngineBuffers::getInstance()->sizeBuffers * sizeof(unsigned int), EngineBuffers::getInstance()->videoBuffer, &ret);

    OpenCLInfo();
}

void Engine::handleOpenCLTriangles()
{
    int numTriangles = EngineBuffers::getInstance()->numOCLTriangles;

    opencl_buffer_triangles = clCreateBuffer(contextCPU, CL_MEM_READ_ONLY, numTriangles * sizeof(OCLTriangle), NULL, &ret);
    clEnqueueWriteBuffer(command_queue_rasterizer, opencl_buffer_triangles, CL_TRUE, 0, numTriangles * sizeof(OCLTriangle), EngineBuffers::getInstance()->OCLTrianglesBuffer, 0, NULL, NULL);

    clSetKernelArg(processAllTriangles, 0, sizeof(cl_mem), (void *)&opencl_buffer_triangles);
    clSetKernelArg(processAllTriangles, 1, sizeof(int), &EngineSetup::getInstance()->screenWidth);
    clSetKernelArg(processAllTriangles, 2, sizeof(cl_mem), (void *)&opencl_buffer_video);
    clSetKernelArg(processAllTriangles, 3, sizeof(cl_mem), (void *)&opencl_buffer_depth);

    size_t global_item_size = numTriangles; // Process the entire lists
    size_t local_item_size = 1;          // Divide work items into groups of 64

    ret = clEnqueueNDRangeKernel(command_queue_rasterizer, processAllTriangles, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

    if (ret != CL_SUCCESS) {
        Logging::getInstance()->getInstance()->Log( "Error processAllTriangles: " + std::to_string(ret) );

        char buffer[1024];
        clGetProgramBuildInfo(programCPU, device_cpu_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
        if (strlen(buffer) > 0 ) {
            Logging::getInstance()->getInstance()->Log( buffer );
        }
    }
    clFinish(command_queue_rasterizer);
    EngineBuffers::getInstance()->numOCLTriangles = 0;
}

void Engine::initCollisionManager()
{
    Brakeza3D::get()->getCollisionManager()->setBspMap( Brakeza3D::get()->getBSP() );
    Brakeza3D::get()->getCollisionManager()->setWeaponManager( Brakeza3D::get()->getWeaponsManager() );
    Brakeza3D::get()->getCollisionManager()->setCamera( Brakeza3D::get()->getCamera() );
    Brakeza3D::get()->getCollisionManager()->setGameObjects( &Brakeza3D::get()->getSceneObjects() );
    Brakeza3D::get()->getCollisionManager()->initBulletSystem();
    Brakeza3D::get()->getCollisionManager()->setVisibleTriangles(visibleTriangles);
}

void Engine::OpenCLInfo()
{
    int i, j;
    char* value;
    size_t valueSize;
    cl_uint platformCount;
    cl_platform_id* platforms;
    cl_uint deviceCount;
    cl_device_id* devices;
    cl_uint maxComputeUnits;

    // get all platforms
    clGetPlatformIDs(0, NULL, &platformCount);
    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
    clGetPlatformIDs(platformCount, platforms, NULL);

    for (i = 0; i < platformCount; i++) {

        // get all devices
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
        devices = (cl_device_id*) malloc(sizeof(cl_device_id) * deviceCount);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);

        // for each device print critical attributes
        for (j = 0; j < deviceCount; j++) {

            // print device name
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
            printf("%d. Device: %s\n", j+1, value);
            free(value);

            // print hardware device version
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
            printf(" %d.%d Hardware version: %s\n", j+1, 1, value);
            free(value);

            // print software driver version
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
            printf(" %d.%d Software version: %s\n", j+1, 2, value);
            free(value);

            // print c version supported by compiler for device
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
            printf(" %d.%d OpenCL C version: %s\n", j+1, 3, value);
            free(value);

            // print parallel compute units
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,
                            sizeof(maxComputeUnits), &maxComputeUnits, NULL);
            printf(" %d.%d Parallel compute units: %d\n", j+1, 4, maxComputeUnits);
        }
        free(devices);
    }

    free(platforms);
}

void Engine::updateGUI()
{
    ImGui::NewFrame();

    //bool open = true;
    //ImGui::ShowDemoWindow(&open);

    Brakeza3D::get()->getGUIManager()->draw(
            Brakeza3D::get()->getDeltaTime(),
            finish,
            Brakeza3D::get()->getSceneObjects(),
            Brakeza3D::get()->getLightPoints(),
            Brakeza3D::get()->getCamera(),
            Brakeza3D::get()->tiles, Brakeza3D::get()->tilesWidth,
            visibleTriangles.size(),
            Brakeza3D::get()->getWeaponsManager()
    );

    ImGui::Render();
}

void Engine::updateWindow()
{
    Brakeza3D::get()->updateFPS();

    EngineBuffers::getInstance()->flipVideoBuffer(Brakeza3D::get()->screenSurface );

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(Brakeza3D::get()->window);

    updateGUI();

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(Brakeza3D::get()->window);

    Brakeza3D::get()->screenTexture = SDL_CreateTextureFromSurface(Brakeza3D::get()->renderer, Brakeza3D::get()->screenSurface);
    SDL_RenderCopy(Brakeza3D::get()->renderer, Brakeza3D::get()->screenTexture, NULL, NULL);

    SDL_DestroyTexture(Brakeza3D::get()->screenTexture);
}

void Engine::onStart()
{
    Brakeza3D::get()->getCamera()->setPosition(EngineSetup::getInstance()->CameraPosition );

    Brakeza3D::get()->getCamera()->velocity.vertex1 = *Brakeza3D::get()->getCamera()->getPosition();
    Brakeza3D::get()->getCamera()->velocity.vertex2 = *Brakeza3D::get()->getCamera()->getPosition();
}

void Engine::preUpdate()
{
    // Posición inicial del vector velocidad que llevará la cámara
    Camera3D *cam = Brakeza3D::get()->getCamera();

    cam->velocity.vertex1 = *cam->getPosition();
    cam->velocity.vertex2 = *cam->getPosition();

    // Determinamos VPS
    if (Brakeza3D::get()->getBSP()->isLoaded()) {

        int leafType = NULL;

        if (Brakeza3D::get()->getBSP()->currentLeaf != NULL) {
            leafType = Brakeza3D::get()->getBSP()->currentLeaf->type;
        }

        bspleaf_t *leaf = Brakeza3D::get()->getBSP()->FindLeaf( cam );
        Brakeza3D::get()->getBSP()->setVisibleSet(leaf);

        if (leafType != Brakeza3D::get()->getBSP()->currentLeaf->type) {
            if (Brakeza3D::get()->getBSP()->isCurrentLeafLiquid()) {
                cam->kinematicController->setFallSpeed(5);
            } else {
                cam->kinematicController->setFallSpeed(256);
            }
        }
    }
}

void Engine::onUpdate()
{
    // update collider forces
    Brakeza3D::get()->getCamera()->UpdateVelocity();

    // update deltaTime
    Brakeza3D::get()->updateTimer();

    // step simulation
    Vertex3D finalVelocity = Brakeza3D::get()->getCollisionManager()->stepSimulation( );

    this->resolveCollisions();

    // update camera position/rotation/frustum
    Brakeza3D::get()->getCamera()->setPosition(finalVelocity);
    Brakeza3D::get()->getCamera()->UpdateRotation();
    Brakeza3D::get()->getCamera()->UpdateFrustum();

    // Clear buffers
    EngineBuffers::getInstance()->clearDepthBuffer();
    EngineBuffers::getInstance()->clearVideoBuffer();

    /*if (EngineSetup::getInstance()->ENABLE_SHADOW_CASTING) {
        // Clear every light's shadow mapping
        this->clearLightPointsShadowMappings();

        // Fill ShadowMapping FOR every object (mesh)
        this->objects3DShadowMapping();
    }*/

    // recollect triangles
    this->getQuakeMapTriangles();
    this->getMesh3DTriangles();
    this->getSpritesTriangles();

    this->hiddenSurfaceRemoval();

    if (EngineSetup::getInstance()->BASED_TILE_RENDER) {
        Brakeza3D::get()->handleTrianglesToTiles(visibleTriangles);

        Brakeza3D::get()->drawTilesTriangles(&visibleTriangles);

        if (EngineSetup::getInstance()->DRAW_TILES_GRID) {
            Brakeza3D::get()->drawTilesGrid();
        }
    } else {
        if (!EngineSetup::getInstance()->RASTERIZER_OPENCL) {
            Brakeza3D::get()->drawFrameTriangles(visibleTriangles);
        } else {
            this->handleOpenCLTriangles();
        }
    }

    if (EngineSetup::getInstance()->BULLET_DEBUG_MODE) {
        Brakeza3D::get()->getCollisionManager()->getDynamicsWorld()->debugDrawWorld();
    }

    if (EngineSetup::getInstance()->RENDER_OBJECTS_AXIS) {
        this->drawSceneObjectsAxis();
    }

    if (EngineSetup::getInstance()->DRAW_FRUSTUM) {
        Drawable::drawFrustum(Brakeza3D::get()->getCamera()->frustum, Brakeza3D::get()->getCamera(), true, true, true);
    }

    if (EngineSetup::getInstance()->RENDER_MAIN_AXIS) {
        Drawable::drawMainAxis(Brakeza3D::get()->getCamera() );
    }

    if (Brakeza3D::get()->getBSP()->isLoaded() && Brakeza3D::get()->getBSP()->isCurrentLeafLiquid() && !EngineSetup::getInstance()->MENU_ACTIVE) {
        Brakeza3D::get()->waterShader();
    }
}

void Engine::resolveCollisions()
{

}

/*void Engine::clearLightPointsShadowMappings()
{
    for (int i = 0; i < Brakeza3D::get()->getLightPoints().size(); i++) {
        if (!Brakeza3D::get()->getLightPoints()[i]->isEnabled()) {
            continue;
        }

        Brakeza3D::get()->getLightPoints()[i]->clearShadowMappingBuffer();
    }
}*/

/*void Engine::objects3DShadowMapping()
{
    for (int i = 0; i < Brakeza3D::get()->getSceneObjects().size() ; i++) {
        if (!Brakeza3D::get()->getSceneObjects()[i]->isEnabled()) {
            continue;
        }

        Mesh3D *oMesh = dynamic_cast<Mesh3D*> (Brakeza3D::get()->getSceneObjects()[i]);
        if (oMesh != NULL) {
            for (int j = 0; j < Brakeza3D::get()->getLightPoints().size(); j++) {
                if (oMesh->isShadowCaster()) {
                    oMesh->shadowMapping(Brakeza3D::get()->getLightPoints()[j]);
                }
            }
        }
    }
}*/

void Engine::getQuakeMapTriangles()
{
    if ( Brakeza3D::get()->getBSP()->isLoaded() ) {
        Brakeza3D::get()->getBSP()->DrawVisibleLeaf(Brakeza3D::get()->getCamera() );
        if (EngineSetup::getInstance()->DRAW_BSP_HULLS) {
            Brakeza3D::get()->getBSP()->DrawHulls(Brakeza3D::get()->getCamera()  );
        }
    }
}

void Engine::getMesh3DTriangles()
{
    // draw meshes
    for (int i = 0; i < Brakeza3D::get()->getSceneObjects().size(); i++) {
        Mesh3D *oMesh = dynamic_cast<Mesh3D*> (Brakeza3D::get()->getSceneObjects()[i]);
        if (oMesh != NULL) {
            if (oMesh->isEnabled()) {
                oMesh->draw( &this->frameTriangles) ;
                if (EngineSetup::getInstance()->TEXT_ON_OBJECT3D) {
                    Tools::writeText3D(Brakeza3D::get()->renderer, Brakeza3D::get()->getCamera(), Brakeza3D::get()->fontDefault, *oMesh->getPosition(), EngineSetup::getInstance()->TEXT_3D_COLOR, oMesh->getLabel());
                }
            }
            if (EngineSetup::getInstance()->DRAW_DECAL_WIREFRAMES) {
                Decal *oDecal = dynamic_cast<Decal*> (oMesh);
                if (oDecal != NULL) {
                    oDecal->cube->draw( &this->frameTriangles );
                }
            }
        }
    }
}

void Engine::getSpritesTriangles()
{
    if (!EngineSetup::getInstance()->DRAW_SPRITES) return;

    std::vector<Object3D *>::iterator it;
    for ( it = Brakeza3D::get()->getSceneObjects().begin(); it != Brakeza3D::get()->getSceneObjects().end(); ) {
        Object3D *object = *(it);

        // Check for delete
        if (object->isRemoved()) {
            Brakeza3D::get()->getSceneObjects().erase(it);
            continue;
        } else {
            it++;
        }

        // Sprite Directional 3D
        SpriteDirectional3D *oSpriteDirectional = dynamic_cast<SpriteDirectional3D*> (object);
        if (oSpriteDirectional != NULL) {

            if (!oSpriteDirectional->isEnabled()) {
                continue;
            }

            if (!Brakeza3D::get()->getCamera()->frustum->isPointInFrustum(*oSpriteDirectional->getPosition())) {
                continue;
            }

            oSpriteDirectional->updateTrianglesCoordinates( Brakeza3D::get()->getCamera() );
            Drawable::drawBillboard(oSpriteDirectional->getBillboard(), &this->frameTriangles);

            if (EngineSetup::getInstance()->TEXT_ON_OBJECT3D) {
                Tools::writeText3D(Brakeza3D::get()->renderer, Brakeza3D::get()->getCamera(), Brakeza3D::get()->fontDefault, *oSpriteDirectional->getPosition(), EngineSetup::getInstance()->TEXT_3D_COLOR, oSpriteDirectional->getLabel());
            }
        }

        // Sprite 3D
        Sprite3D *oSprite = dynamic_cast<Sprite3D*> (object);
        if (oSprite != NULL) {
            if (!oSprite->isEnabled()) {
                continue;
            }

            oSprite->updateTrianglesCoordinatesAndTexture(Brakeza3D::get()->getCamera() );
            Drawable::drawBillboard(oSprite->getBillboard(), &frameTriangles);

            if (EngineSetup::getInstance()->TEXT_ON_OBJECT3D) {
                Tools::writeText3D(Brakeza3D::get()->renderer, Brakeza3D::get()->getCamera(), Brakeza3D::get()->fontDefault, *oSprite->getPosition(), EngineSetup::getInstance()->TEXT_3D_COLOR, oSprite->getLabel());
            }
        }
    }
}

void Engine::hiddenSurfaceRemoval()
{
    visibleTriangles.clear();

    for (int i = 0; i < this->frameTriangles.size() ; i++) {

        // Si la transformación es mediante openCL ya están todas hechas
        // pero si vamos por software actualizamos ObjectSpace y su normal
        // ya que es el mínimo necesario para el clipping y el faceculling

        this->frameTriangles[i]->updateObjectSpace();
        this->frameTriangles[i]->updateNormal();

        // back face culling (needs objectSpace)
        if (EngineSetup::getInstance()->TRIANGLE_BACK_FACECULLING) {
            if (this->frameTriangles[i]->isBackFaceCulling(Brakeza3D::get()->getCamera()->getPosition() )) {
                continue;
            }
        }

        // Clipping (needs objectSpace)
        bool needClipping = false;
        if (this->frameTriangles[i]->testForClipping(
                Brakeza3D::get()->getCamera()->frustum->planes,
                EngineSetup::getInstance()->LEFT_PLANE,
                EngineSetup::getInstance()->BOTTOM_PLANE
        )) {
            needClipping = true;
        }

        if (needClipping) {
            this->frameTriangles[i]->clipping(
                    Brakeza3D::get()->getCamera(),
                    Brakeza3D::get()->getCamera()->frustum->planes,
                    EngineSetup::getInstance()->LEFT_PLANE,
                    EngineSetup::getInstance()->BOTTOM_PLANE,
                    frameTriangles[i]->parent,
                    visibleTriangles,
                    frameTriangles[i]->isBSP
            );
            continue;
        }

        // Frustum Culling (needs objectSpace)
        if (!Brakeza3D::get()->getCamera()->frustum->isPointInFrustum(this->frameTriangles[i]->Ao) &&
            !Brakeza3D::get()->getCamera()->frustum->isPointInFrustum(this->frameTriangles[i]->Bo) &&
            !Brakeza3D::get()->getCamera()->frustum->isPointInFrustum(this->frameTriangles[i]->Co)
        ) {
            continue;
        }

        // Triangle precached data
        // Estas operaciones las hacemos después de descartar triángulos
        // para optimización en el rasterizador por software
        this->frameTriangles[i]->updateCameraSpace(Brakeza3D::get()->getCamera() );
        this->frameTriangles[i]->updateNDCSpace(Brakeza3D::get()->getCamera() );
        this->frameTriangles[i]->updateScreenSpace(Brakeza3D::get()->getCamera() );
        this->frameTriangles[i]->updateBoundingBox();

        if (!EngineSetup::getInstance()->RASTERIZER_OPENCL) {
            this->frameTriangles[i]->updateFullArea();
            this->frameTriangles[i]->updateUVCache();
        } else {
            EngineBuffers::getInstance()->addOCLTriangle(this->frameTriangles[i]->getOpenCL());
        }

        this->frameTriangles[i]->drawed = false;
        this->visibleTriangles.push_back(this->frameTriangles[i]);
    }

    if (EngineSetup::getInstance()->DEBUG_RENDER_INFO) {
        Logging::getInstance()->Log("[DEBUG_RENDER_INFO] frameTriangles: " + std::to_string(frameTriangles.size()) + ", numVisibleTriangles: " + std::to_string(visibleTriangles.size()));
    }

    frameTriangles.clear();
}

void Engine::onEnd()
{
    Brakeza3D::get()->getTimer()->stop();
}

void Engine::Close()
{
    TTF_CloseFont( Brakeza3D::get()->fontDefault );
    TTF_CloseFont( Brakeza3D::get()->fontSmall );
    TTF_CloseFont( Brakeza3D::get()->fontBig );
    TTF_CloseFont( Brakeza3D::get()->fontMedium );

    SDL_DestroyWindow(Brakeza3D::get()->window );
    SDL_Quit();

    printf("\r\nBrakeza3D exit, good bye ;)");
}


void Engine::drawSceneObjectsAxis()
{
    // draw meshes
    for (int i = 0; i < Brakeza3D::get()->getSceneObjects().size(); i++) {
        if (Brakeza3D::get()->getSceneObjects()[i]->isEnabled()) {
            Drawable::drawObject3DAxis(Brakeza3D::get()->getSceneObjects()[i], Brakeza3D::get()->getCamera(), true, true, true);
        }
    }
}