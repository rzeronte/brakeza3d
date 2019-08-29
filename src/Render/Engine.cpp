#include "../../headers/Render/Engine.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Objects/BSPEntity3D.h"
#include <chrono>
#include <iostream>

#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "LinearMath/btTransform.h"
#include "../../headers/Objects/Cube3D.h"
#include <OpenCL/opencl.h>
#include <SDL_image.h>

#define MAX_SOURCE_SIZE (0x100000)


Engine::Engine()
{
    // GUI engine
    gui_engine = new GUI_Engine();

    // cam
    camera = new Camera3D();

    // Link GUI_log with Logging singleton
    Logging::getInstance()->setGUILog(gui_engine->gui_log);

    // input controller
    controller = new Controller();

    this->frameTriangles = new Triangle[250000];
    this->visibleTriangles = new Triangle[250000];

    IMGUI_CHECKVERSION();
    imgui_context = ImGui::CreateContext();

    this->initOpenCL();

    this->initTiles();

    bspMap = new BSPMap();
    weaponManager = new WeaponsManager();

    // Init Physics System
    this->collisionsManager = new CollisionsManager();
    this->initCollisionsManager();

    menu = new MenuManager();

    this->engineTimer.start();

}

bool Engine::initSound()
{
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
    }

    Mix_Volume(-1, EngineSetup::getInstance()->SOUND_VOLUME);
    Mix_VolumeMusic(EngineSetup::getInstance()->SOUND_VOLUME);
;
    EngineBuffers::getInstance()->loadWAVs();
}

bool Engine::initWindow()
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
                    "Window size: " + std::to_string(EngineSetup::getInstance()->screenWidth) + " x " + std::to_string(EngineSetup::getInstance()->screenHeight), "INFO"
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
        std::string pathFont = "../fonts/1467_pannartz_latin.ttf";
        Logging::getInstance()->Log("Loading FONT: " + pathFont, "INFO");

        font = TTF_OpenFont( pathFont.c_str(), EngineSetup::getInstance()->TEXT_3D_SIZE );
        if(!font) {
            Logging::getInstance()->Log(TTF_GetError(), "INFO");
        }
    }
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
    properties[0]= CL_CONTEXT_PLATFORM;
    properties[1]= (cl_context_properties) platform_id;
    properties[2]= 0;

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

