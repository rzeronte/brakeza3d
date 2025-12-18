#include "../../include/Components/ComponentRender.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"
#include "../../include/OpenGL/ShaderOGLCustomPostprocessing.h"
#include "../../include/OpenGL/ShaderOGLCustomMesh3D.h"
#include "../../include/OpenGL/ShaderOGLShadowPass.h"
#include "../../include/Render/Profiler.h"
#include "../../include/Render/Transforms.h"

ComponentRender::~ComponentRender()
{
    for (auto s: sceneShaders) {
        delete s;
    }

    delete textWriter;
}

void ComponentRender::onStart()
{
    Component::onStart();

    setEnabled(true);

    auto window = Components::get()->Window();
    textWriter = new TextWriter(window->getRenderer(), window->getFontDefault());

    RegisterShaders();
}

void ComponentRender::RegisterShaders()
{
    shaders.shaderOGLRender = new ShaderOGLRenderForward();
    shaders.shaderOGLImage = new ShaderOGLImage();
    shaders.shaderOGLLine = new ShaderOGLLine();
    shaders.shaderOGLWireframe = new ShaderOGLWire();
    shaders.shaderOGLLine3D = new ShaderOGLLine3D();
    shaders.shaderOGLShading = new ShaderOGLShading();
    shaders.shaderOGLPoints = new ShaderOGLPoints();
    shaders.shaderOGLOutline = new ShaderOGLOutline();
    shaders.shaderOGLColor = new ShaderOGLColor();
    shaders.shaderOGLParticles = new ShaderOGLParticles();
    shaders.shaderOGLDOFBlur = new ShaderOGLDOF();
    shaders.shaderOGLDepthMap = new ShaderOGLDepthMap();
    shaders.shaderOGLFOG = new ShaderOGLFog();
    shaders.shaderOGLTint = new ShaderOGLTint();
    shaders.shaderOGLBonesTransforms = new ShaderOGLBonesTransforms();
    shaders.shaderOGLGBuffer = new ShaderOGLRenderDeferred();
    shaders.shaderOGLLightPass = new ShaderOGLLightPass();
    shaders.shaderShadowPass = new ShaderOGLShadowPass();
    shaders.shaderShadowPassDebugLight = new ShaderOGLShadowPassDebugLight();
    shaders.shaderOGLGrid = new ShaderOGLGrid();

    std::vector<ShaderBaseOpenGL*> allShaders;
        allShaders.push_back(shaders.shaderOGLRender);
        allShaders.push_back(shaders.shaderOGLImage);
        allShaders.push_back(shaders.shaderOGLLine);
        allShaders.push_back(shaders.shaderOGLWireframe);
        allShaders.push_back(shaders.shaderOGLLine3D);
        allShaders.push_back(shaders.shaderOGLShading);
        allShaders.push_back(shaders.shaderOGLPoints);
        allShaders.push_back(shaders.shaderOGLOutline);
        allShaders.push_back(shaders.shaderOGLColor);
        allShaders.push_back(shaders.shaderOGLParticles);
        allShaders.push_back(shaders.shaderOGLDOFBlur);
        allShaders.push_back(shaders.shaderOGLDepthMap);
        allShaders.push_back(shaders.shaderOGLFOG);
        allShaders.push_back(shaders.shaderOGLTint);
        allShaders.push_back(shaders.shaderOGLBonesTransforms);
        allShaders.push_back(shaders.shaderOGLGBuffer);
        allShaders.push_back(shaders.shaderOGLLightPass);
        allShaders.push_back(shaders.shaderShadowPass);
        allShaders.push_back(shaders.shaderShadowPassDebugLight);
        allShaders.push_back(shaders.shaderOGLGrid);

    for (auto &s : allShaders) {
        s->PrepareBackground();
        s->PrepareMainThread();
    }
}

void ComponentRender::preUpdate()
{
    DeleteRemovedObjects();

    Component::preUpdate();
    ClearShadowMaps();
    UpdateFPS();
}

void ComponentRender::DrawFPS() const
{
    textWriter->WriteTextTTFAutoSize(10, 10, std::to_string(getFps()).c_str(), Color::white(), 1.0f);
}

