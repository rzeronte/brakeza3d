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
#include "../../headers/Objects/Mesh3DGhost.h"
#include <OpenCL/opencl.h>

#define MAX_SOURCE_SIZE (0x100000)


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

    this->frameTriangles = new Triangle[250000];
    this->visibleTriangles = new Triangle[250000];

    IMGUI_CHECKVERSION();
    imgui_context = ImGui::CreateContext();
    fps = 0;

    this->initPhysics();
    this->initOpenCL();

    this->initTiles();
}

bool Engine::initWindow()
{
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
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
            screenSurface = SDL_CreateRGBSurface(0, EngineSetup::getInstance()->SCREEN_WIDTH, EngineSetup::getInstance()->SCREEN_HEIGHT, 32, 0, 0, 0, 0);
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
        std::string pathFont = "../fonts/alterebro-pixel-font.ttf";
        Logging::getInstance()->Log("Loading FONT: " + pathFont, "INFO");

        font = TTF_OpenFont( pathFont.c_str(), EngineSetup::getInstance()->TEXT_3D_SIZE );
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

    btVector3 worldMin(-500,-500,-500);
    btVector3 worldMax(500,500,500);
    btAxisSweep3* sweepBP = new btAxisSweep3(worldMin,worldMax);
    this->overlappingPairCache = sweepBP;

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    this->solver = new btSequentialImpulseConstraintSolver;

    /// Debug drawer
    this->debugDraw = new PhysicsDebugDraw(this->camera);

    this->dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    this->dynamicsWorld->setGravity(btVector3(0, EngineSetup::getInstance()->gravity.y, 0));

    this->overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

    this->camera->kinematicController->setGravity(dynamicsWorld->getGravity() );

    this->dynamicsWorld->addCollisionObject(this->camera->m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter);
    this->dynamicsWorld->addAction(this->camera->kinematicController);

    this->dynamicsWorld->setDebugDrawer(debugDraw);
    this->dynamicsWorld->getDebugDrawer()->setDebugMode(PhysicsDebugDraw::DBG_DrawWireframe);

    /// trigger for detect collision between player and other triggers
    this->triggerCamera = new Mesh3DGhost();
    triggerCamera->setLabel("triggerCamera");
    triggerCamera->setEnabled(true);
    triggerCamera->setLightPoints(Engine::lightPoints, Engine::numberLightPoints);
    triggerCamera->setPosition(*camera->getPosition());
    triggerCamera->getGhostObject()->setCollisionShape(camera->kinematicController->getGhostObject()->getCollisionShape());
    triggerCamera->getGhostObject()->setUserPointer(triggerCamera);
    dynamicsWorld->addCollisionObject(triggerCamera->getGhostObject(), collisionGroups::CameraTrigger, collisionGroups::DefaultFilter|collisionGroups::BSPHullTrigger);
}

void Engine::initOpenCL()
{

    size_t valueSize;
    char* value;
    cl_context_properties properties[3];

    platform_id = NULL;
    device_id = NULL;

    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms) ;
    if (ret != CL_SUCCESS) {
        printf("Unable to get platform_id");
    }

    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
    if (ret != CL_SUCCESS) {
        printf("Unable to get device_id");
    }

    properties[0]= CL_CONTEXT_PLATFORM;
    properties[1]= (cl_context_properties) platform_id;
    properties[2]= 0;

    // Create an OpenCL context
    context = clCreateContext(properties, 1, &device_id, NULL, NULL, &ret);

    command_queue = clCreateCommandQueue(context, device_id, NULL, &ret);

    // Load the kernel source code into the array source_str
    size_t source_size;
    char *source_str = Tools::readFile("../opencl.cl", source_size);

    // Create a program from the kernel source
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

    // Build the program
    clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Create the OpenCL kernel
    kernel = clCreateKernel(program, "handleTiles", &ret);

    opencl_buffer_depth = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, EngineBuffers::getInstance()->sizeBuffers * sizeof(float), EngineBuffers::getInstance()->depthBuffer, &ret);
    opencl_buffer_video = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32), EngineBuffers::getInstance()->videoBuffer, &ret);

    OpenCLInfo();
}

