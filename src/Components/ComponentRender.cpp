
#include "../../include/Components/ComponentRender.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Render/Transforms.h"

ComponentRender::ComponentRender()
:
    fps(0),
    fpsFrameCounter(0),
    frameTime(0),
    sizeTileWidth((EngineSetup::get()->screenWidth / 2)),
    sizeTileHeight((EngineSetup::get()->screenHeight / 2)),
    numberTilesHorizontal(0),
    numberTilesVertical(0),
    numberTiles(0),
    tilePixelsBufferSize(0),
    selectedObject(nullptr),
    clPlatformId(nullptr),
    clDeviceId(nullptr),
    ret_num_devices(0),
    ret_num_platforms(0),
    ret(0),
    clContext(nullptr),
    clCommandQueue(nullptr),
    stateScripts(EngineSetup::LuaStateScripts::LUA_STOP),
    sceneShadersEnabled(true)
{
}

void ComponentRender::onStart()
{
    Logging::head("ComponentRender onStart");
    setEnabled(true);

    initTiles();

    textWriter = new TextWriter(ComponentsManager::get()->getComponentWindow()->getRenderer(), ComponentsManager::get()->getComponentWindow()->getFontDefault());
}

void ComponentRender::preUpdate()
{
    this->updateFPS();

    if (isSceneShadersEnabled()) {
        runShadersOpenCLPreUpdate();
    }
}

void ComponentRender::drawObjetsInHostBuffer()
{
    auto components = ComponentsManager::get();

    /*auto &sceneObjects = Brakeza3D::get()->getSceneObjects();
    for (auto object : sceneObjects) {
        object->onDrawHostBuffer();
    }

    if (SETUP->RENDER_MAIN_AXIS) {
        Drawable::drawMainAxis(components->getComponentCamera()->getCamera());
    }

    if (SETUP->DRAW_CROSSHAIR) {
        Drawable::drawCrossHair();
    }

    if (SETUP->BULLET_DEBUG_MODE) {
        components->getComponentCollisions()->getDynamicsWorld()->debugDrawWorld();
    }*/

    if (SETUP->DRAW_FPS) {
        textWriter->writeTTFCenterHorizontal(
            10,
            std::to_string(components->getComponentRender()->getFps()).c_str(),
            Color::green(),
            0.3
        );
    }

    //this->hiddenSurfaceRemoval();
    //this->drawVisibleTriangles();

    //frameTriangles.clear();
    //spritesTriangles.clear();

}

void ComponentRender::onUpdate()
{
    if (!isEnabled()) return;

    deleteRemovedObjects();

    onUpdateSceneObjects();

    if (SETUP->ENABLE_LIGHTS) {
        this->updateLights();
    }

    if (stateScripts == EngineSetup::LUA_PLAY) {
        runScripts();
    }
    if (isSceneShadersEnabled()) {
        runShadersOpenCLPostUpdate();
    }
}

void ComponentRender::postUpdate()
{

    this->onPostUpdateSceneObjects();
}


