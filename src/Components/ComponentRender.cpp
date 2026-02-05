#include "../../include/Components/ComponentRender.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/ShadersGUI.h"
#include "../../include/OpenGL/Code/ShaderOGLCustomCodePostprocessing.h"
#include "../../include/OpenGL/Code/ShaderOGLCustomCodeMesh3D.h"
#include "../../include/OpenGL/ShaderOGLShadowPass.h"
#include "../../include/OpenGL/Nodes/ShaderNodesMesh3D.h"
#include "../../include/OpenGL/Nodes/ShaderNodesPostProcessing.h"
#include "../../include/Render/Profiler.h"
#include "../../include/Render/Transforms.h"

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
        s->PrepareSync();
    }
}

void ComponentRender::preUpdate()
{
    DeleteRemovedObjects();
    ClearShadowMaps();
    UpdateFPS();
}

void ComponentRender::DrawFPS() const
{
    textWriter->WriteTextTTFAutoSize(10, 10, std::to_string(getFps()).c_str(), Color::white(), 1.0f);
}

void ComponentRender::onUpdate()
{
    if (!isEnabled()) return;

    shaders.shaderOGLRender->CreateUBOFromLights();

    auto numSpotLights = shaders.shaderOGLRender->getNumSpotLights();

    if (Config::get()->ENABLE_LIGHTS && Config::get()->ENABLE_SHADOW_MAPPING) {
        if (shaders.shaderOGLRender->hasSpotLightsChanged()) {
            shaders.shaderShadowPass->createSpotLightsDepthTextures(numSpotLights);
            shaders.shaderShadowPass->setupFBOSpotLights();
            shaders.shaderOGLRender->setLastSpotLightsSize(numSpotLights);
        }
    }

    onUpdateSceneObjects();

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
    std::vector<Object3D *> &sceneObjects = Brakeza::get()->getSceneObjects();

    std::sort(sceneObjects.begin(), sceneObjects.end(), compareDistances);

    for (auto &o: sceneObjects) {
        if (!o->isEnabled()) continue;
        o->postUpdate();
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
    std::vector<Object3D *> &sceneObjects = Brakeza::get()->getSceneObjects();

    std::sort(sceneObjects.begin(), sceneObjects.end(), compareDistances);

    for (const auto &o: sceneObjects) {
        if (!o->isEnabled()) continue;
        o->onUpdate();
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
    selectedObject = o;
}

void ComponentRender::UpdateSelectedObject3D()
{
    auto input = Components::get()->Input();

    if (!input->isEnabled()) return;

    if (input->isClickLeft() && !input->isMouseMotion()) {

        auto window = Components::get()->Window();
      // Coordenadas del mouse en la ventana
        int mouseX = input->getMouseX();
        int mouseY = input->getMouseY();

        // Transformar a coordenadas del framebuffer de 800x600
        int normalizedX = ((float)mouseX / window->getWidth()) * window->getWidthRender();
        int normalizedY = (((float)mouseY / window->getHeight()) * window->getHeightRender());

        LOG_MESSAGE("[Render] Click: %d, %d", (int) normalizedX, (int) normalizedY);

        const auto id = Components::get()->Window()->getObjectIDByPickingColorFramebuffer((int) normalizedX, (int) normalizedY);
        selectedObject = Brakeza::get()->getObjectById(id);
        if (selectedObject != nullptr) {
            LOG_MESSAGE("[Render] Selected object: %s", selectedObject->getName().c_str());
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

void ComponentRender::LoadShaderIntoScene(const std::string &filePath)
{
    auto metaInfo = ShadersGUI::ExtractShaderCustomCodeMetainfo(filePath);

    if (ShaderBaseCustom::getShaderTypeFromString(metaInfo.type) == SHADER_POSTPROCESSING ||
        ShaderBaseCustom::getShaderTypeFromString(metaInfo.type) == SHADER_NODE_POSTPROCESSING
    ) {
        auto shader = CreateCustomShaderFromDisk(metaInfo, nullptr);

        if (shader != nullptr) {
            AddShaderToScene(shader);
            return;
        }
    }

    LOG_ERROR("[Render] Error: Cannot apply shader to scene...");
}

ShaderBaseCustom* ComponentRender::CreateCustomShaderFromDisk(const ShaderBaseCustomMetaInfo &info, Mesh3D* mesh)
{
    if (ShaderBaseCustom::getShaderTypeFromString(info.type) == SHADER_POSTPROCESSING) {
        auto s = new ShaderOGLCustomCodePostprocessing(info.name, info.typesFile, info.vsFile, info.fsFile);
        s->PrepareSync();
        return s;
    }

    if (ShaderBaseCustom::getShaderTypeFromString(info.type) == SHADER_OBJECT) {
        auto s = new ShaderOGLCustomCodeMesh3D(mesh, info.name, info.typesFile, info.vsFile, info.fsFile);
        s->PrepareSync();
        return s;
    }

    if (ShaderBaseCustom::getShaderTypeFromString(info.type) == SHADER_NODE_OBJECT) {
        auto manager = new ShaderNodeEditorManager(SHADER_NODE_OBJECT);
        manager->LoadFromFile(info.typesFile.c_str());

        auto s = new ShaderNodesMesh3D(info.name, info.typesFile, SHADER_NODE_OBJECT, manager, mesh);
        s->PrepareSync();
        return s;
    }

    if (ShaderBaseCustomOGLCode::getShaderTypeFromString(info.type) == SHADER_NODE_POSTPROCESSING) {
        auto manager = new ShaderNodeEditorManager(SHADER_NODE_POSTPROCESSING);
        manager->LoadFromFile(info.typesFile.c_str());
        auto s = new ShaderNodesPostProcessing(info.name, info.typesFile, SHADER_NODE_POSTPROCESSING, manager);
        s->PrepareSync();
        return s;
    }

    return nullptr;
}

void ComponentRender::AddShaderToScene(ShaderBaseCustom *shader)
{
    sceneShaders.push_back(shader);
}

void ComponentRender::PostProcessingShadersChain()
{
    Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), "PostProcessingShadersChain");

    auto window = Components::get()->Window();

    window->getPostProcessingManager()->SetSceneTextures(
        window->getSceneTexture(),
        window->getGBuffer().depth
    );

    window->getPostProcessingManager()->processChain(
        window->getSceneTexture(),
        window->getGlobalFramebuffer()
    );

    Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "PostProcessingShadersChain");
}

void ComponentRender::RemoveSceneShaderByIndex(int index) {

    if (index >= 0 && index < sceneShaders.size()) {
        sceneShaders.erase(sceneShaders.begin() + index);
    }
}

void ComponentRender::RemoveSceneShader(const ShaderBaseCustom *shader)
{
    LOG_MESSAGE("Removing SCENE script %s", shader->getLabel().c_str());

    for (auto it = sceneShaders.begin(); it != sceneShaders.end(); ++it) {
        if (*it == shader) {
            delete *it;
            sceneShaders.erase(it);
            return;
        }
    }
}

ShaderBaseCustom *ComponentRender::getSceneShaderByLabel(const std::string& name) const
{
    for (auto &s: sceneShaders) {
        if (s->getLabel() == name) {
            return s;
        }
    }

    return nullptr;
}

void ComponentRender::MakeScreenShot(std::string filename)
{
    if (filename.empty()) {
        filename = Config::get()->SCREENSHOTS_FOLDER + Brakeza::UniqueObjectLabel("screenshot_") + std::string(".png");
    }

    Tools::saveTextureToFile(
        Components::get()->Window()->getGlobalTexture(),
        Components::get()->Window()->getWidthRender(),
        Components::get()->Window()->getHeightRender(),
        filename.c_str()
    );

    LOG_MESSAGE("[Render] Saving screenshot to file '%s'...", filename.c_str());
}

bool ComponentRender::compareDistances(const Object3D* obj1, const Object3D* obj2)
{
    return obj1->getDistanceToCamera() > obj2->getDistanceToCamera();
}

void ComponentRender::setGlobalIlluminationDirection(Vertex3D v) const
{
    shaders.shaderOGLRender->setGlobalIlluminationDirection(v);
}

void ComponentRender::setGlobalIlluminationAmbient(Vertex3D v) const
{
    shaders.shaderOGLRender->setGlobalIlluminationAmbient(v);
}

void ComponentRender::setGlobalIlluminationDiffuse(Vertex3D v) const
{
    shaders.shaderOGLRender->setGlobalIlluminationDiffuse(v);
}

void ComponentRender::setGlobalIlluminationSpecular(Vertex3D v) const
{
    shaders.shaderOGLRender->setGlobalIlluminationSpecular(v);
}

void ComponentRender::DrawLine(const Vertex3D &from, const Vertex3D &to, const Color &c) const
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

void ComponentRender::ChangeOpenGLProgram(GLuint programID)
{
    if (getLastProgramUsed() != programID) {
        glUseProgram(programID);
        setLastProgramUsed(programID);
    }
}

void ComponentRender::resizeShadersFramebuffers() const
{
    LOG_MESSAGE("[Render] Resizing framebuffers...");

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
        shaders.shaderShadowPass->createSpotLightsDepthTextures((int) shaders.shaderOGLRender->getShadowMappingSpotLights().size());
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
    auto numLights = (int) shaders.shaderOGLRender->getShadowMappingSpotLights().size();

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

void ComponentRender::LightPass() const
{
    Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), "LightPass");

    auto window = Components::get()->Window();
    auto gBuffer = window->getGBuffer();
    auto globalBuffer = window->getGlobalBuffers();

    int widthWindow = window->getWidthRender();
    int heightWindow = window->getHeightRender();
    glViewport(0,0, widthWindow, heightWindow);

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

    Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "LightPass");
}