void ComponentRender::onUpdate()
{
    Component::onUpdate();

    if (!isEnabled()) return;

    shaders.shaderOGLRender->CreateUBOFromLights();

    auto numSpotLights = shaders.shaderOGLRender->getNumSpotLights();

    if (Config::get()->ENABLE_LIGHTS) {
        if (Config::get()->ENABLE_SHADOW_MAPPING) {
            if (shaders.shaderOGLRender->hasSpotLightsChanged()) {
                shaders.shaderShadowPass->createSpotLightsDepthTextures(numSpotLights);
                shaders.shaderShadowPass->setupFBOSpotLights();
                shaders.shaderOGLRender->setLastSpotLightsSize(numSpotLights);
            }
        }
    }

    onUpdateSceneObjects();

    if (SETUP->DRAW_FPS_RENDER) {
        DrawFPS();
    }

    if (Brakeza::get()->GUI()->isWindowOpen(GUIType::DEPTH_LIGHTS_MAPS)) {
        shaders.shaderShadowPassDebugLight->CreateFramebuffer();
        shaders.shaderShadowPassDebugLight->createArrayTextures(numSpotLights);
        shaders.shaderShadowPassDebugLight->updateDebugTextures(numSpotLights);
    }

    if (SETUP->ENABLE_GRID_BACKGROUND) {
        shaders.shaderOGLGrid->render(Components::get()->Window()->getBackgroundFramebuffer());
    }
}

void ComponentRender::postUpdate()
{
    Component::postUpdate();

    std::vector<Object3D *> sceneObjects = Brakeza::get()->getSceneObjects();

    std::sort(sceneObjects.begin(), sceneObjects.end(), compareDistances);

    for (auto &o: sceneObjects) {
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

    for (const auto &o: sceneObjects) {
        if (o->isEnabled()) {
            o->onUpdate();
        }
    }
}

void ComponentRender::UpdateFPS()
{
    if (!Config::get()->DRAW_FPS_RENDER) return;

    frameTime += Brakeza::get()->getDeltaTimeMicro();
    ++fpsFrameCounter;

    if (frameTime >= 1000.0f) {
        fps = fpsFrameCounter;
        frameTime -= 1000.0f;
        fpsFrameCounter = 0;
    }
}

void ComponentRender::setSelectedObject(Object3D *o)
{
    this->selectedObject = o;
}

void ComponentRender::UpdateSelectedObject3D()
{
    auto input = Components::get()->Input();

    if (!input->isEnabled()) return;

    if (input->isClickLeft() && !input->isMouseMotion()) {
        auto x = input->getMouseX();
        auto y = input->getMouseY();

        const auto id = Components::get()->Window()->getObjectIDByPickingColorFramebuffer(x, y);
        selectedObject = Brakeza::get()->getObjectById(id);
        if (selectedObject != nullptr) {
            Logging::Message("[Render] Selected object: %s", selectedObject->getName().c_str());
            Brakeza::get()->GUI()->setSelectedObject(selectedObject);
        }
    }
}

void ComponentRender::DeleteRemovedObjects()
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

void ComponentRender::LoadShaderIntoScene(const std::string &folder, const std::string &jsonFilename)
{
    auto name = Tools::getFilenameWithoutExtension(jsonFilename);

    std::string shaderFragmentFile = folder + std::string(name + ".fs");
    std::string shaderVertexFile = folder + std::string(name + ".vs");
    std::string typesFile = folder + std::string(name + ".json");

    auto type = ShaderOGLCustom::ExtractTypeFromShaderName(folder, name);

    Logging::Message("[Render] Loading shader into scene: Folder: %s, Name: %s, Type: %d", folder.c_str(), name.c_str(), type);

    switch(type) {
        case SHADER_POSTPROCESSING : {
            auto s = new ShaderOGLCustomPostprocessing(name, typesFile, shaderVertexFile, shaderFragmentFile);
            s->PrepareBackground();
            s->PrepareMainThread();
            AddShaderToScene(s);
            break;
        }
        default:
        case SHADER_OBJECT : {
            Logging::Error("You can't add a ShaderObject type into scene");
            break;
        }
    }
}

ShaderOGLCustom* ComponentRender::CreateCustomShaderFromDisk(const std::string &folder, const std::string &jsonFilename)
{
    auto name = Tools::getFilenameWithoutExtension(jsonFilename);

    std::string fsFile = folder + std::string(name + ".fs");
    std::string vsFile = folder + std::string(name + ".vs");
    std::string typesFile = folder + std::string(name + ".json");

    auto type = ShaderOGLCustom::ExtractTypeFromShaderName(folder, name);

    Logging::Message("[Render] Loading shader into scene: Name: %s, Folder: %s, Type: %d", name.c_str(), folder.c_str(), type);

    switch(type) {
        case SHADER_POSTPROCESSING : {
            return new ShaderOGLCustomPostprocessing(name, typesFile, vsFile, fsFile);
        }
        default:
        case SHADER_OBJECT : {
            return new ShaderOGLCustomMesh3D(nullptr, name, typesFile, vsFile, fsFile);
        }
    }
}

void ComponentRender::AddShaderToScene(ShaderOGLCustom *shader)
{
    sceneShaders.push_back(shader);
}

void ComponentRender::setSceneShadersEnabled(bool value)
{
    sceneShadersEnabled = value;
}

void ComponentRender::RunSceneShadersPostUpdate()
{

    Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), "RunShadersOpenGLPostUpdate");

    Components::get()->Window()->getPostProcessingManager()->processChain(
        Components::get()->Window()->getSceneTexture(),
        Components::get()->Window()->getGlobalFramebuffer()
    );

    Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "RunShadersOpenGLPostUpdate");

}