void ComponentRender::writeOCLBufferIntoHost() const
{
    clEnqueueReadBuffer(clCommandQueue,
        EngineBuffers::get()->videoBufferOCL,
        CL_TRUE,
        0,
        EngineBuffers::get()->sizeBuffers * sizeof(Uint32),
        EngineBuffers::get()->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    /*clEnqueueReadBuffer(
        clCommandQueue,
        EngineBuffers::get()->depthBufferOCL,
        CL_TRUE,
        0,
        EngineBuffers::get()->sizeBuffers * sizeof(float),
        EngineBuffers::get()->depthBuffer,
        0,
        nullptr,
        nullptr
    );*/
}

void ComponentRender::writeOCLBuffersFromHost() const
{
    cl_int ret;

    /*ret = clEnqueueWriteBuffer(
        clCommandQueue,
        EngineBuffers::get()->videoBufferOCL,
        CL_FALSE,
        0,
        EngineBuffers::get()->sizeBuffers * sizeof(Uint32),
        EngineBuffers::get()->videoBuffer,
        0,
        nullptr,
        nullptr
    );*/
    float max_value2 = 0;
    clEnqueueFillBuffer(clCommandQueue, EngineBuffers::get()->videoBufferOCL, &max_value2, sizeof(float), 0, EngineBuffers::get()->sizeBuffers * sizeof(unsigned int), 0, NULL, NULL);

    if (ret != CL_SUCCESS) {
        Logging::Log("Error writing to Video OCL Buffer: (%d)", ret);
    }

    float max_value = 100000;
    clEnqueueFillBuffer(clCommandQueue, EngineBuffers::get()->depthBufferOCL, &max_value, sizeof(float), 0, EngineBuffers::get()->sizeBuffers * sizeof(unsigned int), 0, NULL, NULL);

    if (ret != CL_SUCCESS) {
        Logging::Log("Error writing to Depth OCL Buffer (%d)", ret);
    }
}
void ComponentRender::onEnd() {
}

void ComponentRender::onSDLPollEvent(SDL_Event *event, bool &finish)
{
    if (SETUP->CLICK_SELECT_OBJECT3D) {
        updateSelectedObject3D();
    }
}

void ComponentRender::updateSelectedObject3D()
{
    auto input = ComponentsManager::get()->getComponentInput();

    if (input->isClickLeft() && !input->isMouseMotion()) {
        int index = -1;
        selectedObject = getObject3DFromClickPoint(
            input->getRelativeRendererMouseX(),
            input->getRelativeRendererMouseY(),
            index
        );

        if (selectedObject != nullptr){
            Logging::Message("Selected object: %s", selectedObject->getLabel().c_str());
            Brakeza3D::get()->getManagerGui()->setSelectedObjectIndex(index);
        }
    }

    if (input->isClickRight() && !input->isMouseMotion()) {
        setSelectedObject(nullptr);
    }
}


std::vector<Triangle *> &ComponentRender::getFrameTriangles() {
    return frameTriangles;
}

std::vector<Triangle *> &ComponentRender::getVisibleTriangles() {
    return visibleTriangles;
}

std::vector<Triangle *> &ComponentRender::getSpritesTriangles() {
    return spritesTriangles;
}

std::vector<LightPoint3D *> &ComponentRender::getLightPoints()
{
    return lightPoints;
}

void ComponentRender::deleteRemovedObjects()
{
    if (!SETUP->EXECUTE_GAMEOBJECTS_ONUPDATE) return;

    auto &sceneObjects = Brakeza3D::get()->getSceneObjects();
    sceneObjects.erase(
        std::remove_if(
            sceneObjects.begin(),
            sceneObjects.end(), [](Object3D* object) {
                if (object->isRemoved()) {
                    delete object;
                    return true;
                }
                return false;
            }
        ),
        sceneObjects.end()
    );
}

void ComponentRender::onUpdateSceneObjects()
{
    auto sceneObjects = Brakeza3D::get()->getSceneObjects();

    for (auto object : sceneObjects) {
        if (object != nullptr && object->isEnabled()) {
            object->onUpdate();
        }
    }
}

void ComponentRender::onUpdateSceneObjectsSecondPass() const
{
    auto &sceneObjects = Brakeza3D::get()->getSceneObjects();
    for (auto object : sceneObjects) {
        if (object != nullptr && object->isEnabled()) {
            object->drawOnUpdateSecondPass();
        }
    }
}

void ComponentRender::hiddenOctreeRemoval() {
    std::vector<Triangle *> newFrameTriangles;

    this->frameTriangles = newFrameTriangles;
}

void ComponentRender::hiddenOctreeRemovalNode(OctreeNode *node, std::vector<Triangle *> &triangles)
{
    auto frustum = ComponentsManager::get()->getComponentCamera()->getCamera()->getFrustum();
    if (node->isLeaf() &&frustum->isAABBInFrustum(&node->bounds)) {
        for (auto & triangle : node->triangles) {
            triangles.push_back(triangle);
        }
    }

    for (auto & i : node->children) {
        if (i != nullptr) {
            this->hiddenOctreeRemovalNode(i, triangles);
        }
    }
}

void ComponentRender::initTiles() {
    if (SETUP->screenWidth % this->sizeTileWidth != 0) {
        printf("Bad sizeTileWidth\r\n");
        exit(-1);
    }
    if (SETUP->screenHeight % this->sizeTileHeight != 0) {
        printf("Bad sizeTileHeight\r\n");
        exit(-1);
    }

    // Tiles Raster setup
    this->numberTilesHorizontal = SETUP->screenWidth / this->sizeTileWidth;
    this->numberTilesVertical = SETUP->screenHeight / this->sizeTileHeight;
    this->numberTiles = numberTilesHorizontal * numberTilesVertical;
    this->tilePixelsBufferSize = this->sizeTileWidth * this->sizeTileHeight;

    for (int y = 0; y < SETUP->screenHeight; y += this->sizeTileHeight) {
        for (int x = 0; x < SETUP->screenWidth; x += this->sizeTileWidth) {

            Tile t;

            t.draw = true;
            t.id_x = (x / this->sizeTileWidth);
            t.id_y = (y / this->sizeTileHeight);
            t.id = t.id_y * this->numberTilesHorizontal + t.id_x;
            t.start_x = x;
            t.start_y = y;

            this->tiles.emplace_back(t);
            // Load up the vector with MyClass objects
        }
    }

    // Create local buffers and openCL buffer pointers
    for (int i = 0; i < numberTiles; i++) {

        this->tiles[i].buffer = new unsigned int[tilePixelsBufferSize];
        for (int j = 0; j < tilePixelsBufferSize; j++) {
            this->tiles[i].buffer[j] = 0;
        }

        this->tiles[i].bufferDepth = new float[tilePixelsBufferSize];
        for (int j = 0; j < tilePixelsBufferSize; j++) {
            this->tiles[i].bufferDepth[j] = 0;
        }
    }
}

void ComponentRender::drawWireframe(Triangle *t)
{
    Drawable::drawLine2D(Line2D(t->As.x, t->As.y, t->Bs.x, t->Bs.y), Color::green());
    Drawable::drawLine2D(Line2D(t->Bs.x, t->Bs.y, t->Cs.x, t->Cs.y), Color::green());
    Drawable::drawLine2D(Line2D(t->Cs.x, t->Cs.y, t->As.x, t->As.y), Color::green());
}

void ComponentRender::updateFPS()
{
    if (!EngineSetup::get()->DRAW_FPS) return;

    frameTime += Brakeza3D::get()->getDeltaTimeMicro();
    ++fpsFrameCounter;

    if (frameTime >= 1000.0f) {
        fps = fpsFrameCounter;
        frameTime -= 1000.0f;
        fpsFrameCounter = 0;
    }
}

void ComponentRender::updateLights()
{
    for (auto & lightpoint : this->lightPoints) {
        if (!lightpoint->isEnabled()) {
            continue;
        }
    }
}

Object3D* ComponentRender::getObject3DFromClickPoint(int xClick, int yClick, int &objectIndex)
{
    auto *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    Point2D  fixedPosition = Point2D(xClick,yClick);
    Vertex3D nearPlaneVertex = Transforms::Point2DToWorld(fixedPosition, camera);
    Vector3D ray(camera->getPosition(),nearPlaneVertex);

    int i = 0;
    for (auto o: Brakeza3D::get()->getSceneObjects()){
        auto mesh = dynamic_cast<Mesh3D*>(o);

        if (mesh == nullptr) continue;

        for (auto &triangle : mesh->getModelTriangles()) {
            auto *p = new Plane(triangle->Ao, triangle->Bo, triangle->Co);
            triangle->updateObjectSpace();
            float t;
            if (Maths::isVector3DClippingPlane(*p, ray)) {
                Vertex3D intersectionPoint  = p->getPointIntersection(ray.origin(), ray.end(), t);
                if (triangle->isPointInside(intersectionPoint)) {
                    objectIndex = i;
                    return triangle->parent;
                }
            }
        }
        i++;
    }

    return nullptr;
}

Object3D *ComponentRender::getSelectedObject() const
{
    return selectedObject;
}

void ComponentRender::setSelectedObject(Object3D *o) {
    this->selectedObject = o;
}

void ComponentRender::onPostUpdateSceneObjects()
{
    for (auto &object : Brakeza3D::get()->getSceneObjects()) {
        object->postUpdate();
    }
}

cl_device_id ComponentRender::selectDefaultGPUDevice()
{
    Logging::Message("Looking for GPU: %s", defaultGPU.c_str());

    cl_uint numPlatforms;
    cl_int ret = clGetPlatformIDs(0, nullptr, &numPlatforms);
    if (ret != CL_SUCCESS) {
        Logging::Log("Unable to get number of platforms");
        return nullptr;
    }

    std::vector<cl_platform_id> platforms(numPlatforms);
    ret = clGetPlatformIDs(numPlatforms, platforms.data(), nullptr);
    if (ret != CL_SUCCESS) {
        Logging::Log("Unable to get platform IDs");
        return nullptr;
    }

    for (const auto &platform : platforms) {
        cl_uint numDevices;
        ret = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &numDevices);
        if (ret != CL_SUCCESS || numDevices == 0) {
            continue;
        }

        std::vector<cl_device_id> devices(numDevices);
        ret = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices.data(), nullptr);
        if (ret != CL_SUCCESS) {
            continue;
        }

        for (const auto &device : devices) {
            char vendor[128];
            ret = clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(vendor), vendor, nullptr);
            if (ret != CL_SUCCESS) {
                continue;
            }

            if (std::string(vendor).find(defaultGPU) != std::string::npos) {
                clPlatformId = platform;
                return device;
            }
        }
    }

    return nullptr;
}