void ComponentRender::FlipBuffersToGlobal() const
{
    Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), "FlipBuffersToGlobal");

    auto window = Components::get()->Window();
    auto gBuffer = window->getGBuffer();
    auto globalBuffer = window->getGlobalBuffers();

    int w = window->getWidthRender();
    int h = window->getHeightRender();

    ComponentWindow::ResetOpenGLSettings();

    shaders.shaderOGLImage->renderTexture(globalBuffer.backgroundTexture, 0, 0, w, h, w, h, 1, true, globalBuffer.globalFBO);
    shaders.shaderOGLImage->renderTexture(globalBuffer.sceneTexture, 0, 0, w, h, w, h ,1, true, globalBuffer.globalFBO);

    if (Config::get()->ENABLE_FOG) {
        shaders.shaderOGLFOG->render(globalBuffer.sceneTexture, gBuffer.depth);
        shaders.shaderOGLImage->renderTexture(shaders.shaderOGLFOG->getTextureResult(), 0, 0, w, h, w, h ,1, false, globalBuffer.globalFBO);
    }

    if (Config::get()->ENABLE_DOF_BLUR) {
        shaders.shaderOGLDOFBlur->render(globalBuffer.sceneTexture, gBuffer.depth);
        shaders.shaderOGLImage->renderTexture(shaders.shaderOGLDOFBlur->getTextureResult(), 0, 0, w, h, w, h ,1, false, globalBuffer.globalFBO);
    }

    if (Config::get()->ENABLE_TRIANGLE_MODE_DEPTHMAP) {
        shaders.shaderOGLDepthMap->Render(gBuffer.depth, globalBuffer.globalFBO);
    }

    if (Config::get()->TRIANGLE_MODE_PICKING_COLORS) {
        shaders.shaderOGLImage->renderTexture(
            window->getPickingColorFramebuffer().rbgTexture, 0, 0, w, h, w, h, 1, true, globalBuffer.globalFBO
        );
    }


    Components::get()->Collisions()->DrawDebugCache();

    Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "FlipBuffersToGlobal");
}

void ComponentRender::MoveSceneShaderUp(ShaderBaseCustom* shader)
{
    if (!shader || sceneShaders.size() < 2)
        return;

    auto it = std::find(sceneShaders.begin(), sceneShaders.end(), shader);

    // No encontrado o ya está arriba
    if (it == sceneShaders.end() || it == sceneShaders.begin())
        return;

    std::iter_swap(it, it - 1);
}

void ComponentRender::MoveSceneShaderDown(ShaderBaseCustom* shader)
{
    if (!shader || sceneShaders.size() < 2)
        return;

    auto it = std::find(sceneShaders.begin(), sceneShaders.end(), shader);

    // No encontrado o ya está abajo
    if (it == sceneShaders.end() || it == sceneShaders.end() - 1)
        return;

    std::iter_swap(it, it + 1);
}

ComponentRender::~ComponentRender()
{
    for (auto &s: sceneShaders) {
        delete s;
    }

    delete textWriter;
}
