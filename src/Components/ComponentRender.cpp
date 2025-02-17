
#include "../../include/Components/ComponentRender.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"
#include "../../include/OpenGL/ShaderOpenGLCustomPostprocessing.h"
#include "../../include/OpenGL/ShaderOpenGLCustomMesh3D.h"

ComponentRender::ComponentRender()
:
    fps(0),
    fpsFrameCounter(0),
    frameTime(0),
    selectedObject(nullptr),
    textWriter(nullptr),
    sceneShadersEnabled(true),
    shaderOGLRender(nullptr),
    shaderOGLImage(nullptr),
    shaderOGLLine(nullptr),
    shaderOGLWireframe(nullptr),
    shaderOGLLine3D(nullptr),
    shaderOGLShading(nullptr),
    shaderOGLPoints(nullptr),
    shaderOGLOutline(nullptr),
    shaderOGLColor(nullptr),
    shaderOGLParticles(nullptr),
    shaderOGLDOF(nullptr),
    shaderOGLDepthMap(nullptr),
    shaderOGLFOG(nullptr),
    shaderOGLTint(nullptr),
    lastFrameBufferUsed(0),
    lastProgramUsed(0)
{
}

void ComponentRender::onStart()
{
    Logging::Message("ComponentRender onStart");

    setEnabled(true);

    auto window = ComponentsManager::get()->getComponentWindow();
    textWriter = new TextWriter(window->getRenderer(),window->getFontDefault());

    shaderOGLRender = new ShaderOpenGLRender();
    shaderOGLImage = new ShaderOpenGLImage();
    shaderOGLLine = new ShaderOpenGLLine();
    shaderOGLWireframe = new ShaderOpenGLWireframe();
    shaderOGLLine3D = new ShaderOpenGLLine3D();
    shaderOGLShading = new ShaderOpenGLShading();
    shaderOGLPoints = new ShaderOpenGLPoints();
    shaderOGLOutline = new ShaderOpenGLOutline();
    shaderOGLColor = new ShaderOpenGLColor();
    shaderOGLParticles = new ShaderOpenGLParticles();
    shaderOGLDOF = new ShaderOpenGLDOF();
    shaderOGLDepthMap = new ShaderOpenGLDepthMap();
    shaderOGLFOG = new ShaderOpenGLFOG();
    shaderOGLTint = new ShaderOpenGLTint();
}

void ComponentRender::preUpdate()
{
    this->updateFPS();

    if (!isEnabled()) return;

    deleteRemovedObjects();
}

void ComponentRender::onUpdate()
{
    if (!isEnabled()) return;

    getShaderOGLRender()->createUBOFromLights();

    if (isSceneShadersEnabled()) {
        //runShadersOpenCLPreUpdate();
    }

    onUpdateSceneObjects();

    if (SETUP->RENDER_MAIN_AXIS) {
        Drawable::drawMainAxis();
    }

    if (isSceneShadersEnabled()) {
        runShadersOpenGLPostUpdate();
    }

    if (SETUP->DRAW_FPS) {
        textWriter->writeTextTTFMiddleScreen(
            std::to_string(getFps()).c_str(),
            Color::green(),
            2.5
        );
    }
}

void ComponentRender::postUpdate()
{
    std::vector<Object3D *> sceneObjects = Brakeza3D::get()->getSceneObjects();

    std::sort(sceneObjects.begin(), sceneObjects.end(), compareDistances);

    for (auto o: sceneObjects) {
        if (o->isEnabled()) {
            o->postUpdate();
        }
    }
}

void ComponentRender::onEnd()
{
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

    if (!input->isEnabled()) return;

    if (input->isClickLeft() && !input->isMouseMotion()) {
        selectedObject = getObject3DFromClickPoint(
            input->getRelativeRendererMouseX(),
            input->getRelativeRendererMouseY()
        );

        if (selectedObject != nullptr) {
            Logging::Message("Selected object by click: %s", selectedObject->getLabel().c_str());
            Brakeza3D::get()->getManagerGui()->setSelectedObject(selectedObject);
        } else {
            setSelectedObject(nullptr);
        }
    }
}