void Engine::processOpenCL()
{
    // Create OCLTriangles
    for (int i = 0 ; i < numVisibleTriangles ; i++) {
        EngineBuffers::getInstance()->addOCLTriangle(visibleTriangles[i].getOpenCL());
    }

    // Create OCLTiles
    OCLTile tilesOCL[numTiles];
    for (int i = 0 ; i < numTiles ; i++) {
        int numTrianglesTile = this->tiles[i].numTriangles;

        tilesOCL[i].draw    = this->tiles[i].draw;
        tilesOCL[i].start_x = this->tiles[i].start_x;
        tilesOCL[i].start_y = this->tiles[i].start_y;
        tilesOCL[i].id      = this->tiles[i].id;
        tilesOCL[i].id_x    = this->tiles[i].id_x;
        tilesOCL[i].id_y    = this->tiles[i].id_y;

        tilesOCL[i].numTriangles = numTrianglesTile;
        for (int j = 0 ; j < numTrianglesTile ; j++) {
            tilesOCL[i].triangleIds[j] = this->tiles[i].triangleIds[j];
        }
    }

    int LIST_SIZE = EngineBuffers::getInstance()->numOCLTriangles;

    opencl_buffer_triangles = clCreateBuffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(OCLTriangle), NULL, &ret);
    clEnqueueWriteBuffer(command_queue, opencl_buffer_triangles, CL_TRUE, 0, LIST_SIZE * sizeof(OCLTriangle), EngineBuffers::getInstance()->OCLTrianglesBuffer, 0, NULL, NULL);

    opencl_buffer_tiles = clCreateBuffer(context, CL_MEM_READ_ONLY, numTiles * sizeof(OCLTile), NULL, &ret);
    clEnqueueWriteBuffer(command_queue, opencl_buffer_tiles, CL_TRUE, 0, numTiles * sizeof(OCLTile), &tilesOCL, 0, NULL, NULL);

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::getInstance()->SCREEN_WIDTH);
    clSetKernelArg(kernel, 1, sizeof(int), &this->sizeTileWidth);
    clSetKernelArg(kernel, 2, sizeof(int), &this->sizeTileHeight);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&opencl_buffer_video);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&opencl_buffer_triangles);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&opencl_buffer_tiles);
    clSetKernelArg(kernel, 6, (sizeTileWidth*sizeTileHeight) * sizeof(float), NULL); // Local ZBuffer for Tile render

    size_t global_item_size = numTiles; // Process the entire lists
    size_t local_item_size = 1;          // Divide work items into groups of 64

    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    if (ret != CL_SUCCESS) {
        Logging::getInstance()->getInstance()->Log( "Error kernel: " + std::to_string(ret) );

        char buffer[1024];
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
        if (strlen(buffer) > 0 ) {
            Logging::getInstance()->getInstance()->Log( buffer );
        }
    }

    clReleaseMemObject(opencl_buffer_triangles);
    clReleaseMemObject(opencl_buffer_tiles);
    EngineBuffers::getInstance()->numOCLTriangles = 0;

}