void Engine::handleOpenCLTrianglesForTiles()
{
    // Create OCLTiles
    OCLTile tilesOCL[numTiles];
    for (int i = 0 ; i < numTiles ; i++) {

        tilesOCL[i].draw    = this->tiles[i].draw;
        tilesOCL[i].start_x = this->tiles[i].start_x;
        tilesOCL[i].start_y = this->tiles[i].start_y;
        tilesOCL[i].id      = this->tiles[i].id;
        tilesOCL[i].id_x    = this->tiles[i].id_x;
        tilesOCL[i].id_y    = this->tiles[i].id_y;

    }

    opencl_buffer_tiles = clCreateBuffer(contextCPU, CL_MEM_READ_ONLY, numTiles * sizeof(OCLTile), NULL, &ret);
    clEnqueueWriteBuffer(command_queue_rasterizer, opencl_buffer_tiles, CL_TRUE, 0, numTiles * sizeof(OCLTile), &tilesOCL, 0, NULL, NULL);

    for (int i = 0 ; i < numTiles ; i++) {
        if (!this->tiles[i].draw) continue;

        int numTrianglesTile = this->tiles[i].numTriangles;
        trianglesTile = new OCLTriangle[this->tiles[i].numTriangles];
        for (int j = 0 ; j < this->tiles[i].numTriangles ; j++) {
            int triangleId = this->tiles[i].triangleIds[j];
            trianglesTile[j] = EngineBuffers::getInstance()->OCLTrianglesBuffer[ triangleId ];
        }

        opencl_buffer_triangles = clCreateBuffer(contextCPU, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, this->tiles[i].numTriangles * sizeof(OCLTriangle), trianglesTile, &ret);

        clSetKernelArg(processTileTriangles, 0, sizeof(int), &this->tiles[i].id);
        clSetKernelArg(processTileTriangles, 1, sizeof(int), &EngineSetup::getInstance()->screenWidth);
        clSetKernelArg(processTileTriangles, 2, sizeof(int), &this->sizeTileWidth);
        clSetKernelArg(processTileTriangles, 3, sizeof(int), &this->sizeTileHeight);
        clSetKernelArg(processTileTriangles, 4, sizeof(cl_mem), (void *)&this->tiles[i].clBuffer);
        clSetKernelArg(processTileTriangles, 5, sizeof(cl_mem), (void *)&this->tiles[i].clBufferDepth);
        clSetKernelArg(processTileTriangles, 6, sizeof(cl_mem), (void *)&opencl_buffer_triangles);
        clSetKernelArg(processTileTriangles, 7, sizeof(cl_mem), (void *)&opencl_buffer_tiles);

        size_t global_item_size = numTrianglesTile; // Process the entire lists
        size_t local_item_size = 1;                 // Divide work items into groups of 64

        ret = clEnqueueNDRangeKernel(command_queue_rasterizer, processTileTriangles, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

        if (ret != CL_SUCCESS) {
            Logging::getInstance()->getInstance()->Log( "Error processTileTriangles: " + std::to_string(ret) );

            char buffer[1024];
            clGetProgramBuildInfo(programCPU, device_cpu_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
            if (strlen(buffer) > 0 ) {
                Logging::getInstance()->getInstance()->Log( buffer );
            }
        }
        clReleaseMemObject(opencl_buffer_triangles);
    }

    clFinish(command_queue_rasterizer);
    for (int i = 0 ; i < numTiles ; i++) {
        if (!this->tiles[i].draw) continue;
        this->dumpTileToFrameBuffer(&this->tiles[i]);
    }

    clReleaseMemObject(opencl_buffer_tiles);
    EngineBuffers::getInstance()->numOCLTriangles = 0;

}

void Engine::dumpTileToFrameBuffer(Tile *t)
{
    int start_x = t->start_x;
    int start_y = t->start_y;

    for (int buffer_y = 0; buffer_y < sizeTileHeight; buffer_y++) {
        for (int buffer_x = 0; buffer_x < sizeTileWidth; buffer_x++) {
            int index = (buffer_y * sizeTileWidth) + buffer_x;
            EngineBuffers::getInstance()->setVideoBuffer(start_x + buffer_x, start_y + buffer_y, t->buffer[index]);
        }
    }

    std::fill(t->buffer, t->buffer + (sizeTileWidth*sizeTileHeight), 0);
    std::fill(t->bufferDepth, t->bufferDepth + (sizeTileWidth*sizeTileHeight), 10000);
}

void Engine::handleOpenCLTransform()
{
    EngineBuffers::getInstance()->numOCLTriangles = 0;
    if (this->numFrameTriangles == 0) return;

    for (int i = 0; i < this->numFrameTriangles ; i++) {
        EngineBuffers::getInstance()->addOCLTriangle(this->frameTriangles[i].getOpenCL());
    }

    OCLPlane planesOCL[4];
    int cont = 0;
    for (int i = EngineSetup::getInstance()->LEFT_PLANE ; i <= EngineSetup::getInstance()->BOTTOM_PLANE ; i++) {
        planesOCL[cont].A[0] = camera->frustum->planes[i].A.x;
        planesOCL[cont].A[1] = camera->frustum->planes[i].A.y;
        planesOCL[cont].A[2] = camera->frustum->planes[i].A.z;

        planesOCL[cont].B[0] = camera->frustum->planes[i].B.x;
        planesOCL[cont].B[1] = camera->frustum->planes[i].B.y;
        planesOCL[cont].B[2] = camera->frustum->planes[i].B.z;

        planesOCL[cont].C[0] = camera->frustum->planes[i].C.x;
        planesOCL[cont].C[1] = camera->frustum->planes[i].C.y;
        planesOCL[cont].C[2] = camera->frustum->planes[i].C.z;

        planesOCL[cont].normal[0] = camera->frustum->planes[i].normal.x;
        planesOCL[cont].normal[1] = camera->frustum->planes[i].normal.y;
        planesOCL[cont].normal[2] = camera->frustum->planes[i].normal.z;

        cont++;
    }

    opencl_buffer_frustum   = clCreateBuffer(contextGPU, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 4 * sizeof(OCLPlane), planesOCL, &ret);
    opencl_buffer_triangles = clCreateBuffer(contextGPU, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, EngineBuffers::getInstance()->numOCLTriangles * sizeof(OCLTriangle), EngineBuffers::getInstance()->OCLTrianglesBuffer, &ret);

    Vertex3D camPos = *camera->getPosition();
    M3 camRot = camera->getRotation();
    float pitch = camRot.getPitch();
    float yaw   = camRot.getYaw();
    float roll  = camRot.getRoll();

    clSetKernelArg(processTransformTriangles, 0, sizeof(cl_mem), (void *)&opencl_buffer_triangles);

    clSetKernelArg(processTransformTriangles, 1, sizeof(float), &camPos.x);
    clSetKernelArg(processTransformTriangles, 2, sizeof(float), &camPos.y);
    clSetKernelArg(processTransformTriangles, 3, sizeof(float), &camPos.z);

    clSetKernelArg(processTransformTriangles, 4, sizeof(float), &pitch);
    clSetKernelArg(processTransformTriangles, 5, sizeof(float), &yaw);
    clSetKernelArg(processTransformTriangles, 6, sizeof(float), &roll);

    clSetKernelArg(processTransformTriangles, 7, sizeof(float), &camera->frustum->vNLpers.x);
    clSetKernelArg(processTransformTriangles, 8, sizeof(float), &camera->frustum->vNLpers.y);
    clSetKernelArg(processTransformTriangles, 9, sizeof(float), &camera->frustum->vNLpers.z);

    clSetKernelArg(processTransformTriangles, 10, sizeof(float), &camera->frustum->vNRpers.x);
    clSetKernelArg(processTransformTriangles, 11, sizeof(float), &camera->frustum->vNRpers.y);
    clSetKernelArg(processTransformTriangles, 12, sizeof(float), &camera->frustum->vNRpers.z);

    clSetKernelArg(processTransformTriangles, 13, sizeof(float), &camera->frustum->vNTpers.x);
    clSetKernelArg(processTransformTriangles, 14, sizeof(float), &camera->frustum->vNTpers.y);
    clSetKernelArg(processTransformTriangles, 15, sizeof(float), &camera->frustum->vNTpers.z);

    clSetKernelArg(processTransformTriangles, 16, sizeof(float), &camera->frustum->vNBpers.x);
    clSetKernelArg(processTransformTriangles, 17, sizeof(float), &camera->frustum->vNBpers.y);
    clSetKernelArg(processTransformTriangles, 18, sizeof(float), &camera->frustum->vNBpers.z);

    clSetKernelArg(processTransformTriangles, 19, sizeof(int), &EngineSetup::getInstance()->screenWidth);
    clSetKernelArg(processTransformTriangles, 20, sizeof(int), &EngineSetup::getInstance()->screenHeight);

    clSetKernelArg(processTransformTriangles, 21, sizeof(cl_mem), (void *)&opencl_buffer_frustum);

    size_t global_item_size = EngineBuffers::getInstance()->numOCLTriangles;
    size_t local_item_size = 1;

    cl_event event;
    ret = clEnqueueNDRangeKernel(command_queue_transforms, processTransformTriangles, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

    if (ret != CL_SUCCESS) {
        Logging::getInstance()->getInstance()->Log( "Error processTransformTriangles: " + std::to_string(ret) );

        char buffer[1024];
        clGetProgramBuildInfo(programGPU, device_gpu_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
        if (strlen(buffer) > 0 ) {
            Logging::getInstance()->getInstance()->Log( buffer );
        }
    }

    if (EngineSetup::getInstance()->OPENCL_SHOW_TIME_KERNELS) {
        clWaitForEvents(1, &event);
        clFinish(command_queue_transforms);

        cl_ulong time_start;
        cl_ulong time_end;

        clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
        clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
        double nanoSeconds = time_end-time_start;

        Logging::getInstance()->Log("OpenCL processTransformTriangles kernel time is: "+ std::to_string(nanoSeconds / 1000000.0) + " milliseconds");
    } else {
        clFinish(command_queue_transforms);
    }

    // Recover transform Data
    for (int i = 0; i < this->numFrameTriangles ; i++) {
        OCLTriangle oclt = EngineBuffers::getInstance()->OCLTrianglesBuffer[i];
        frameTriangles[i].Ao.x = oclt.Ao_x;
        frameTriangles[i].Ao.y = oclt.Ao_y;
        frameTriangles[i].Ao.z = oclt.Ao_z;
        frameTriangles[i].Bo.x = oclt.Bo_x;
        frameTriangles[i].Bo.y = oclt.Bo_y;
        frameTriangles[i].Bo.z = oclt.Bo_z;
        frameTriangles[i].Co.x = oclt.Co_x;
        frameTriangles[i].Co.y = oclt.Co_y;
        frameTriangles[i].Co.z = oclt.Co_z;

        frameTriangles[i].Ac.x = oclt.Ac_x;
        frameTriangles[i].Ac.y = oclt.Ac_y;
        frameTriangles[i].Ac.z = oclt.Ac_z;
        frameTriangles[i].Bc.x = oclt.Bc_x;
        frameTriangles[i].Bc.y = oclt.Bc_y;
        frameTriangles[i].Bc.z = oclt.Bc_z;
        frameTriangles[i].Cc.x = oclt.Cc_x;
        frameTriangles[i].Cc.y = oclt.Cc_y;
        frameTriangles[i].Cc.z = oclt.Cc_z;

        frameTriangles[i].An.x = oclt.An_x;
        frameTriangles[i].An.y = oclt.An_y;
        frameTriangles[i].An.z = oclt.An_z;
        frameTriangles[i].Bn.x = oclt.Bn_x;
        frameTriangles[i].Bn.y = oclt.Bn_y;
        frameTriangles[i].Bn.z = oclt.Bn_z;
        frameTriangles[i].Cn.x = oclt.Cn_x;
        frameTriangles[i].Cn.y = oclt.Cn_y;
        frameTriangles[i].Cn.z = oclt.Cn_z;

        frameTriangles[i].As.x = oclt.As_x;
        frameTriangles[i].As.y = oclt.As_y;
        frameTriangles[i].Bs.x = oclt.Bs_x;
        frameTriangles[i].Bs.y = oclt.Bs_y;
        frameTriangles[i].Cs.x = oclt.Cs_x;
        frameTriangles[i].Cs.y = oclt.Cs_y;

        frameTriangles[i].minX = oclt.minX;
        frameTriangles[i].maxX = oclt.maxX;
        frameTriangles[i].minY = oclt.minY;
        frameTriangles[i].maxY = oclt.maxY;

        frameTriangles[i].normal.x = oclt.normal[0];
        frameTriangles[i].normal.y = oclt.normal[1];
        frameTriangles[i].normal.z = oclt.normal[2];

        frameTriangles[i].is_clipped_cl = oclt.is_clipping;
    }

    EngineBuffers::getInstance()->numOCLTriangles = 0;

}

void Engine::initTiles()
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

            this->tiles.push_back(t);
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

        this->tiles[i].clBuffer = clCreateBuffer(contextCPU, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, tilePixelsBufferSize * sizeof(unsigned int), this->tiles[i].buffer, &ret);
        this->tiles[i].clBufferDepth = clCreateBuffer(contextCPU, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, tilePixelsBufferSize * sizeof(float), this->tiles[i].bufferDepth, &ret);
    }
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

void Engine::drawGUI()
{
    ImGui::NewFrame();

    //bool open = true;
    //ImGui::ShowDemoWindow(&open);

    gui_engine->draw(
            getDeltaTime(),
            finish,
            gameObjects,
            lightPoints,
            camera,
            tiles, tilesWidth,
            this->numVisibleTriangles,
            mapsJSONList,
            weaponManager
    );

    ImGui::Render();
}

void Engine::windowUpdate()
{
    Engine::processFPS();

    if (EngineSetup::getInstance()->MENU_ACTIVE) {
        this->drawMenuScreen();
    }

    EngineBuffers::getInstance()->flipVideoBuffer( screenSurface );

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);

    EngineSetup::getInstance()->EVENT_GUI = false;
    drawGUI();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    if (EngineSetup::getInstance()->EVENT_GUI) {
        if (EngineSetup::getInstance()->EVENT_LAUNCH == EngineSetup::getInstance()->EVENT_GUI_CHANGE_MAP) {
            this->loadBSP(std::string(EngineSetup::getInstance()->EVENT_DATA).c_str(), "palette.lmp");
        }

        Logging::getInstance()->Log("Event from GUI (" + std::to_string(EngineSetup::getInstance()->EVENT_LAUNCH) + ", " + EngineSetup::getInstance()->EVENT_DATA + ")");
    }

    SDL_GL_SwapWindow(window);

    screenTexture = SDL_CreateTextureFromSurface(renderer, screenSurface);
    SDL_RenderCopy(renderer, screenTexture, NULL, NULL);

    SDL_DestroyTexture(screenTexture);
}

void Engine::onStart()
{
    Engine::camera->setPosition( EngineSetup::getInstance()->CameraPosition );

    Engine::camera->velocity.vertex1 = *Engine::camera->getPosition();
    Engine::camera->velocity.vertex2 = *Engine::camera->getPosition();

    this->menu->getOptionsJSON();
    this->getWeaponsJSON();
    this->getMapsJSON();

    this->initSound();

    Mix_PlayMusic(EngineBuffers::getInstance()->snd_base_menu, -1 );
}

void Engine::preUpdate()
{
    this->controller->resetFlags();

    // Posición inicial del vector velocidad que llevará la cámara
    camera->velocity.vertex1 = *camera->getPosition();
    camera->velocity.vertex2 = *camera->getPosition();

    // Determinamos VPS
    if (bspMap->isLoaded()) {

        int leafType = NULL;

        if (this->bspMap->currentLeaf != NULL) {
            leafType = this->bspMap->currentLeaf->type;
        }

        bspleaf_t *leaf = bspMap->FindLeaf(camera );
        bspMap->setVisibleSet(leaf);

        if (leafType != bspMap->currentLeaf->type) {
            if (bspMap->isCurrentLeafLiquid()) {
                this->camera->kinematicController->setFallSpeed(5);
            } else {
                this->camera->kinematicController->setFallSpeed(256);
            }
        }
    }
}

void Engine::postUpdate()
{
    // update collider forces
    camera->UpdateVelocity();

    // update deltaTime
    this->updateTimer();

    // step simulation
    Vertex3D finalVelocity = collisionsManager->stepSimulation(getDeltaTime());

    // update camera position/rotation/frustum
    this->camera->setPosition(finalVelocity);
    this->camera->UpdateRotation();
    this->camera->UpdateFrustum();
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



    this->getQuakeMapTriangles();
    this->getMesh3DTriangles();
    this->getLightPointsTriangles();
    this->getSpritesTriangles();
    this->getObjectsBillboardTriangles();

    if (EngineSetup::getInstance()->TRANSFORMS_OPENCL) {
        this->handleOpenCLTransform();
    }

    // temporal
    for (int i = 0; i < this->gameObjects.size(); i++) {
        Decal *dec = dynamic_cast<Decal*> (this->gameObjects[i]);
        if (dec != NULL) {
        }
    }

    this->hiddenSurfaceRemoval();

    if (EngineSetup::getInstance()->BASED_TILE_RENDER) {
        this->handleTrianglesToTiles();

        if (!EngineSetup::getInstance()->RASTERIZER_OPENCL) {
            this->drawTilesTriangles();
        } else {
            this->handleOpenCLTrianglesForTiles();
        }

        if (EngineSetup::getInstance()->DRAW_TILES_GRID) {
            this->drawTilesGrid();
        }
    } else {
        if (!EngineSetup::getInstance()->RASTERIZER_OPENCL) {
            this->drawFrameTriangles();
        } else {
            this->handleOpenCLTriangles();
        }
    }


    if (EngineSetup::getInstance()->BULLET_DEBUG_MODE) {
        collisionsManager->getDynamicsWorld()->debugDrawWorld();
    }

    if (EngineSetup::getInstance()->DRAW_FRUSTUM) {
        Drawable::drawFrustum(camera->frustum, camera, true, true, true);
    }

    if (EngineSetup::getInstance()->RENDER_MAIN_AXIS) {
        Drawable::drawMainAxis( camera );
    }

    if (EngineSetup::getInstance()->DRAW_CROSSHAIR) {
        Drawable::drawCrossHair();
    }

    if (EngineSetup::getInstance()->SHOW_WEAPON) {
        this->weaponManager->onUpdate(this->camera, this->controller->isFiring(), screenSurface, this->camera->velocity);
    }

    if (bspMap->isLoaded() && bspMap->isCurrentLeafLiquid() && !EngineSetup::getInstance()->MENU_ACTIVE) {
        this->waterShader();
    }
}

void Engine::clearLightPointsShadowMappings()
{
    for (int i = 0; i < this->lightPoints.size(); i++) {
        if (!this->lightPoints[i]->isEnabled()) {
            continue;
        }

        this->lightPoints[i]->clearShadowMappingBuffer();
    }
}

void Engine::objects3DShadowMapping()
{
    for (int i = 0; i < this->gameObjects.size(); i++) {
        if (!this->gameObjects[i]->isEnabled()) {
            continue;
        }

        Mesh3D *oMesh = dynamic_cast<Mesh3D*> (this->gameObjects[i]);
        if (oMesh != NULL) {
            for (int j = 0; j < this->lightPoints.size(); j++) {
                if (oMesh->isShadowCaster()) {
                    oMesh->shadowMapping(this->lightPoints[j]);
                }
            }
        }
    }
}


void Engine::getQuakeMapTriangles()
{
    if (bspMap->isLoaded() && EngineSetup::getInstance()->RENDER_BSP_MAP) {
        bspMap->DrawVisibleLeaf(camera);
        if (EngineSetup::getInstance()->DRAW_BSP_HULLS) {
            bspMap->DrawHulls(camera );
        }
    }
}

void Engine::getMesh3DTriangles()
{
    // draw meshes
    for (int i = 0; i < this->gameObjects.size(); i++) {
        Mesh3D *oMesh = dynamic_cast<Mesh3D*> (this->gameObjects[i]);
        if (oMesh != NULL) {
            if (oMesh->isEnabled()) {
                oMesh->draw(camera);
                if (EngineSetup::getInstance()->RENDER_OBJECTS_AXIS) {
                    Drawable::drawObject3DAxis(oMesh, camera, true, true, true);
                }
                if (EngineSetup::getInstance()->TEXT_ON_OBJECT3D) {
                    Tools::writeText3D(Engine::renderer, camera, Engine::font, *oMesh->getPosition(), EngineSetup::getInstance()->TEXT_3D_COLOR, oMesh->getLabel());
                }
            }
        }
    }
}

void Engine::getLightPointsTriangles()
{
    for (int i = 0; i < this->lightPoints.size(); i++) {
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
    std::vector<Object3D *>::iterator itObject3D;
    for ( itObject3D = gameObjects.begin(); itObject3D != gameObjects.end(); ) {
        Object3D *object = *(itObject3D);

        // Check for delete
        if (object->isRemoved()) {
            gameObjects.erase(itObject3D);
            continue;
        } else {
            itObject3D++;
        }

        // Sprite Directional 3D
        SpriteDirectional3D *oSpriteDirectional = dynamic_cast<SpriteDirectional3D*> (object);
        if (oSpriteDirectional != NULL) {

            if (!oSpriteDirectional->isEnabled()) {
                continue;
            }

            oSpriteDirectional->updateTrianglesCoordinates(camera);
            oSpriteDirectional->draw(camera);

            if (EngineSetup::getInstance()->RENDER_OBJECTS_AXIS) {
                Drawable::drawObject3DAxis(oSpriteDirectional, camera, true, true, true);
            }

            if (EngineSetup::getInstance()->TEXT_ON_OBJECT3D) {
                Tools::writeText3D(Engine::renderer, camera, Engine::font, *oSpriteDirectional->getPosition(), EngineSetup::getInstance()->TEXT_3D_COLOR, oSpriteDirectional->getLabel());
            }
        }

        // Sprite 3D
        Sprite3D *oSprite = dynamic_cast<Sprite3D*> (object);
        if (oSprite != NULL) {
            if (!oSprite->isEnabled()) {
                continue;
            }

            oSprite->updateTrianglesCoordinatesAndTexture(camera);
            oSprite->draw(camera);

            if (EngineSetup::getInstance()->RENDER_OBJECTS_AXIS) {
                Drawable::drawObject3DAxis(oSprite, camera, true, true, true);
            }

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
        for (int i = 0; i < this->gameObjects.size(); i++) {
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

        // Si la transformación es mediante openCL ya están todas hechas
        // pero si vamos por software actualizamos ObjectSpace y su normal
        // ya que es el mínimo necesario para el clipping y el faceculling

        if (!EngineSetup::getInstance()->TRANSFORMS_OPENCL) {
            this->frameTriangles[i].updateObjectSpace();
            this->frameTriangles[i].updateNormal();
        }

        // back face culling (needs objectSpace)
        if (EngineSetup::getInstance()->TRIANGLE_BACK_FACECULLING) {
            if (this->frameTriangles[i].isBackFaceCulling(camera->getPosition())) {
                continue;
            }
        }

        // Clipping (needs objectSpace)
        bool needClipping = false;
        if (EngineSetup::getInstance()->TRANSFORMS_OPENCL) {
            if (this->frameTriangles[i].is_clipped_cl ) {
                needClipping = true;
            }
        } else {
            if (this->frameTriangles[i].testForClipping( camera->frustum->planes,
                                                         EngineSetup::getInstance()->LEFT_PLANE,
                                                         EngineSetup::getInstance()->BOTTOM_PLANE )
             ) {
                needClipping = true;
            }
        }

        if (needClipping) {
            this->frameTriangles[i].clipping(
                    camera,
                    camera->frustum->planes,
                    EngineSetup::getInstance()->LEFT_PLANE,
                    EngineSetup::getInstance()->BOTTOM_PLANE,
                    frameTriangles[i].parent,
                    visibleTriangles,
                    numVisibleTriangles,
                    frameTriangles[i].isBSP
            );
            continue;
        }

        // Frustum Culling (needs objectSpace)
        if (!camera->frustum->isPointInFrustum(this->frameTriangles[i].Ao) &&
            !camera->frustum->isPointInFrustum(this->frameTriangles[i].Bo) &&
            !camera->frustum->isPointInFrustum(this->frameTriangles[i].Co)
        ) {
            continue;
        }

        // Triangle precached data
        // Estas operaciones las hacemos después de descartar triángulos
        // para optimización en el rasterizador por software
        if (!EngineSetup::getInstance()->TRANSFORMS_OPENCL) {
            this->frameTriangles[i].updateCameraSpace(camera);
            this->frameTriangles[i].updateNDCSpace(camera);
            this->frameTriangles[i].updateScreenSpace(camera);
            this->frameTriangles[i].updateBoundingBox();
        }

        if (!EngineSetup::getInstance()->RASTERIZER_OPENCL) {
            this->frameTriangles[i].updateFullArea();
            this->frameTriangles[i].updateUVCache();
        } else {
            EngineBuffers::getInstance()->addOCLTriangle(this->frameTriangles[i].getOpenCL());
        }

        // Add triangle to visible list
        visibleTriangles[numVisibleTriangles] = this->frameTriangles[i];
        numVisibleTriangles++;
    }

    if (EngineSetup::getInstance()->DEBUG_RENDER_INFO) {
        Logging::getInstance()->Log("[DEBUG_RENDER_INFO] frameTriangles: " + std::to_string(numFrameTriangles) + ", numVisibleTriangles: " + std::to_string(numVisibleTriangles));
    }

    this->numFrameTriangles = 0;

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
    obj->setLabel(label);
    gameObjects.push_back(obj);
    //gameObjects[numberGameObjects] = obj;
    //gameObjects[numberGameObjects]->setLabel(label);
    //numberGameObjects++;
}

void Engine::addLightPoint(LightPoint3D *lightPoint, std::string label)
{
    Logging::getInstance()->Log("Adding LightPoint: '" + label + "'", "INFO");

    lightPoints.push_back(lightPoint);
}

Object3D* Engine::getObjectByLabel(std::string label)
{
    for (int i = 0; i < this->gameObjects.size(); i++ ) {
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
    Logging::getInstance()->Log("Loading BSP Quake map: " + std::string(bspFilename));

    EngineSetup::getInstance()->BULLET_STEP_SIMULATION = true;

    this->bspMap->Initialize(bspFilename, paletteFilename);

    // Load start position from BSP
    Vertex3D bspOriginalPosition = this->bspMap->getStartMapPosition();

    int entityID = this->bspMap->getIndexOfFirstEntityByClassname("info_player_start");
    btTransform initialTransform;
    initialTransform.setOrigin( btVector3(bspOriginalPosition.x, bspOriginalPosition.y, bspOriginalPosition.z) );
    char *angle = bspMap->getEntityValue(entityID, "angle");
    int angleInt = atoi( std::string(angle).c_str() );
    camera->yaw   = 90-angleInt;
    camera->pitch = 0;
    camera->roll  = 0;

    this->camera->setPosition(bspOriginalPosition);
    this->camera->kinematicController->getGhostObject()->setWorldTransform(initialTransform);
}

void Engine::processFPS()
{
    if (!EngineSetup::getInstance()->DRAW_FPS) return;

    ++fpsFrameCounter;
    if (this->frameTime > 1000) {
        fps = fpsFrameCounter;
        frameTime = 0;
        fpsFrameCounter = 0;
    }

    int renderer_w, renderer_h;
    SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);
    Tools::writeText(Engine::renderer, Engine::font, renderer_w-150, 14, Color::yellow(), std::to_string(fps) +"fps");
}

void Engine::updateTimer()
{
    this->current_ticks = this->engineTimer.getTicks();
    this->deltaTime = this->current_ticks - this->last_ticks;
    this->last_ticks = this->current_ticks;

    this->frameTime += this->deltaTime;
    this->executionTime += this->deltaTime / 1000.f;

}

float Engine::getDeltaTime()
{
    return this->deltaTime/1000;
}

void Engine::waterShader()
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
    switch(bspMap->currentLeaf->type) {
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

            int nx = (cache1 + LAVA_INTENSITY * sin(LAVA_SPEED * this->executionTime + cache2) ) * LAVA_SCALE;
            int ny = (cache2 + LAVA_INTENSITY * sin(LAVA_SPEED * this->executionTime + cache1) ) * LAVA_SCALE;

            int bufferIndex = nx + ny * EngineSetup::getInstance()->screenWidth;

            if(Tools::isPixelInWindow(nx, ny)) {
                newVideoBuffer[bufferIndex] = currentPixelColor;
            }
        }
    }

    memcpy (&EngineBuffers::getInstance()->videoBuffer, &newVideoBuffer, sizeof(newVideoBuffer));
}

void Engine::getWeaponsJSON()
{
    size_t file_size;
    std::string filePath = EngineSetup::getInstance()->CONFIG_FOLDER + EngineSetup::getInstance()->CFG_WEAPONS;
    const char* mapsFile = Tools::readFile(filePath, file_size);
    cJSON *myDataJSON = cJSON_Parse(mapsFile);

    if (myDataJSON == NULL) {
        Logging::getInstance()->Log(filePath + " can't be loaded", "ERROR");
        return;
    }

    weaponsJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "weapons" );
    int sizeWeaponsList = cJSON_GetArraySize(weaponsJSONList);

    if (sizeWeaponsList > 0) {
        Logging::getInstance()->Log(filePath + " have " + std::to_string(sizeWeaponsList));
    } else {
        Logging::getInstance()->Log(filePath + " is empty", "ERROR");
    }

    // weapons loop
    cJSON *currentWeapon;
    cJSON_ArrayForEach(currentWeapon, weaponsJSONList) {
        cJSON *name = cJSON_GetObjectItemCaseSensitive(currentWeapon, "name");
        cJSON *hit = cJSON_GetObjectItemCaseSensitive(currentWeapon, "hit");
        cJSON *cadence = cJSON_GetObjectItemCaseSensitive(currentWeapon, "cadence");
        cJSON *damage = cJSON_GetObjectItemCaseSensitive(currentWeapon, "damage");
        cJSON *speed = cJSON_GetObjectItemCaseSensitive(currentWeapon, "speed");
        cJSON *projectileW = cJSON_GetObjectItemCaseSensitive(currentWeapon, "projectile_width");
        cJSON *projectileH = cJSON_GetObjectItemCaseSensitive(currentWeapon, "projectile_height");

        // Weapon Type attributes
        weaponManager->addWeaponType(name->valuestring);
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setHitType(hit->valueint);
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setCadence((float)cadence->valuedouble);
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setDamage(damage->valueint);
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setSpeed((float)speed->valuedouble);
        weaponManager->getWeaponTypeByLabel(name->valuestring)->setProjectileSize(projectileW->valuedouble, projectileH->valuedouble);

        cJSON *mark = cJSON_GetObjectItemCaseSensitive(currentWeapon, "mark");
        cJSON *markPath = cJSON_GetObjectItemCaseSensitive(mark, "path");
        cJSON *markFrames = cJSON_GetObjectItemCaseSensitive(mark, "frames");
        cJSON *markFps = cJSON_GetObjectItemCaseSensitive(mark, "fps");
        cJSON *markW = cJSON_GetObjectItemCaseSensitive(mark, "width");
        cJSON *markH = cJSON_GetObjectItemCaseSensitive(mark, "height");

        weaponManager->getWeaponTypeByLabel(name->valuestring)->setupMarkTemplate(
                markPath->valuestring,
                (int) markFrames->valueint,
                (int) markFps->valueint,
                (float) markW->valuedouble,
                (float) markH->valuedouble
        );

        Logging::getInstance()->Log("Weapon JSON detected: name: " + std::string(name->valuestring) +
            ", hitType: " + std::to_string(hit->valueint) +
            ", cadence: " + std::to_string(cadence->valuedouble) +
            ", speed: " + std::to_string(speed->valuedouble)
        );

        Logging::getInstance()->Log("JSON Weapon mark details for : " + std::string(name->valuestring) +
            ", path: " + markPath->valuestring +
            ", frames: " + std::to_string(markFrames->valueint) +
            ", fps: " + std::to_string(markFps->valueint) +
            ", w: " + std::to_string(markW->valuedouble) + ", h: " + std::to_string(markH->valuedouble)
        );

        // animation's weapon loop
        cJSON *weaponAnimationsJSONList;
        weaponAnimationsJSONList = cJSON_GetObjectItemCaseSensitive(currentWeapon, "animations" );

        cJSON *currentWeaponAnimation;
        cJSON_ArrayForEach(currentWeaponAnimation, weaponAnimationsJSONList) {
            cJSON *subfolder = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "subfolder");
            cJSON *frames    = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "frames");
            cJSON *fps       = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "fps");
            cJSON *offsetX   = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "offsetX");
            cJSON *offsetY   = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "offsetY");

            Logging::getInstance()->Log("Reading JSON Weapon Animation: " + std::string(subfolder->valuestring));

            weaponManager->getWeaponTypeByLabel(name->valuestring)->addAnimation(
                std::string(name->valuestring) + "/" + std::string(subfolder->valuestring),
                frames->valueint,
                fps->valueint,
                offsetX->valueint,
                offsetY->valueint
            );
        }
    }
}