void ComponentRender::deleteRemovedObjects()
{
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
    std::vector<Object3D *> sceneObjects = Brakeza3D::get()->getSceneObjects();

    std::sort(sceneObjects.begin(), sceneObjects.end(), compareDistances);

    for (auto o: sceneObjects) {
        if (o->isEnabled()) {
            o->onUpdate();
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

Object3D* ComponentRender::getObject3DFromClickPoint(int xClick, int yClick)
{
    auto *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    Point2D  fixedPosition = Point2D(xClick,yClick);
    Vertex3D nearPlaneVertex = Transforms::Point2DToWorld(fixedPosition);
    Vector3D ray(camera->getPosition(),nearPlaneVertex);

    int i = 0;
    Object3D *foundObject = nullptr;
    float lastDepthFound = -1;
    for (auto o: Brakeza3D::get()->getSceneObjects()) {
        auto mesh = dynamic_cast<Mesh3D*>(o);

        if (mesh == nullptr) continue;
        for (auto m: mesh->meshes) {
            for (auto &triangle : m.modelTriangles) {
                triangle->updateObjectSpace();
                auto p = new Plane(triangle->Ao, triangle->Bo, triangle->Co);
                float t;
                if (Maths::isVector3DClippingPlane(*p, ray)) {
                    Vertex3D intersectionPoint  = p->getPointIntersection(ray.origin(), ray.end(), t);
                    if (triangle->isPointInside(intersectionPoint)) {
                        auto distance = intersectionPoint - camera->getPosition();
                        auto m = distance.getModule();
                        if ( m < lastDepthFound || lastDepthFound == -1) {
                            foundObject = triangle->parent;
                            lastDepthFound = m;
                        }
                    }
                }
            }
            i++;

        }
    }

    return foundObject;
}

Object3D *ComponentRender::getSelectedObject() const
{
    return selectedObject;
}

void ComponentRender::setSelectedObject(Object3D *o)
{
    this->selectedObject = o;
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


SceneLoader &ComponentRender::getSceneLoader()
{
    return sceneLoader;
}

ProjectLoader &ComponentRender::getProjectLoader()
{
    return projectLoader;
}

std::vector<ShaderOpenGLCustom *> &ComponentRender::getSceneShaders() {
    return sceneShaders;
}

ShaderOpenGLCustom *ComponentRender::getSceneShaderByIndex(int i) {
    return sceneShaders[i];
}

ShaderOpenGLCustom *ComponentRender::getSceneShaderByLabel(const std::string& name)
{
    for (auto &s: sceneShaders) {
        if (s->getLabel() == name) {
            return s;
        }
    }

    return nullptr;
}

ShaderOpenGLCustom* ComponentRender::getLoadedShader(std::string folder, std::string jsonFilename)
{
    auto name = Tools::getFilenameWithoutExtension(jsonFilename);

    std::string shaderFragmentFile = folder + std::string(name + ".fs");
    std::string shaderVertexFile = folder + std::string(name + ".vs");

    auto type = ShaderOpenGLCustom::extractTypeFromShaderName(folder, name);

    Logging::Message("LoadShaderInto Scene: Folder: %s, Name: %s, Type: %d", folder.c_str(), name.c_str(), type);

    switch(type) {
        case ShaderCustomTypes::SHADER_POSTPROCESSING : {
            return new ShaderOpenGLCustomPostprocessing(name, shaderVertexFile, shaderFragmentFile);
        }
        case ShaderCustomTypes::SHADER_OBJECT : {
            return new ShaderOpenGLCustomMesh3D(nullptr, name, shaderVertexFile, shaderFragmentFile);
            break;
        }
    }
}

void ComponentRender::loadShaderIntoScene(std::string folder, std::string jsonFilename)
{
    auto name = Tools::getFilenameWithoutExtension(jsonFilename);

    std::string shaderFragmentFile = folder + std::string(name + ".fs");
    std::string shaderVertexFile = folder + std::string(name + ".vs");

    auto type = ShaderOpenGLCustom::extractTypeFromShaderName(folder, name);

    Logging::Message("LoadShaderInto Scene: Folder: %s, Name: %s, Type: %d", folder.c_str(), name.c_str(), type);

    switch(type) {
        case ShaderCustomTypes::SHADER_POSTPROCESSING : {
            this->addShaderToScene(
                new ShaderOpenGLCustomPostprocessing(name, shaderVertexFile, shaderFragmentFile)
            );
            break;
        }
        case ShaderCustomTypes::SHADER_OBJECT : {
            Logging::Message("[error] You can't add a ShaderObject type into scene");
            break;
        }
    }
}

void ComponentRender::addShaderToScene(ShaderOpenGLCustom *shader)
{
    sceneShaders.push_back(shader);
}

bool ComponentRender::isSceneShadersEnabled() const {
    return sceneShadersEnabled;
}

void ComponentRender::setSceneShadersEnabled(bool sceneShadersEnabled) {
    ComponentRender::sceneShadersEnabled = sceneShadersEnabled;
}

void ComponentRender::runShadersOpenGLPostUpdate()
{
    for( auto s: sceneShaders) {
        if (!s->isEnabled()) continue;
        s->postUpdate();
    }
}

void ComponentRender::runShadersOpenCLPreUpdate() {
    for( auto s: sceneShaders) {
        if (!s->isEnabled()) continue;
        s->onUpdate();
    }
}

void ComponentRender::removeSceneShaderByIndex(int index) {
    if (index >= 0 && index < sceneShaders.size()) {
        sceneShaders.erase(sceneShaders.begin() + index);
    }
}

void ComponentRender::removeSceneShader(ShaderOpenGLCustom *shader)
{
    Logging::Message("Removing SCENE script %s", shader->getLabel().c_str());

    for (auto it = sceneShaders.begin(); it != sceneShaders.end(); ++it) {
        if ((*it) == shader) {
            delete *it;
            sceneShaders.erase(it);
            return;
        }
    }
}

bool ComponentRender::compareDistances(Object3D* obj1, Object3D* obj2)
{
    return obj1->getDistanceToCamera() > obj2->getDistanceToCamera();
}

void ComponentRender::setGlobalIlluminationDirection(Vertex3D v) const
{
    getShaderOGLRender()->setGlobalIlluminationDirection(v);
}

void ComponentRender::setGlobalIlluminationAmbient(Vertex3D v) const
{
    getShaderOGLRender()->setGlobalIlluminationAmbient(v);
}

void ComponentRender::setGlobalIlluminationDiffuse(Vertex3D v) const
{
    getShaderOGLRender()->setGlobalIlluminationDiffuse(v);
}

void ComponentRender::setGlobalIlluminationSpecular(Vertex3D v) const
{
    getShaderOGLRender()->setGlobalIlluminationSpecular(v);
}

void ComponentRender::drawLine(Vertex3D from, Vertex3D to, Color c) const
{
    getShaderOGLLine3D()->render(
        from,
        to,
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer(),
        c
    );
}

ShaderOpenGLImage *ComponentRender::getShaderOGLImage() const {
    return shaderOGLImage;
}

ShaderOpenGLLine3D *ComponentRender::getShaderOGLLine3D() const {
    return shaderOGLLine3D;
}

ShaderOpenGLRender *ComponentRender::getShaderOGLRender() const {
    return shaderOGLRender;
}

ShaderOpenGLLine *ComponentRender::getShaderOGLLine() const {
    return shaderOGLLine;
}

ShaderOpenGLWireframe *ComponentRender::getShaderOGLWireframe() const {
    return shaderOGLWireframe;
}

ShaderOpenGLShading *ComponentRender::getShaderOGLShading() const {
    return shaderOGLShading;
}

ShaderOpenGLPoints *ComponentRender::getShaderOGLPoints() const {
    return shaderOGLPoints;
}

ShaderOpenGLDOF *ComponentRender::getShaderOGLDOF() const {
    return shaderOGLDOF;
}

ShaderOpenGLOutline *ComponentRender::getShaderOGLOutline() const {
    return shaderOGLOutline;
}

ShaderOpenGLColor *ComponentRender::getShaderOGLColor() const {
    return shaderOGLColor;
}

ShaderOpenGLParticles *ComponentRender::getShaderOGLParticles() const {
    return shaderOGLParticles;
}

ShaderOpenGLFOG *ComponentRender::getShaderOGLFOG() const {
    return shaderOGLFOG;
}

ShaderOpenGLTint *ComponentRender::getShaderOGLTint() const {
    return shaderOGLTint;
}

ShaderOpenGLDepthMap *ComponentRender::getShaderOGLDepthMap() const {
    return shaderOGLDepthMap;
}

GLuint ComponentRender::getLastFrameBufferUsed() {
    return lastFrameBufferUsed;
}

void ComponentRender::setLastFrameBufferUsed(GLuint lastFrameBufferUsed) {
    ComponentRender::lastFrameBufferUsed = lastFrameBufferUsed;
}

GLuint ComponentRender::getLastProgramUsed() const {
    return lastProgramUsed;
}

void ComponentRender::setLastProgramUsed(GLuint lastProgramUsed) {
    ComponentRender::lastProgramUsed = lastProgramUsed;
}

void ComponentRender::changeOpenGLFramebuffer(GLuint framebuffer)
{
    if (getLastFrameBufferUsed() != framebuffer || framebuffer == 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        setLastFrameBufferUsed(framebuffer);
    }
}

void ComponentRender::changeOpenGLProgram(GLuint programID)
{
    if (getLastProgramUsed() != programID) {
        glUseProgram(programID);
        setLastProgramUsed(programID);
    }
}

const std::map<std::string, ShaderCustomTypes> &ComponentRender::getShaderTypesMapping() const
{
    return ShaderTypesMapping;
}