/*void processOpenCL2()
{
    OCLTile tilesOCL[numTiles];

    for (int i = 0 ; i < numVisibleTriangles ; i++) {
        visibleTriangles[i].updateFullVertexSpaces(camera);
        EngineBuffers::getInstance()->addOCLTriangle(visibleTriangles[i].getOpenCL());
    }

    Logging::getInstance()->getInstance()->Log( "OCLTriangles: " + std::to_string(EngineBuffers::getInstance()->numOCLTriangles) );

    for (int i = 0 ; i < numTiles ; i++) {
        int numTrianglesTile = this->tiles[i].numTriangles;

        tilesOCL[i].draw    = this->tiles[i].draw;
        tilesOCL[i].start_x = this->tiles[i].start_x;
        tilesOCL[i].start_y = this->tiles[i].start_y;
        tilesOCL[i].id      = this->tiles[i].id;
        tilesOCL[i].id_x    = this->tiles[i].id_x;
        tilesOCL[i].id_y    = this->tiles[i].id_y;

        tilesOCL[i].numTriangles = numTrianglesTile;
        for (int j = 0 ; j < numTrianglesTile ; j++) {
            tilesOCL[i].triangleIds[j] = this->tiles[i].triangleIds[j];
        }
    }

    opencl_buffer_tiles = clCreateBuffer(context, CL_MEM_READ_ONLY, numTiles * sizeof(OCLTile), NULL, &ret);
    opencl_buffer_triangles = clCreateBuffer(context, CL_MEM_READ_ONLY, EngineBuffers::getInstance()->numOCLTriangles * sizeof(OCLTriangle), NULL, &ret);

    clEnqueueWriteBuffer(command_queue, opencl_buffer_triangles, CL_TRUE, 0, EngineBuffers::getInstance()->numOCLTriangles * sizeof(OCLTriangle), &EngineBuffers::getInstance()->OCLTrianglesBuffer, 0, NULL, NULL);
    clEnqueueWriteBuffer(command_queue, opencl_buffer_tiles, CL_TRUE, 0, numTiles * sizeof(OCLTile), &tilesOCL, 0, NULL, NULL);

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::getInstance()->SCREEN_WIDTH);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&opencl_buffer_video);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&opencl_buffer_depth);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&opencl_buffer_triangles);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&opencl_buffer_tiles);

    size_t global_item_size = numTiles; // Process the entire lists
    size_t local_item_size = 1;          // Divide work items into groups of 64

    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

    if (ret != CL_SUCCESS) {
        Logging::getInstance()->getInstance()->Log( "Error kernel: " + std::to_string(ret) );

        char buffer[1024];
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
        if (strlen(buffer) > 0 ) {
            Logging::getInstance()->getInstance()->Log( buffer );
        }
    }
    clReleaseMemObject(opencl_buffer_tiles);
    clReleaseMemObject(opencl_buffer_triangles);
    EngineBuffers::getInstance()->numOCLTriangles = 0;

}*/

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
    cl_ulong maxMem;
    cl_ulong maxLocalMem;
    cl_uint maxWorkItemsSize;
    size_t maxWorkGrouptItems;
    size_t maxWorkGroupSize;

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

            // print parallel compute units
            clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE,
                            sizeof(maxMem), &maxMem, NULL);
            printf(" %d.%d CL_DEVICE_GLOBAL_MEM_SIZE: %ld\n", j+1, 4, maxMem);

            // print parallel compute units
            clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_SIZE,
                            sizeof(maxLocalMem), &maxLocalMem, NULL);
            printf(" %d.%d CL_DEVICE_LOCAL_MEM_SIZE: %ld\n", j+1, 4, maxLocalMem);

            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
                            sizeof(maxWorkItemsSize), &maxWorkItemsSize, NULL);
            printf(" %d.%d CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %d\n", j+1, 4, maxWorkItemsSize);

            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE,
                            sizeof(maxWorkGrouptItems), &maxWorkGrouptItems, NULL);
            printf(" %d.%d CL_DEVICE_MAX_WORK_GROUP_SIZE: %d\n", j+1, 4, maxWorkGrouptItems);

            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES,
                            sizeof(maxWorkGroupSize), &maxWorkGroupSize, NULL);
            printf(" %d.%d CL_DEVICE_MAX_WORK_ITEM_SIZES: %d\n", j+1, 4, maxWorkGroupSize);
        }

        free(devices);

    }

    free(platforms);
}


void Engine::testOpenCL()
{
    int i;
    const int LIST_SIZE = 100024000;
    int *A = (int*)malloc(sizeof(int)*LIST_SIZE);
    int *B = (int*)malloc(sizeof(int)*LIST_SIZE);
    for(i = 0; i < LIST_SIZE; i++) {
        A[i] = i;
        B[i] = LIST_SIZE - i;
    }

    // Create memory buffers on the device for each vector
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,LIST_SIZE * sizeof(int), NULL, &ret);

    Timer *t = new Timer();
    Timer *ts1 = new Timer();
    t->start();
    ts1->start();

    // Copy the lists A and B to their respective memory buffers
    clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), A, 0, NULL, NULL);
    clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), B, 0, NULL, NULL);

    // Set the arguments of the kernel
    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

    // Execute the OpenCL kernel on the list
    size_t global_item_size = LIST_SIZE; // Process the entire lists
    size_t local_item_size = 64; // Divide work items into groups of 64
    clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

    // Read the memory buffer C on the device to the local variable C
    int *C = (int*)malloc(sizeof(int)*LIST_SIZE);
    clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), C, 0, NULL, NULL);
}

