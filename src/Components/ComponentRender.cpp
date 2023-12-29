
#include "../../include/Components/ComponentRender.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Render/Transforms.h"

ComponentRender::ComponentRender()
:
    textWriter(nullptr),
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
    addShaderToScene(new FXColorTint(false, Color::red(), 0));
}

void ComponentRender::preUpdate()
{
    this->updateFPS();

}

void ComponentRender::onUpdate()
{
    if (!isEnabled()) return;

    deleteRemovedObjects();
    if (isSceneShadersEnabled()) {
        runShadersOpenCLPreUpdate();
    }

    onUpdateSceneObjects();

    if (SETUP->RENDER_MAIN_AXIS) {
        Drawable::drawMainAxis(ComponentsManager::get()->getComponentCamera()->getCamera());
    }

    if (SETUP->ENABLE_LIGHTS) {
    }

    if (stateScripts == EngineSetup::LUA_PLAY) {
        runScripts();
    }
    if (isSceneShadersEnabled()) {
        runShadersOpenCLPostUpdate();
    }

    if (SETUP->DRAW_FPS) {
        textWriter->writeTTFCenterHorizontal(
            10,
            std::to_string(getFps()).c_str(),
            Color::green(),
            0.3
        );
    }
}

void ComponentRender::postUpdate()
{
    for (auto &object : Brakeza3D::get()->getSceneObjects()) {
        object->postUpdate();
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
    auto cameraPosition = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition();

    std::map<float, Object3D*> sorted;
    for (auto & sceneObject : sceneObjects) {
        float distance = cameraPosition.distance(sceneObject->position);
        sorted[distance] = sceneObject;
        if (sceneObject->isEnabled()) sceneObject->onUpdate();
    }
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

int ComponentRender::getFps() const{
    return fps;
}

ComponentRender::~ComponentRender()
{
    for (auto s: sceneShaders) {
        delete s;
    }

    delete textWriter;
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

    auto &lua = LUAManager::get()->getLua();
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

std::vector<FXEffectOpenGL *> &ComponentRender::getSceneShaders() {
    return sceneShaders;
}

FXEffectOpenGL *ComponentRender::getSceneShaderByIndex(int i) {
    return sceneShaders[i];
}

void ComponentRender::addShaderToScene(FXEffectOpenGL *shader)
{
    sceneShaders.push_back(shader);
}

bool ComponentRender::isSceneShadersEnabled() const {
    return sceneShadersEnabled;
}

void ComponentRender::setSceneShadersEnabled(bool sceneShadersEnabled) {
    ComponentRender::sceneShadersEnabled = sceneShadersEnabled;
}

void ComponentRender::runShadersOpenCLPostUpdate()
{
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