void Engine::getMapsJSON()
{
    size_t file_size;
    const char* mapsFile = Tools::readFile(EngineSetup::getInstance()->CONFIG_FOLDER + EngineSetup::getInstance()->CFG_MAPS, file_size);
    cJSON *myDataJSON = cJSON_Parse(mapsFile);
    if (myDataJSON == NULL) {
        Logging::getInstance()->Log("maps.json can't be loaded", "ERROR");
        return;
    }

    cJSON *currentMap = NULL;
    mapsJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "maps" );
    int sizeMaps = cJSON_GetArraySize(mapsJSONList);

    if (sizeMaps > 0) {
        cJSON *firstMap = cJSON_GetArrayItem(mapsJSONList, 0);
        cJSON *nameMap = cJSON_GetObjectItemCaseSensitive(firstMap, "name");
        Logging::getInstance()->Log("maps.json have " + std::to_string(sizeMaps) + " maps");
        if (EngineSetup::getInstance()->CFG_AUTOLOAD_MAP) {
            this->loadBSP(nameMap->valuestring, "palette.lmp");
        }
    } else {
        Logging::getInstance()->Log("maps.json is empty", "ERROR");
    }

    cJSON_ArrayForEach(currentMap, mapsJSONList) {
        cJSON *nameMap = cJSON_GetObjectItemCaseSensitive(currentMap, "name");

        if (cJSON_IsString(nameMap)) {
            Logging::getInstance()->Log("Map JSON detected: " + std::string(nameMap->valuestring));
        }
    }
}

void Engine::drawMenuScreen()
{
    //this->waterShader();
    this->menu->drawOptions(screenSurface);
    Drawable::drawFireShader();
}

void Engine::initCollisionsManager()
{
    this->collisionsManager->setBspMap(this->bspMap);
    this->collisionsManager->setWeaponManager(this->weaponManager);
    this->collisionsManager->setCamera(this->camera);
    this->collisionsManager->setGameObjects(&this->gameObjects);
    this->collisionsManager->initBulletSystem();
}