void ComponentRender::initOpenCL()
{
    OpenCLInfo();

    loadConfig();

    ret = clGetPlatformIDs(1, &clPlatformId, &ret_num_platforms) ;

    if (ret != CL_SUCCESS) {
        Logging::Log("Unable to get platform_id");
    }

    ret = clGetDeviceIDs(clPlatformId, CL_DEVICE_TYPE_GPU, 1, &clDeviceId, &ret_num_devices);
    if (ret != CL_SUCCESS) {
        Logging::Log("Unable to get cpu_device_id");
    }

    clDeviceId = selectDefaultGPUDevice();
    if (clDeviceId == nullptr) {
        Logging::Log("Unable to find a suitable NVIDIA device");
        return;
    }

    cl_context_properties properties[3];
    properties[0]= CL_CONTEXT_PLATFORM;
    properties[1]= (cl_context_properties) clPlatformId;
    properties[2]= 0;

    clContext = clCreateContext(properties, 1, &clDeviceId, NULL, NULL, &ret);
    clCommandQueue = clCreateCommandQueue(clContext, clDeviceId, NULL, &ret);

    EngineBuffers::get()->createOpenCLBuffers(clContext, clCommandQueue);

    // Get device information
    char vendor[128];
    char deviceName[128];
    ret = clGetDeviceInfo(clDeviceId, CL_DEVICE_VENDOR, sizeof(vendor), vendor, nullptr);
    if (ret != CL_SUCCESS) {
        Logging::Log("Unable to get device vendor");
    }

    ret = clGetDeviceInfo(clDeviceId, CL_DEVICE_NAME, sizeof(deviceName), deviceName, nullptr);
    if (ret != CL_SUCCESS) {
        Logging::Log("Unable to get device name");
    }

    // Print device information
    Logging::Message("Selected device vendor: %s", vendor);
    Logging::Message("Selected device name: %s", deviceName);

    loadCommonKernels();

    addShaderToScene(new ShaderImage(EngineSetup::get()->IMAGES_FOLDER + "cloud.png"));
    addShaderToScene(new ShaderDepthOfField(true));
    addShaderToScene(new ShaderBilinear(true));
    addShaderToScene(new ShaderParticlesBlurBuffer(true, 5));

    clBufferLights = clCreateBuffer(clContext, CL_MEM_READ_WRITE, MAX_OPENCL_LIGHTS * sizeof(OCLight), nullptr, nullptr);
    clBufferVideoParticles = clCreateBuffer(clContext, CL_MEM_READ_WRITE, EngineSetup::get()->RESOLUTION * sizeof(Uint32), nullptr, nullptr);
}