void ComponentRender::RemoveSceneShaderByIndex(int index) {

    if (index >= 0 && index < sceneShaders.size()) {
        sceneShaders.erase(sceneShaders.begin() + index);
    }
}

void ComponentRender::RemoveSceneShader(const ShaderOGLCustom *shader)
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

ShaderOGLCustom *ComponentRender::getSceneShaderByLabel(const std::string& name) const
{
    for (auto &s: sceneShaders) {
        if (s->getLabel() == name) {
            return s;
        }
    }

    return nullptr;
}

void ComponentRender::MakeScreenShot()
{
    const auto file = Config::get()->SCREENSHOTS_FOLDER + Brakeza::UniqueObjectLabel("screenshot_") + std::string(".png");

    Tools::saveTextureToFile(
        Components::get()->Window()->getSceneTexture(),
        Components::get()->Window()->getWidthRender(),
        Components::get()->Window()->getHeightRender(),
        file.c_str()
    );

    Logging::Message("[Render] Saving screenshot to file '%s'...", file.c_str());
}

bool ComponentRender::compareDistances(const Object3D* obj1, const Object3D* obj2)
{
    return obj1->getDistanceToCamera() > obj2->getDistanceToCamera();
}

void ComponentRender::setGlobalIlluminationDirection(const Vertex3D &v) const
{
    shaders.shaderOGLRender->setGlobalIlluminationDirection(v);
}

void ComponentRender::setGlobalIlluminationAmbient(const Vertex3D &v) const
{
    shaders.shaderOGLRender->setGlobalIlluminationAmbient(v);
}

void ComponentRender::setGlobalIlluminationDiffuse(const Vertex3D &v) const
{
    shaders.shaderOGLRender->setGlobalIlluminationDiffuse(v);
}

void ComponentRender::setGlobalIlluminationSpecular(const Vertex3D &v) const
{
    shaders.shaderOGLRender->setGlobalIlluminationSpecular(v);
}

void ComponentRender::drawLine(const Vertex3D &from, const Vertex3D &to, const Color &c) const
{
    shaders.shaderOGLLine3D->render(
        from,
        to,
        Components::get()->Window()->getForegroundFramebuffer(),
        c
    );
}

void ComponentRender::setLastFrameBufferUsed(GLuint value)
{
    lastFrameBufferUsed = value;
}

void ComponentRender::setLastProgramUsed(GLuint value)
{
    lastProgramUsed = value;
}

void ComponentRender::ChangeOpenGLFramebuffer(GLuint framebuffer)
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

