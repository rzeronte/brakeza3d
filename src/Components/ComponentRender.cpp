#include "../../include/Components/ComponentRender.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"
#include "../../include/OpenGL/ShaderOpenGLCustomPostprocessing.h"
#include "../../include/OpenGL/ShaderOpenGLCustomMesh3D.h"
#include "../../include/OpenGL/ShaderOGLShadowPass.h"
#include "../../include/Render/Transforms.h"

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
    shaderOGLBonesTransforms(nullptr),
    shaderOGLGBuffer(nullptr),
    shaderOGLLightPass(nullptr),
    shaderShadowPass(nullptr),
    shaderShadowPassDebugLight(nullptr),
    lastFrameBufferUsed(0),
    lastProgramUsed(0),
    shadowMapArrayTex(0)
{
}

void ComponentRender::onStart()
{
    Logging::Message("ComponentRender onStart");

    setEnabled(true);

    auto window = ComponentsManager::get()->getComponentWindow();
    textWriter = new TextWriter(window->getRenderer(),window->getFontDefault());

    shaderOGLRender = new ShaderOGLRenderForward();
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
    shaderOGLBonesTransforms = new ShaderOpenGLBonesTransforms();
    shaderOGLGBuffer = new ShaderOGLGRenderDeferred();
    shaderOGLLightPass = new ShaderOGLLightPass();
    shaderShadowPass = new ShaderOGLShadowPass();
    shaderShadowPassDebugLight = new ShaderOGLShadowPassDebugLight();
}

void ComponentRender::preUpdate()
{
    clearShadowMaps();

    this->updateFPS();

    if (!isEnabled()) return;

    deleteRemovedObjects();
}