void ComponentRender::OpenCLInfo()
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
        Logging::Message("--");
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
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, NULL);
            printf(" %d.%d Parallel compute units: %d\n", j+1, 4, maxComputeUnits);
        }

        free(devices);
    }

    free(platforms);
}

std::vector<Tile> &ComponentRender::getTiles() {
    return tiles;
}

int ComponentRender::getTilesWidth() const {
    return numberTilesHorizontal;
}

int ComponentRender::getTilesHeight() const {
    return numberTilesVertical;
}

int ComponentRender::getNumTiles() const {
    return numberTiles;
}

int ComponentRender::getFps(){
    return fps;
}

_cl_platform_id *ComponentRender::getClPlatformId(){
    return clPlatformId;
}

_cl_device_id *ComponentRender::getClDeviceId() {
    return clDeviceId;
}

_cl_context *ComponentRender::getClContext() {
    return clContext;
}

_cl_command_queue *ComponentRender::getClCommandQueue() {
    return clCommandQueue;
}

ComponentRender::~ComponentRender()
{
    for (auto &l : lightPoints) {
        delete l;
    }

    for (auto &l : clippedTriangles) {
        delete l;
    }

    for (auto &l : visibleTriangles) {
        delete l;
    }

    for (auto &l : frameTriangles) {
        delete l;
    }

    for (auto s: sceneShaders) {
        delete s;
    }

    delete textWriter;
}