void Engine::drawGUI()
{
    ImGui::NewFrame();

    //bool open = true;
    //ImGui::ShowDemoWindow(&open);

    gui_engine->draw(
        getDeltaTime(),
        finish,
        gameObjects, numberGameObjects,
        lightPoints, numberLightPoints,
        camera,
        tiles, tilesWidth
    );

    ImGui::Render();
}

void Engine::windowUpdate()
{
    if (EngineSetup::getInstance()->RENDER_WITH_HARDWARE) {
        this->processOpenCL( );
    }

    EngineBuffers::getInstance()->flipVideoBuffer( screenSurface );

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);

    drawGUI();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);

    screenTexture = SDL_CreateTextureFromSurface(renderer, screenSurface);
    SDL_RenderCopy(renderer, screenTexture, NULL, NULL);

    SDL_DestroyTexture(screenTexture);
}

void Engine::onStart()
{
    this->engineTimer.start();

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

void Engine::postUpdate()
{
    // update collider forces
    camera->UpdateVelocity();

    // update deltaTime
    this->updateTimer();

    this->processPairsCollisions();

    Vertex3D vel = Engine::camera->velocity.getComponent();

    btVector3 bulletVelocity = btVector3(vel.x, vel.y, vel.z);

    if (this->camera->kinematicController->onGround()) {
        bulletVelocity = btVector3(vel.x, vel.y, vel.z);
    } else {
        bulletVelocity = btVector3(vel.x, vel.y, vel.z) / EngineSetup::getInstance()->AIR_RESISTANCE;
    }

    this->camera->kinematicController->setWalkDirection(bulletVelocity);

    Vertex3D finalVelocity;
    if (EngineSetup::getInstance()->BULLET_STEP_SIMULATION) {
        // Bullet Step Simulation
        dynamicsWorld->stepSimulation(getDeltaTime(), 1);

        // Physics for meshes
        this->updatePhysicObjects();
        btTransform trans = this->camera->kinematicController->getGhostObject()->getWorldTransform();

        btVector3 pos = trans.getOrigin();
        float BSP_YOffset = 1;
        // El offset es porqué nuestros ojos deberian estar por encima del punto central
        // de la cápsula que hemos utilizando. De lo contrario lo colocaríamos en el centro del mismo la cámara.
        finalVelocity = Vertex3D(pos.getX(), pos.getY() - BSP_YOffset, pos.getZ());
    } else {
        finalVelocity = this->camera->velocity.vertex2;
    }

    this->camera->setPosition(finalVelocity);
    this->cameraUpdate();

    if (EngineSetup::getInstance()->SHOW_WEAPON) {
        this->getWeapon()->setEnabled(true);
        this->getWeapon()->updateWeaponPosition( camera );
    } else {
        this->getWeapon()->setEnabled(false);
    }
}

void Engine::processPairsCollisions()
{

    for (int i = 0; i < this->triggerCamera->getGhostObject()->getNumOverlappingObjects(); i++) {
        const btCollisionObject *obj = this->triggerCamera->getGhostObject()->getOverlappingObject(i);
        Mesh3D *brkObjectB = (Mesh3D *) obj->getUserPointer();
        if (brkObjectB->getLabel().find("hull") != std::string::npos) {
            int entityIndex = brkObjectB->getBspEntityIndex();
            char *classname = bsp_map->getEntityValue(entityIndex, "classname");

            if (!strcmp(classname, "trigger_teleport")) {
                int targetEntityId = bsp_map->getIndexOfFirstEntityByTargetname( bsp_map->getEntityValue(entityIndex, "target") );
                if (targetEntityId >= 0) {
                    Logging::getInstance()->getInstance()->Log( "Founded entity:" + std::string(bsp_map->getEntityValue(entityIndex, "target")));

                    if (this->bsp_map->hasEntityAttribute(targetEntityId, "origin")) {
                        char *value = bsp_map->getEntityValue(targetEntityId, "origin");
                        char *angle = bsp_map->getEntityValue(targetEntityId, "angle");
                        Vertex3D teleportPos = bsp_map->parsePositionFromEntityAttribute(value);

                        float BSP_YOffset = -5;
                        float BSP_ZOffset = 0;

                        int angleInt = atoi( std::string(angle).c_str() );
                        camera->yaw   = 90-angleInt;
                        camera->pitch = 0;
                        camera->roll  = 0;

                        btVector3 btPos = btVector3(teleportPos.x, teleportPos.y+BSP_YOffset, teleportPos.z+BSP_ZOffset);

                        btTransform initialTransform;
                        initialTransform.setOrigin( btPos );

                        this->camera->kinematicController->getGhostObject()->setWorldTransform(initialTransform);
                        Logging::getInstance()->getInstance()->Log( "teleporting to " +std::string(bsp_map->getEntityValue(entityIndex, "target")));
                    }
                }
            }

            if (!strcmp(classname, "func_door")) {
                Logging::getInstance()->getInstance()->Log( "ros");
                Tools::writeTextCenter(Engine::renderer, Engine::font, Color::white(), std::string("func_door") );
            }

            if (!strcmp(classname, "trigger_multiple")) {
                // check for message response
                if (strlen(bsp_map->getEntityValue(entityIndex, "message")) > 0) {
                    Tools::writeTextCenter(Engine::renderer, Engine::font, Color::white(), std::string(bsp_map->getEntityValue(entityIndex, "message")) );
                }
            }
        }

        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->getInstance()->Log("Collision between triggerCamera and " + brkObjectB->getLabel());
        }
    }

    int numManifolds = this->dynamicsWorld->getDispatcher()->getNumManifolds();
    if (EngineSetup::getInstance()->BULLET_CHECK_ALL_PAIRS) {
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold *contactManifold = this->dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
            if (contactManifold->getNumContacts() > 0) {
                const btCollisionObject *obA = contactManifold->getBody0();
                const btCollisionObject *obB = contactManifold->getBody1();

                Mesh3D *brkObjectA = (Mesh3D *) obA->getUserPointer();
                Mesh3D *brkObjectB = (Mesh3D *) obB->getUserPointer();

                if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
                    Logging::getInstance()->getInstance()->Log("Collision between " + brkObjectA->getLabel() + " and " + brkObjectB->getLabel());
                }
            }
        }
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

    this->getMesh3DTriangles();
    this->getQuakeMapTriangles();
    this->getLightPointsTriangles();
    this->getSpritesTriangles();
    this->getObjectsBillboardTriangles();

    this->hiddenSurfaceRemoval();

    if (EngineSetup::getInstance()->RENDER_WITH_HARDWARE) {
        this->handleTrianglesToTiles();
    } else {
        if (EngineSetup::getInstance()->BASED_TILE_RENDER) {
            this->handleTrianglesToTiles();
            this->drawTilesTriangles();
            if (EngineSetup::getInstance()->DRAW_TILES_GRID) {
                this->drawTilesGrid();
            }
        } else {
            this->drawFrameTriangles();
        }
    }

    this->numFrameTriangles = 0;

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