void ComponentRender::resizeShadersFramebuffers() const
{
    Logging::Message("[Render] Resizing framebuffers...");

    shaders.shaderOGLRender->Destroy();
    shaders.shaderOGLImage->Destroy();
    shaders.shaderOGLLine->Destroy();
    shaders.shaderOGLWireframe->Destroy();
    shaders.shaderOGLShading->Destroy();
    shaders.shaderOGLPoints->Destroy();
    shaders.shaderOGLOutline->Destroy();
    shaders.shaderOGLColor->Destroy();
    shaders.shaderOGLParticles->Destroy();
    shaders.shaderOGLDOFBlur->Destroy();
    shaders.shaderOGLDepthMap->Destroy();
    shaders.shaderOGLFOG->Destroy();
    shaders.shaderOGLGBuffer->Destroy();
    shaders.shaderOGLLightPass->Destroy();

    if (Config::get()->ENABLE_SHADOW_MAPPING) {
        shaders.shaderShadowPass->createSpotLightsDepthTextures(static_cast<int>(shaders.shaderOGLRender->getShadowMappingSpotLights().size()));
        shaders.shaderShadowPass->ResetFramebuffers();
    }

    for (const auto s: sceneShaders) {
        s->Destroy();
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
    auto numLights = static_cast<int>(shaders.shaderOGLRender->getShadowMappingSpotLights().size());

    glBindFramebuffer(GL_FRAMEBUFFER, shaders.shaderShadowPass->getDirectionalLightDepthMapFBO());
    glClear(GL_DEPTH_BUFFER_BIT);

    if (numLights <= 0) return;

    glBindFramebuffer(GL_FRAMEBUFFER, shaders.shaderShadowPass->getSpotLightsDepthMapsFBO());
    glClear(GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < numLights; i++) {
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shaders.shaderShadowPass->getSpotLightsShadowMapArrayTextures(), 0, i);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void ComponentRender::FlipBuffersToGlobal() const
{
    Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), "FlipBuffersToGlobal");

    auto window = Components::get()->Window();
    auto gBuffer = window->getGBuffer();
    auto globalBuffer = window->getGlobalBuffers();

    int widthWindow = window->getWidth();
    int heightWindow = window->getHeight();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shaders.shaderOGLLightPass->FillSpotLightsMatricesUBO();

    if (Config::get()->ENABLE_LIGHTS) {
        shaders.shaderOGLLightPass->render(
            gBuffer.positions,
            gBuffer.normals,
            gBuffer.albedo,
            shaders.shaderOGLRender->getDirectionalLight(),
            shaders.shaderShadowPass->getDirectionalLightDepthTexture(),
            shaders.shaderOGLRender->getNumPointLights(),
            shaders.shaderOGLRender->getNumSpotLights(),
            shaders.shaderShadowPass->getSpotLightsShadowMapArrayTextures(),
            (int) shaders.shaderOGLRender->getShadowMappingSpotLights().size(),
            globalBuffer.sceneFBO
        );
    }

    shaders.shaderOGLImage->renderTexture(globalBuffer.backgroundTexture, 0, 0, widthWindow, heightWindow, 1, true, globalBuffer.globalFBO);
    shaders.shaderOGLImage->renderTexture(globalBuffer.sceneTexture, 0, 0, widthWindow, heightWindow, 1, true, globalBuffer.globalFBO);
    shaders.shaderOGLImage->renderTexture(globalBuffer.postProcessingTexture, 0, 0, widthWindow, heightWindow, 1, true, globalBuffer.globalFBO);

    if (Config::get()->ENABLE_FOG) {
        shaders.shaderOGLFOG->render(globalBuffer.sceneTexture, gBuffer.depth);
        shaders.shaderOGLImage->renderTexture(shaders.shaderOGLFOG->getTextureResult(), 0, 0, widthWindow, heightWindow, 1, false, globalBuffer.globalFBO);
    }

    if (Config::get()->ENABLE_DOF_BLUR) {
        shaders.shaderOGLDOFBlur->render(globalBuffer.sceneTexture, gBuffer.depth);
        shaders.shaderOGLImage->renderTexture(shaders.shaderOGLDOFBlur->getTextureResult(), 0, 0, widthWindow, heightWindow, 1, false, globalBuffer.globalFBO);
    }

    if (Config::get()->ENABLE_TRIANGLE_MODE_DEPTHMAP) {
        shaders.shaderOGLDepthMap->Render(gBuffer.depth, globalBuffer.globalFBO);
    }

    if (Config::get()->TRIANGLE_MODE_PICKING_COLORS) {
        shaders.shaderOGLImage->renderTexture(
            window->getPickingColorFramebuffer().rbgTexture, 0, 0, widthWindow, heightWindow, 1, true, globalBuffer.globalFBO
        );
    }

    Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "FlipBuffersToGlobal");
}