_cl_program *ComponentRender::getRendererProgram(){
    return rendererProgram;
}

_cl_kernel *ComponentRender::getRendererKernel() {
    return rendererKernel;
}

_cl_kernel *ComponentRender::getParticlesKernel() {
    return particlesKernel;
}

void ComponentRender::loadKernel(cl_program &program, cl_kernel &kernel, const std::string& source)
{
    size_t source_size;
    char * source_str = Tools::readFile(source, source_size );
    program = clCreateProgramWithSource(
        clContext,
        1,
        (const char **)&source_str,
        (const size_t *)&source_size,
        &ret
    );

    clBuildProgram(program, 1, &clDeviceId, nullptr, nullptr, nullptr);
    kernel = clCreateKernel(program, "onUpdate", &ret);

    free(source_str);
}

_cl_kernel *ComponentRender::getExplosionKernel()
{
    return explosionKernel;
}


_cl_kernel *ComponentRender::getBlinkKernel()
{
    return blinkKernel;
}

cl_mem *ComponentRender::getClBufferLights() {
    return &clBufferLights;
}

cl_mem *ComponentRender::getClBufferVideoParticles()
{
    return &clBufferVideoParticles;
}

void ComponentRender::loadConfig()
{
    Logging::Message("Loading setup.json...");

    const std::string filePath = EngineSetup::get()->CONFIG_FOLDER + "setup.json";

    size_t file_size;
    auto contentFile = Tools::readFile(filePath, file_size);

    cJSON *myDataJSON = cJSON_Parse(contentFile);

    if (myDataJSON == nullptr) {
        Logging::Message("[Load Config] Can't be loaded: %s", filePath.c_str());
        exit(-1);
    }

    defaultGPU = cJSON_GetObjectItemCaseSensitive(myDataJSON, "gpu")->valuestring;
}

void ComponentRender::loadCommonKernels()
{
    Logging::Message("Loading common OpenCL kernels");

    loadKernel(rendererProgram, rendererKernel, EngineSetup::get()->CL_SHADERS_FOLDER + "renderer.cl");
    loadKernel(fragmentsProgram, fragmentsKernel, EngineSetup::get()->CL_SHADERS_FOLDER + "fragments.cl");
    loadKernel(rasterizeProgram, rasterizeKernel, EngineSetup::get()->CL_SHADERS_FOLDER + "rasterizer.cl");


    loadKernel(particlesProgram, particlesKernel, EngineSetup::get()->CL_SHADERS_FOLDER + "particles.cl");
    loadKernel(explosionProgram, explosionKernel, EngineSetup::get()->CL_SHADERS_FOLDER + "explosion.cl");
    loadKernel(blinkProgram, blinkKernel, EngineSetup::get()->CL_SHADERS_FOLDER + "blink.opencl");
    loadKernel(edgeProgram, edgeKernel, EngineSetup::get()->CL_SHADERS_FOLDER + "edge.opencl");

    clBufferFragments = clCreateBuffer(clContext, CL_MEM_WRITE_ONLY, EngineBuffers::get()->sizeBuffers * sizeof(OCFragment), nullptr, nullptr );
}