void Engine::getQuakeMapTriangles()
{
    if (bsp_map) {
        bsp_map->DrawVisibleLeaf(camera);
        if (EngineSetup::getInstance()->DRAW_BSP_HULLS) {
            bsp_map->DrawHulls( camera );
        }
    }
}

void Engine::getMesh3DTriangles()
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

void Engine::getLightPointsTriangles()
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

void Engine::getSpritesTriangles()
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

void Engine::getObjectsBillboardTriangles()
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

void Engine::drawFrameTriangles()
{
    for (int i = 0; i < numVisibleTriangles; i++) {
        this->visibleTriangles[i].draw(camera);
    }
}

void Engine::drawTilesGrid()
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

void Engine::hiddenSurfaceRemoval()
{
    numVisibleTriangles = 0;

    for (int i = 0; i < this->numFrameTriangles ; i++) {

        this->frameTriangles[i].updateObjectSpace();

        // back face culling (needs objectSpace)
        if (EngineSetup::getInstance()->TRIANGLE_BACK_FACECULLING) {
            if (this->frameTriangles[i].isBackFaceCulling(camera)) {
                continue;
            }
        }

        // Clipping (needs objectSpace)
        if (EngineSetup::getInstance()->TRIANGLE_RENDER_CLIPPING) {
            if (this->frameTriangles[i].testForClipping( camera ) ) {
                this->frameTriangles[i].clipping(camera, visibleTriangles, numVisibleTriangles);
                continue;
            }
        }

        // Frustum Culling (needs objectSpace)
        if (EngineSetup::getInstance()->TRIANGLE_FRUSTUM_CULLING) {
            if (!camera->frustum->isPointInFrustum(this->frameTriangles[i].Ao) &&
                !camera->frustum->isPointInFrustum(this->frameTriangles[i].Bo) &&
                !camera->frustum->isPointInFrustum(this->frameTriangles[i].Co)
            ) {
                continue;
            }
        }

        // Triangle precached data
        this->frameTriangles[i].updateCameraSpace(camera);
        this->frameTriangles[i].updateNDCSpace(camera);
        this->frameTriangles[i].updateScreenSpace(camera);
        this->frameTriangles[i].updateNormal();
        this->frameTriangles[i].updateFullArea();
        this->frameTriangles[i].updateUVCache();
        this->frameTriangles[i].updateBoundingBox();

        // Add triangle to visible list
        visibleTriangles[numVisibleTriangles] = this->frameTriangles[i];
        numVisibleTriangles++;
    }

    //Logging::getInstance()->Log("frameTriangles: " + std::to_string(numFrameTriangles) + ", numVisibleTriangles: " + std::to_string(numVisibleTriangles));
}