void ComponentRender::onUpdate()
{
    if (!isEnabled()) return;

    getShaderOGLRenderForward()->createUBOFromLights();

    auto shaderRender = getShaderOGLRenderForward();
    auto numSpotLights = static_cast<int>(shaderRender->getShadowMappingSpotLights().size());

    if (EngineSetup::get()->ENABLE_SHADOW_MAPPING) {
        static int lastNumLights = -1;
        if (numSpotLights != lastNumLights) {
            createSpotLightsDepthTextures(numSpotLights);
            getShaderOGLShadowPass()->setupFBOSpotLights();
            lastNumLights = numSpotLights;
        }
    }

    onUpdateSceneObjects();

    if (Brakeza3D::get()->getManagerGui()->isShowLightsDepthMapsViewerWindow() ) {
        shaderShadowPassDebugLight->createFramebuffer();
        shaderShadowPassDebugLight->createArrayTextures(numSpotLights);
        shaderShadowPassDebugLight->updateDebugTextures(numSpotLights);
    }

    if (SETUP->RENDER_MAIN_AXIS) {
        Drawable::drawMainAxis();
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
    if (SETUP->MOUSE_CLICK_SELECT_OBJECT3D) {
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

    for (const auto o: sceneObjects) {
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

    Object3D *foundObject = nullptr;
    float lastDepthFound = -1;
    for (auto o: Brakeza3D::get()->getSceneObjects()) {
        o->checkClickObject(ray, foundObject, lastDepthFound);
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
        case SHADER_POSTPROCESSING : {
            return new ShaderOpenGLCustomPostprocessing(name, shaderVertexFile, shaderFragmentFile);
        }
        default:
        case SHADER_OBJECT : {
            return new ShaderOpenGLCustomMesh3D(nullptr, name, shaderVertexFile, shaderFragmentFile);
        }
    }
}

void ComponentRender::loadShaderIntoScene(const std::string &folder, const std::string &jsonFilename)
{
    auto name = Tools::getFilenameWithoutExtension(jsonFilename);

    std::string shaderFragmentFile = folder + std::string(name + ".fs");
    std::string shaderVertexFile = folder + std::string(name + ".vs");

    auto type = ShaderOpenGLCustom::extractTypeFromShaderName(folder, name);

    Logging::Message("LoadShaderInto Scene: Folder: %s, Name: %s, Type: %d", folder.c_str(), name.c_str(), type);

    switch(type) {
        case SHADER_POSTPROCESSING : {
            this->addShaderToScene(
                new ShaderOpenGLCustomPostprocessing(name, shaderVertexFile, shaderFragmentFile)
            );
            break;
        }
        default:
        case SHADER_OBJECT : {
            Logging::Message("[error] You can't add a ShaderObject type into scene");
            break;
        }
    }
}

void ComponentRender::addShaderToScene(ShaderOpenGLCustom *shader)
{
    sceneShaders.push_back(shader);
}

bool ComponentRender::isSceneShadersEnabled() const
{
    return sceneShadersEnabled;
}

void ComponentRender::setSceneShadersEnabled(bool value)
{
    sceneShadersEnabled = value;
}

void ComponentRender::runShadersOpenGLPostUpdate()
{
    for( auto s: sceneShaders) {
        if (!s->isEnabled()) continue;
        s->postUpdate();
    }
}

void ComponentRender::runShadersOpenGLPreUpdate()
{
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
        if (*it == shader) {
            delete *it;
            sceneShaders.erase(it);
            return;
        }
    }
}

bool ComponentRender::compareDistances(const Object3D* obj1, const Object3D* obj2)
{
    return obj1->getDistanceToCamera() > obj2->getDistanceToCamera();
}

void ComponentRender::setGlobalIlluminationDirection(Vertex3D v) const
{
    getShaderOGLRenderForward()->setGlobalIlluminationDirection(v);
}

void ComponentRender::setGlobalIlluminationAmbient(Vertex3D v) const
{
    getShaderOGLRenderForward()->setGlobalIlluminationAmbient(v);
}

void ComponentRender::setGlobalIlluminationDiffuse(Vertex3D v) const
{
    getShaderOGLRenderForward()->setGlobalIlluminationDiffuse(v);
}

void ComponentRender::setGlobalIlluminationSpecular(Vertex3D v) const
{
    getShaderOGLRenderForward()->setGlobalIlluminationSpecular(v);
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

ShaderOGLRenderForward *ComponentRender::getShaderOGLRenderForward() const {
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

ShaderOpenGLBonesTransforms *ComponentRender::getShaderOGLBonesTransforms() const {
    return shaderOGLBonesTransforms;
}

ShaderOGLShadowPass *ComponentRender::getShaderOGLShadowPass() const {
    return shaderShadowPass;
}
ShaderOGLShadowPassDebugLight *ComponentRender::getShaderOGLShadowPassDebugLight() const {
    return shaderShadowPassDebugLight;
}

ShaderOpenGLDepthMap *ComponentRender::getShaderOGLDepthMap() const {
    return shaderOGLDepthMap;
}

ShaderOGLGRenderDeferred *ComponentRender::getShaderOGLRenderDeferred() const {
    return shaderOGLGBuffer;
}

ShaderOGLLightPass *ComponentRender::getShaderOGLLightPass() const {
    return shaderOGLLightPass;
}

GLuint ComponentRender::getLastFrameBufferUsed() const {
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

void ComponentRender::resizeFramebuffers()
{
    Logging::Message("Resizing framebuffers...");

    getShaderOGLRenderForward()->destroy();
    getShaderOGLImage()->destroy();
    getShaderOGLLine()->destroy();
    getShaderOGLWireframe()->destroy();
    getShaderOGLShading()->destroy();
    getShaderOGLPoints()->destroy();
    getShaderOGLOutline()->destroy();
    getShaderOGLColor()->destroy();
    getShaderOGLParticles()->destroy();
    getShaderOGLDOF()->destroy();
    getShaderOGLDepthMap()->destroy();
    getShaderOGLFOG()->destroy();
    getShaderOGLRenderDeferred()->destroy();
    getShaderOGLLightPass()->destroy();

    createSpotLightsDepthTextures(static_cast<int>(getShaderOGLRenderForward()->getShadowMappingSpotLights().size()));
    getShaderOGLShadowPass()->setupFBOSpotLights();
    getShaderOGLShadowPass()->createDirectionalLightDepthTexture();
    getShaderOGLShadowPass()->setupFBODirectionalLight();

    for (auto s: sceneShaders) {
        s->destroy();
    }
}

void ComponentRender::FillOGLBuffers(std::vector<meshData> &meshes)
{
    for (auto &m: meshes) {
        glGenBuffers(1, &m.vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m.vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(glm::vec4), &m.vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m.uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m.uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, m.uvs.size() * sizeof(glm::vec2), &m.uvs[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m.normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m.normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, m.normals.size() * sizeof(glm::vec3), &m.normals[0], GL_STATIC_DRAW);

        // Creamos el buffer para Transform Feedback de vértices
        glGenBuffers(1, &m.feedbackBuffer);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m.feedbackBuffer);  // Vinculamos el buffer de feedback
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, m.vertices.size() * sizeof(glm::vec4), &m.vertices[0], GL_DYNAMIC_COPY);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);  // Desvinculamos el buffer
    }
}

void ComponentRender::clearShadowMaps()
{
    auto numLights = static_cast<int>(getShaderOGLRenderForward()->getShadowMappingSpotLights().size());
    if (numLights > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, getShaderOGLShadowPass()->getSpotLightsDepthMapsFBO());
        glClear(GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < numLights; i++) {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, getSpotLightsShadowMapArrayTextures(), 0, i);
            glClear(GL_DEPTH_BUFFER_BIT);
        }
    }
}

GLuint ComponentRender::getSpotLightsShadowMapArrayTextures() const {
    return shadowMapArrayTex;
}

void ComponentRender::createSpotLightsDepthTextures(int numLights)
{
    auto window = ComponentsManager::get()->getComponentWindow();

    int w, h;
    SDL_GetRendererOutputSize(window->getRenderer(), &w, &h);

    glGenTextures(1, &shadowMapArrayTex);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArrayTex);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32, w, h, numLights,0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Configuración
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
}