EngineSetup::LuaStateScripts ComponentRender::getStateLUAScripts() {
    return stateScripts;
}

void ComponentRender::playLUAScripts()
{
    Logging::Message("LUA Scripts state changed to PLAY");

    onStartScripts();

    stateScripts = EngineSetup::LuaStateScripts::LUA_PLAY;
}

void ComponentRender::stopLUAScripts()
{
    Logging::Message("LUA Scripts state changed to STOP");

    stateScripts = EngineSetup::LuaStateScripts::LUA_STOP;

    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        object->reloadScriptsEnvironment();
    }

    reloadScriptGlobals();

    Logging::Message("Removing objects creating by LUA...");
    for (auto object : Brakeza3D::get()->getSceneObjects()) {
        if (!object->isBelongToScene()) {
            object->setRemoved(true);
        }
    }
}

void ComponentRender::reloadLUAScripts()
{
    Logging::Message("Reloading LUA Scripts...");

    auto &lua = EngineBuffers::get()->getLua();
    lua.collect_garbage();

    for(auto script : scripts) {
        script->reloadScriptCode();
    }

    auto &sceneObjects = Brakeza3D::get()->getSceneObjects();
    for (auto object : sceneObjects) {
        object->reloadScriptsCode();
        object->reloadScriptsEnvironment();
    }

    reloadScriptGlobals();
}

std::vector<ScriptLUA*> &ComponentRender::getLUAScripts()
{
    return scripts;
}

void ComponentRender::addLUAScript(ScriptLUA *script)
{
    scripts.push_back(script);
    reloadScriptGlobals();
}

void ComponentRender::runScripts()
{
    for(auto script : scripts) {
        script->runGlobal("onUpdate");
    }
}

void ComponentRender::reloadScriptGlobals()
{
    for (auto script : scripts) {
        script->reloadGlobals();
    }
}

void ComponentRender::removeScript(ScriptLUA *script)
{
    Logging::Message("Removing game script %s", script->scriptFilename.c_str());

    for (auto it = scripts.begin(); it != scripts.end(); ++it) {
        if ((*it)->scriptFilename == script->scriptFilename) {
            delete *it;
            scripts.erase(it);
            return;
        }
    }
}

void ComponentRender::onStartScripts()
{
    Logging::Message("Executing OnStart for project scripts...");

    for (auto script : scripts) {
        script->runGlobal("onStart");
    }

    auto &sceneObjects = Brakeza3D::get()->getSceneObjects();
    for (auto object : sceneObjects) {
        object->runStartScripts();
    }
}

SceneLoader &ComponentRender::getSceneLoader() {
    return sceneLoader;
}

std::vector<ShaderOpenCL *> &ComponentRender::getSceneShaders() {
    return sceneShaders;
}

ShaderOpenCL *ComponentRender::getSceneShaderByIndex(int i) {
    return sceneShaders[i];
}

void ComponentRender::addShaderToScene(ShaderOpenCL *shader)
{
    sceneShaders.push_back(shader);
}

bool ComponentRender::isSceneShadersEnabled() const {
    return sceneShadersEnabled;
}

void ComponentRender::setSceneShadersEnabled(bool sceneShadersEnabled) {
    ComponentRender::sceneShadersEnabled = sceneShadersEnabled;
}

void ComponentRender::runShadersOpenCLPostUpdate() {
    for( auto s: sceneShaders) {
        if (!s->isEnabled()) continue;
        s->postUpdate();
    }
}

void ComponentRender::runShadersOpenCLPreUpdate() {
    for( auto s: sceneShaders) {
        if (!s->isEnabled()) continue;
        s->preUpdate();
    }
}

void ComponentRender::removeShader(int index) {
    if (index >= 0 && index < sceneShaders.size()) {
        sceneShaders.erase(sceneShaders.begin() + index);
    }
}

_cl_kernel *ComponentRender::getEdgeKernel() {
    return edgeKernel;
}

_cl_kernel *ComponentRender::getFragmentsKernel(){
    return fragmentsKernel;
}

_cl_kernel *ComponentRender::getRasterizeKernel() {
    return rasterizeKernel;
}