void Engine::drawTilesTriangles()
{
    for (int i = 0 ; i < this->numTiles ; i++) {
        if (!this->tiles[i].draw) continue;

        for (int j = 0 ; j < this->tiles[i].numTriangles ; j++) {
            int triangleId = this->tiles[i].triangleIds[j];
            Tile *t = &this->tiles[i];
            this->visibleTriangles[triangleId].drawForTile(
                camera,
                t->start_x,
                t->start_y,
                t->start_x+sizeTileWidth,
                t->start_y+sizeTileHeight
            );
        }
    }
}

void Engine::handleTrianglesToTiles()
{
    for (int i = 0 ; i < this->numTiles ; i++) {
        this->tiles[i].numTriangles = 0;
    }

    for (int i = 0 ; i < numVisibleTriangles ; i++) {
        int tileStartX =  std::max((float)(this->visibleTriangles[i].minX/this->sizeTileWidth), 0.0f);
        int tileEndX   =  std::min((float)(this->visibleTriangles[i].maxX/this->sizeTileWidth), (float)this->tilesWidth-1);

        int tileStartY =  std::max((float)(this->visibleTriangles[i].minY/this->sizeTileHeight), 0.0f);
        int tileEndY   =  std::min((float)(this->visibleTriangles[i].maxY/this->sizeTileHeight), (float)this->tilesHeight-1);

        for (int y = tileStartY; y <= tileEndY ; y++) {
            for (int x = tileStartX; x <= tileEndX ; x++) {
                int tileOffset = y * tilesWidth + x;
                this->tiles[tileOffset].triangleIds[ this->tiles[tileOffset].numTriangles ] = i;
                this->tiles[tileOffset].numTriangles++;
            }
        }
    }
}

