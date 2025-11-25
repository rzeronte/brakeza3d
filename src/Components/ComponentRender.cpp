#include "../../include/Components/ComponentRender.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Brakeza.h"
#include "../../include/OpenGL/ShaderOGLCustomPostprocessing.h"
#include "../../include/OpenGL/ShaderOGLCustomMesh3D.h"
#include "../../include/OpenGL/ShaderOGLShadowPass.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Render/Drawable.h"

ComponentRender::ComponentRender()
{
}

ComponentRender::~ComponentRender()
{
    for (auto s: sceneShaders) {
        delete s;
    }

    delete textWriter;
}

void ComponentRender::onStart()
{
    Logging::Message("ComponentRender onStart");

    setEnabled(true);

    auto window = ComponentsManager::get()->getComponentWindow();
    textWriter = new TextWriter(window->getRenderer(),window->getFontDefault());

    shaderOGLRender = new ShaderOGLRenderForward();
    shaderOGLImage = new ShaderOGLImage();
    shaderOGLLine = new ShaderOGLLine();
    shaderOGLWireframe = new ShaderOGLWire();
    shaderOGLLine3D = new ShaderOGLLine3D();
    shaderOGLShading = new ShaderOGLShading();
    shaderOGLPoints = new ShaderOGLPoints();
    shaderOGLOutline = new ShaderOGLOutline();
    shaderOGLColor = new ShaderOGLColor();
    shaderOGLParticles = new ShaderOGLParticles();
    shaderOGLDOFBlur = new ShaderOGLDOF();
    shaderOGLDepthMap = new ShaderOGLDepthMap();
    shaderOGLFOG = new ShaderOGLFog();
    shaderOGLTint = new ShaderOGLTint();
    shaderOGLBonesTransforms = new ShaderOGLBonesTransforms();
    shaderOGLGBuffer = new ShaderOGLRenderDeferred();
    shaderOGLLightPass = new ShaderOGLLightPass();
    shaderShadowPass = new ShaderOGLShadowPass();
    shaderShadowPassDebugLight = new ShaderOGLShadowPassDebugLight();
}

void ComponentRender::preUpdate()
{
    ClearShadowMaps();

    this->updateFPS();

    if (!isEnabled()) return;

    deleteRemovedObjects();
}

void ComponentRender::onUpdate()
{
    if (!isEnabled()) return;

    shaderOGLRender->createUBOFromLights();

    auto numSpotLights = shaderOGLRender->getNumSpotLights();

    if (BrakezaSetup::get()->ENABLE_LIGHTS) {
        if (BrakezaSetup::get()->ENABLE_SHADOW_MAPPING) {
            if (shaderOGLRender->hasSpotLightsChanged()) {
                Logging::Message("Updating shadow maps for %d lights", numSpotLights);
                shaderShadowPass->createSpotLightsDepthTextures(numSpotLights);
                shaderShadowPass->setupFBOSpotLights();
                shaderOGLRender->setLastSpotLightsSize(numSpotLights);
            }
        }
    }

    onUpdateSceneObjects();

    if (SETUP->DRAW_FPS_RENDER) {
        textWriter->writeTextTTFMiddleScreen(
            std::to_string(getFps()).c_str(),
            Color::green(),
            1.5
        );
    }

    if (Brakeza::get()->getManagerGui()->isShowLightsDepthMapsViewerWindow() ) {
        shaderShadowPassDebugLight->createFramebuffer();
        shaderShadowPassDebugLight->createArrayTextures(numSpotLights);
        shaderShadowPassDebugLight->updateDebugTextures(numSpotLights);
    }
}

void ComponentRender::postUpdate()
{
    std::vector<Object3D *> sceneObjects = Brakeza::get()->getSceneObjects();

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
}

void ComponentRender::onUpdateSceneObjects()
{
    std::vector<Object3D *> sceneObjects = Brakeza::get()->getSceneObjects();

    std::sort(sceneObjects.begin(), sceneObjects.end(), compareDistances);

    for (const auto o: sceneObjects) {
        if (o->isEnabled()) {
            o->onUpdate();
        }
    }
}

void ComponentRender::updateFPS()
{
    if (!(BrakezaSetup::get()->DRAW_FPS_IMGUI || BrakezaSetup::get()->DRAW_FPS_RENDER)) return;

    frameTime += Brakeza::get()->getDeltaTimeMicro();
    ++fpsFrameCounter;

    if (frameTime >= 1000.0f) {
        fps = fpsFrameCounter;
        frameTime -= 1000.0f;
        fpsFrameCounter = 0;
    }
}

Object3D *ComponentRender::getSelectedObject() const
{
    return selectedObject;
}

void ComponentRender::setSelectedObject(Object3D *o)
{
    this->selectedObject = o;
}

Object3D* ComponentRender::getObject3DFromClickPoint(const int x, const int y)
{
    const auto id = ComponentsManager::get()->getComponentWindow()->getObjectIDByPickingColorFramebuffer(x, y);

    Logging::Message("Click point: %d, %d | ObjectId: %d", x, y, id);
    return Brakeza::get()->getSceneObjectById(id);
}

void ComponentRender::updateSelectedObject3D()
{
    auto input = ComponentsManager::get()->getComponentInput();

    if (!input->isEnabled()) return;

    if (input->isClickLeft() && !input->isMouseMotion()) {
        auto x = input->getMouseX();
        auto y = input->getMouseY();
        const auto id = ComponentsManager::get()->getComponentWindow()->getObjectIDByPickingColorFramebuffer(x, y);
        selectedObject = Brakeza::get()->getSceneObjectById(id);
        if (selectedObject != nullptr) {
            Logging::Message("Selected object by click(%d, %d): %s", x, y, selectedObject->getLabel().c_str());
            Brakeza::get()->getManagerGui()->setSelectedObject(selectedObject);
        }
    }
}

int ComponentRender::getFps() const
{
    return fps;
}

void ComponentRender::deleteRemovedObjects()
{
    auto &sceneObjects = Brakeza::get()->getSceneObjects();
    sceneObjects.erase(
        std::remove_if(
            sceneObjects.begin(),
            sceneObjects.end(), [](const Object3D* object) {
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

SceneLoader &ComponentRender::getSceneLoader()
{
    return sceneLoader;
}

ProjectLoader &ComponentRender::getProjectLoader()
{
    return projectLoader;
}

std::vector<ShaderOGLCustom *> &ComponentRender::getSceneShaders() {
    return sceneShaders;
}

void ComponentRender::loadShaderIntoScene(const std::string &folder, const std::string &jsonFilename)
{
    auto name = Tools::getFilenameWithoutExtension(jsonFilename);

    std::string shaderFragmentFile = folder + std::string(name + ".fs");
    std::string shaderVertexFile = folder + std::string(name + ".vs");

    auto type = ShaderOGLCustom::extractTypeFromShaderName(folder, name);

    Logging::Message("LoadShaderInto Scene: Folder: %s, Name: %s, Type: %d", folder.c_str(), name.c_str(), type);

    switch(type) {
        case SHADER_POSTPROCESSING : {
            this->addShaderToScene(
                new ShaderOGLCustomPostprocessing(name, shaderVertexFile, shaderFragmentFile)
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

ShaderOGLCustom* ComponentRender::getLoadedShader(const std::string &folder, const std::string &jsonFilename)
{
    auto name = Tools::getFilenameWithoutExtension(jsonFilename);

    std::string shaderFragmentFile = folder + std::string(name + ".fs");
    std::string shaderVertexFile = folder + std::string(name + ".vs");

    auto type = ShaderOGLCustom::extractTypeFromShaderName(folder, name);

    Logging::Message("LoadShaderInto Scene: Folder: %s, Name: %s, Type: %d", folder.c_str(), name.c_str(), type);

    switch(type) {
        case SHADER_POSTPROCESSING : {
            return new ShaderOGLCustomPostprocessing(name, shaderVertexFile, shaderFragmentFile);
        }
        default:
        case SHADER_OBJECT : {
            return new ShaderOGLCustomMesh3D(nullptr, name, shaderVertexFile, shaderFragmentFile);
        }
    }
}

void ComponentRender::addShaderToScene(ShaderOGLCustom *shader)
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

void ComponentRender::RunShadersOpenGLPostUpdate() const
{
    for (auto s: sceneShaders) {
        if (!s->isEnabled()) continue;
        s->postUpdate();
    }
}

void ComponentRender::removeSceneShaderByIndex(int index) {

    if (index >= 0 && index < sceneShaders.size()) {
        sceneShaders.erase(sceneShaders.begin() + index);
    }
}

void ComponentRender::removeSceneShader(const ShaderOGLCustom *shader)
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

void ComponentRender::RunShadersOpenGLPreUpdate() const
{
    for( auto s: sceneShaders) {
        if (!s->isEnabled()) continue;
        s->onUpdate();
    }
}

ShaderOGLCustom *ComponentRender::getSceneShaderByIndex(int i) const
{
    return sceneShaders[i];
}

ShaderOGLCustom *ComponentRender::getSceneShaderByLabel(const std::string& name) const
{
    for (auto &s: sceneShaders) {
        if (s->getLabel() == name) {
            return s;
        }
    }

    return nullptr;
}

bool ComponentRender::compareDistances(const Object3D* obj1, const Object3D* obj2)
{
    return obj1->getDistanceToCamera() > obj2->getDistanceToCamera();
}

void ComponentRender::setGlobalIlluminationDirection(const Vertex3D &v) const
{
    shaderOGLRender->setGlobalIlluminationDirection(v);
}

void ComponentRender::setGlobalIlluminationAmbient(const Vertex3D &v) const
{
    shaderOGLRender->setGlobalIlluminationAmbient(v);
}

void ComponentRender::setGlobalIlluminationDiffuse(const Vertex3D &v) const
{
    shaderOGLRender->setGlobalIlluminationDiffuse(v);
}

void ComponentRender::setGlobalIlluminationSpecular(const Vertex3D &v) const
{
    getShaderOGLRenderForward()->setGlobalIlluminationSpecular(v);
}

void ComponentRender::drawLine(const Vertex3D &from, const Vertex3D &to, const Color &c) const
{
    shaderOGLLine3D->render(
        from,
        to,
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer(),
        c
    );
}

ShaderOGLLine3D *ComponentRender::getShaderOGLLine3D() const
{
    return shaderOGLLine3D;
}

ShaderOGLImage *ComponentRender::getShaderOGLImage() const
{
    return shaderOGLImage;
}

ShaderOGLRenderForward *ComponentRender::getShaderOGLRenderForward() const
{
    return shaderOGLRender;
}

ShaderOGLLine *ComponentRender::getShaderOGLLine() const
{
    return shaderOGLLine;
}

ShaderOGLWire *ComponentRender::getShaderOGLWireframe() const
{
    return shaderOGLWireframe;
}

ShaderOGLShading *ComponentRender::getShaderOGLShading() const
{
    return shaderOGLShading;
}

ShaderOGLPoints *ComponentRender::getShaderOGLPoints() const
{
    return shaderOGLPoints;
}

ShaderOGLOutline *ComponentRender::getShaderOGLOutline() const
{
    return shaderOGLOutline;
}

ShaderOGLColor *ComponentRender::getShaderOGLColor() const
{
    return shaderOGLColor;
}

ShaderOGLParticles *ComponentRender::getShaderOGLParticles() const
{
    return shaderOGLParticles;
}

ShaderOGLDOF *ComponentRender::getShaderOGLDOF() const
{
    return shaderOGLDOFBlur;
}

ShaderOGLFog *ComponentRender::getShaderOGLFOG() const
{
    return shaderOGLFOG;
}

ShaderOGLTint *ComponentRender::getShaderOGLTint() const
{
    return shaderOGLTint;
}

ShaderOGLBonesTransforms *ComponentRender::getShaderOGLBonesTransforms() const
{
    return shaderOGLBonesTransforms;
}

ShaderOGLShadowPass *ComponentRender::getShaderOGLShadowPass() const
{
    return shaderShadowPass;
}

ShaderOGLShadowPassDebugLight *ComponentRender::getShaderOGLShadowPassDebugLight() const {

    return shaderShadowPassDebugLight;
}

ShaderOGLDepthMap *ComponentRender::getShaderOGLDepthMap() const
{
    return shaderOGLDepthMap;
}

ShaderOGLRenderDeferred *ComponentRender::getShaderOGLRenderDeferred() const
{
    return shaderOGLGBuffer;
}

ShaderOGLLightPass *ComponentRender::getShaderOGLLightPass() const
{
    return shaderOGLLightPass;
}

GLuint ComponentRender::getLastFrameBufferUsed() const
{
    return lastFrameBufferUsed;
}

void ComponentRender::setLastFrameBufferUsed(GLuint value)
{
    lastFrameBufferUsed = value;
}

GLuint ComponentRender::getLastProgramUsed() const
{
    return lastProgramUsed;
}

void ComponentRender::setLastProgramUsed(GLuint value)
{
    lastProgramUsed = value;
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

void ComponentRender::resizeShadersFramebuffers() const
{
    Logging::Message("Resizing framebuffers...");

    shaderOGLRender->destroy();
    shaderOGLImage->destroy();
    shaderOGLLine->destroy();
    shaderOGLWireframe->destroy();
    shaderOGLShading->destroy();
    shaderOGLPoints->destroy();
    shaderOGLOutline->destroy();
    shaderOGLColor->destroy();
    shaderOGLParticles->destroy();
    shaderOGLDOFBlur->destroy();
    shaderOGLDepthMap->destroy();
    shaderOGLFOG->destroy();
    shaderOGLGBuffer->destroy();
    shaderOGLLightPass->destroy();

    if (BrakezaSetup::get()->ENABLE_SHADOW_MAPPING) {
        shaderShadowPass->createSpotLightsDepthTextures(static_cast<int>(shaderOGLRender->getShadowMappingSpotLights().size()));
        shaderShadowPass->resetFramebuffers();
    }

    for (const auto s: sceneShaders) {
        s->destroy();
    }
}

void ComponentRender::FillOGLBuffers(std::vector<Mesh3DData> &meshes)
{
    for (auto &m: meshes) {
        glGenBuffers(1, &m.vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(m.vertices.size() * sizeof(glm::vec4)), &m.vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m.uvBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m.uvBuffer);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(m.uvs.size() * sizeof(glm::vec2)), &m.uvs[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m.normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m.normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(m.normals.size() * sizeof(glm::vec3)), &m.normals[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m.feedbackBuffer); // Creamos el buffer para Transform Feedback de vértices
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m.feedbackBuffer);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, static_cast<GLuint>(m.vertices.size() * sizeof(glm::vec4)), &m.vertices[0], GL_DYNAMIC_COPY);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
    }
}

void ComponentRender::ClearShadowMaps() const
{
    auto numLights = static_cast<int>(shaderOGLRender->getShadowMappingSpotLights().size());

    glBindFramebuffer(GL_FRAMEBUFFER, shaderShadowPass->getDirectionalLightDepthMapFBO());
    glClear(GL_DEPTH_BUFFER_BIT);

    if (numLights <= 0) return;

    glBindFramebuffer(GL_FRAMEBUFFER, shaderShadowPass->getSpotLightsDepthMapsFBO());
    glClear(GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < numLights; i++) {
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shaderShadowPass->getSpotLightsShadowMapArrayTextures(), 0, i);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void ComponentRender::RenderLayersToGlobalFramebuffer() const
{
    auto window = ComponentsManager::get()->getComponentWindow();
    auto gBuffer = window->getGBuffer();
    auto globalBuffer = window->getGlobalBuffers();

    int widthWindow = window->getWidth();
    int heightWindow = window->getHeight();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shaderOGLLightPass->fillSpotLightsMatricesUBO();

    if (BrakezaSetup::get()->ENABLE_LIGHTS) {
        shaderOGLLightPass->render(
            gBuffer.positions,
            gBuffer.normals,
            gBuffer.albedo,
            shaderOGLRender->getDirectionalLight(),
            shaderShadowPass->getDirectionalLightDepthTexture(),
            shaderOGLRender->getNumPointLights(),
            shaderOGLRender->getNumSpotLights(),
            shaderShadowPass->getSpotLightsShadowMapArrayTextures(),
            static_cast<int>(shaderOGLRender->getShadowMappingSpotLights().size()),
            globalBuffer.sceneFBO
        );
    }

    shaderOGLImage->renderTexture(globalBuffer.backgroundTexture, 0, 0, widthWindow, heightWindow, 1, true, globalBuffer.globalFBO);
    shaderOGLImage->renderTexture(globalBuffer.sceneTexture, 0, 0, widthWindow, heightWindow, 1, true, globalBuffer.globalFBO);
    shaderOGLImage->renderTexture(globalBuffer.postProcessingTexture, 0, 0, widthWindow, heightWindow, 1, true, globalBuffer.globalFBO);

    if (BrakezaSetup::get()->ENABLE_FOG) {
        shaderOGLFOG->render(globalBuffer.sceneTexture, gBuffer.depth);
        shaderOGLImage->renderTexture(shaderOGLFOG->getTextureResult(), 0, 0, widthWindow, heightWindow, 1, false, globalBuffer.globalFBO);
    }

    if (BrakezaSetup::get()->ENABLE_DOF_BLUR) {
        shaderOGLDOFBlur->render(globalBuffer.sceneTexture, gBuffer.depth);
        shaderOGLImage->renderTexture(shaderOGLDOFBlur->getTextureResult(), 0, 0, widthWindow, heightWindow, 1, false, globalBuffer.globalFBO);
    }

    if (BrakezaSetup::get()->ENABLE_TRIANGLE_MODE_DEPTHMAP) {
        shaderOGLDepthMap->render(gBuffer.depth, globalBuffer.globalFBO);
    }

    if (BrakezaSetup::get()->TRIANGLE_MODE_PICKING_COLORS) {
        shaderOGLImage->renderTexture(
            window->getPickingColorFramebuffer().rbgTexture, 0, 0, widthWindow, heightWindow, 1, true, globalBuffer.globalFBO
        );
    }
}