void Engine::onEnd()
{
    this->engineTimer.stop();
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

    // Load start position from BSP
    Vertex3D bspOriginalPosition = this->bsp_map->getStartMapPosition();

    int entityID = this->bsp_map->getIndexOfFirstEntityByClassname("info_player_start");
    btTransform initialTransform;
    initialTransform.setOrigin( btVector3(bspOriginalPosition.x, bspOriginalPosition.y, bspOriginalPosition.z) );
    char *angle = bsp_map->getEntityValue(entityID, "angle");
    int angleInt = atoi( std::string(angle).c_str() );
    camera->yaw   = 90-angleInt;
    camera->pitch = 0;
    camera->roll  = 0;

    this->camera->setPosition(bspOriginalPosition);
    this->camera->kinematicController->getGhostObject()->setWorldTransform(initialTransform);
}

void Engine::processFPS()
{
    fps = countedFrames / ( this->engineTimer.getTicks() / 1000.f );
    if( fps > 2000000 ) { fps = 0; }
    ++countedFrames;

    int renderer_w, renderer_h;
    SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);
    Tools::writeText(Engine::renderer, Engine::font, renderer_w-150, 14, Color::yellow(), "FPS: " + Tools::floatTruncate(fps, 4) );
}

void Engine::updatePhysicObjects()
{
    std::vector<Mesh3DBody *>::iterator it;
    for (it = meshPhysics.begin() ; it!= meshPhysics.end() ; it++) {
        (*it)->integrate( );
    }

    // Sync position
    Vertex3D direction = camera->getRotation().getTranspose() * EngineSetup::getInstance()->forward;
    Vertex3D p = *camera->getPosition();

    float farDist = 1;
    p.x = p.x + direction.x * farDist;
    p.y = p.y + direction.y * farDist;
    p.z = p.z + direction.z * farDist;

    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(p.x, p.y, p.z));
    triggerCamera->getGhostObject()->setWorldTransform(t);
}

Weapon3D *Engine::getWeapon() const {
    return weapon;
}

void Engine::setWeapon(Weapon3D *weapon) {
    Engine::weapon = weapon;
}


bool Engine::needsCollision(const btCollisionObject* body0, const btCollisionObject* body1)
{
    bool collides = (body0->getBroadphaseHandle()->m_collisionFilterGroup & body1->getBroadphaseHandle()->m_collisionFilterMask) != 0;
    collides = collides && (body1->getBroadphaseHandle()->m_collisionFilterGroup & body0->getBroadphaseHandle()->m_collisionFilterMask);
    return collides;
}

void Engine::updateTimer()
{
    //Logging::getInstance()->Log("updateTimer: " + std::to_string(this->engineTimer.getTicks()));

    this->current_ticks = this->engineTimer.getTicks();
    this->deltaTime = this->current_ticks - this->last_ticks;
    this->last_ticks = this->current_ticks;

    this->timerCurrent += this->deltaTime/1000.f;
    //this->fps = 1000/this->deltaTime;
}

float Engine::getDeltaTime()
{
    return this->deltaTime/1000;
}

void Engine::initTiles()
{
    // Tiles Raster setup
    this->tilesWidth  = EngineSetup::getInstance()->SCREEN_WIDTH / this->sizeTileWidth;
    this->tilesHeight = EngineSetup::getInstance()->SCREEN_HEIGHT / this->sizeTileHeight;
    this->numTiles = tilesWidth * tilesHeight;

    Logging::getInstance()->Log("Tiles: ("+std::to_string(tilesWidth)+"x"+std::to_string(tilesHeight)+"), Size: ("+std::to_string(sizeTileWidth)+"x"+std::to_string(sizeTileHeight)+")");

    for (int y = 0 ; y < EngineSetup::getInstance()->SCREEN_HEIGHT; y+=this->sizeTileHeight)
    {
        for (int x = 0 ; x < EngineSetup::getInstance()->SCREEN_WIDTH; x+=this->sizeTileWidth)
        {
            Tile t;

            t.draw    = true;
            t.id_x    = (x/this->sizeTileWidth);
            t.id_y    = (y/this->sizeTileHeight);
            t.id      = t.id_y * this->tilesWidth + t.id_x;
            t.start_x = x;
            t.start_y = y;

            this->tiles.push_back(t);
            // Load up the vector with MyClass objects


            Logging::getInstance()->Log("Tiles: (id:" + std::to_string(t.id) + "), (offset_x: " + std::to_string(x)+", offset_y: " + std::to_string(y) + ")");
        }

    }
}
