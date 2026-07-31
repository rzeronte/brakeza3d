#include "imgui.h"
#include <algorithm>
#include <cmath>
#include "../../include/3D/Vector3D.h"
#include "../../include/Components/ComponentRender.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/ShadersGUI.h"
#include "../../include/OpenGL/Code/ShaderOGLCustomCodePostprocessing.h"
#include "../../include/OpenGL/Code/ShaderOGLCustomCodeMesh3D.h"
#include "../../include/OpenGL/ShaderOGLShadowPass.h"
#include "../../include/OpenGL/Nodes/ShaderNodesMesh3D.h"
#include "../../include/OpenGL/Nodes/ShaderNodesPostProcessing.h"
#include <limits>
#include "../../include/Render/Profiler.h"
#include "../../include/Render/Transforms.h"
#include "../../include/3D/LightPoint.h"
#include "../../include/3D/Mesh3DAnimation.h"
#include "../../include/Render/EngineObserver.h"
#include "../../include/Cache/ImageCache.h"
#include "../../include/Cache/ModelDataCache.h"
#include "../../include/Cache/AnimationDataCache.h"
#include "../../include/Cache/ScriptDataCache.h"

// ── Selection forwarding ──────────────────────────────────────────────────
void ComponentRender::setSelectedObject(Object3D *o)                  { selection.setSelectedObject(o); }
void ComponentRender::addToSelection(Object3D *o)                     { selection.addToSelection(o); }
void ComponentRender::removeFromSelection(const Object3D *o)          { selection.removeFromSelection(o); }
void ComponentRender::clearSelection()                                 { selection.clearSelection(); }
void ComponentRender::DrawSelectionBox() const                         { selection.DrawSelectionBox(); }
void ComponentRender::DrawSelectionRectFill() const                    { selection.DrawSelectionRectFill(); }

Object3D* ComponentRender::getSelectedObject() const                   { return selection.getSelectedObject(); }
const std::vector<Object3D*>& ComponentRender::getSelectedObjects() const { return selection.getSelectedObjects(); }
bool ComponentRender::isObjectInSelection(const Object3D *o) const    { return selection.isObjectInSelection(o); }
bool ComponentRender::hasMultipleSelected() const                      { return selection.hasMultipleSelected(); }

Object3D* ComponentRender::getLastRightClickedObject() const           { return selection.getLastRightClickedObject(); }
std::string ComponentRender::getLastRightClickedSubmeshName() const    { return selection.getLastRightClickedSubmeshName(); }
void ComponentRender::clearRightClickedObject()
{
    selection.clearRightClickedObject();
    selection.clearRightClickedSubmeshName();
}

Object3D* ComponentRender::getLastLeftClickedObject() const            { return selection.getLastLeftClickedObject(); }
std::string ComponentRender::getLastLeftClickedSubmeshName() const     { return selection.getLastLeftClickedSubmeshName(); }
void ComponentRender::clearLeftClickedObject()                         { selection.clearLeftClickedObject(); }

// ── Submesh registry ──────────────────────────────────────────────────────
void ComponentRender::registerSubmesh(unsigned int id, Mesh3D *mesh, const std::string &name)
{
    submeshRegistry[id] = {mesh, name};
}

void ComponentRender::unregisterSubmeshes(Mesh3D *mesh)
{
    for (auto it = submeshRegistry.begin(); it != submeshRegistry.end(); ) {
        it = (it->second.first == mesh) ? submeshRegistry.erase(it) : std::next(it);
    }
}

std::pair<Mesh3D*, std::string> ComponentRender::getSubmeshEntry(unsigned int id) const
{
    auto it = submeshRegistry.find(id);
    return (it != submeshRegistry.end()) ? it->second : std::make_pair(nullptr, std::string{});
}

// ─────────────────────────────────────────────────────────────────────────

void ComponentRender::onStart()
{
    Component::onStart();

    setEnabled(true);

    auto window = Components::get()->Window();
    textWriter = new TextWriter(window->getRenderer(), window->getFontDefault());

    // Build glyph atlas for batched text rendering
    glyphAtlas = new GlyphAtlas();
    if (!glyphAtlas->build(window->getFontDefault(), 512)) {
        Logging::Warning("[ComponentRender] GlyphAtlas build failed");
    }
    textWriter->setGlyphAtlas(glyphAtlas);

    RegisterShaders();

    uiManager = new UIManager();
    uiManager->init(this, Config::get()->UI_WIDGETS_FOLDER);
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
    shaders.shaderOGLDepthMap = new ShaderOGLDepthMap();
    shaders.shaderOGLBonesTransforms = new ShaderOGLBonesTransforms();
    shaders.shaderOGLGBuffer = new ShaderOGLRenderDeferred();
    shaders.shaderOGLLightPass = new ShaderOGLLightPass();
    shaders.shaderShadowPass = new ShaderOGLShadowPass();
    shaders.shaderShadowPassDebugLight = new ShaderOGLShadowPassDebugLight();
    shaders.shaderOGLGrid = new ShaderOGLGrid();
    shaders.shaderGroundCircle = new ShaderOGLGroundCircle();
    shaders.shaderGroundDecal  = new ShaderOGLGroundDecal();
    shaders.shaderAxisQuad     = new ShaderOGLAxisQuad();
    shaders.shaderOGLRect      = new ShaderOGLRect();
    shaders.shaderComputeParticles = new ShaderOGLComputeParticles();
    shaders.shaderGPUParticles     = new ShaderOGLGPUParticles();
    shaders.shaderCircle2D         = new ShaderOGLCircle2D();

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
        allShaders.push_back(shaders.shaderOGLDepthMap);
        allShaders.push_back(shaders.shaderOGLBonesTransforms);
        allShaders.push_back(shaders.shaderOGLGBuffer);
        allShaders.push_back(shaders.shaderOGLLightPass);
        allShaders.push_back(shaders.shaderShadowPass);
        allShaders.push_back(shaders.shaderShadowPassDebugLight);
        allShaders.push_back(shaders.shaderOGLGrid);
        allShaders.push_back(shaders.shaderGroundCircle);
        allShaders.push_back(shaders.shaderGroundDecal);
        allShaders.push_back(shaders.shaderAxisQuad);
        allShaders.push_back(shaders.shaderOGLRect);
        allShaders.push_back(shaders.shaderComputeParticles);
        allShaders.push_back(shaders.shaderGPUParticles);
        allShaders.push_back(shaders.shaderCircle2D);

    for (auto &s : allShaders) {
        s->PrepareSync();
    }

    for (auto &s : allShaders) {
        LOG_MESSAGE("[Render] Register programID=%d (%s)", s->getProgramID(), s->getVertexFilename().c_str());
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

    selection.update();
    onUpdateSceneObjects();

    if (Brakeza::get()->GUI()->isWindowOpen(GUIType::DEPTH_LIGHTS_MAPS)) {
        shaders.shaderShadowPassDebugLight->CreateFramebuffer();
        shaders.shaderShadowPassDebugLight->createArrayTextures(numSpotLights);
        shaders.shaderShadowPassDebugLight->updateDebugTextures(numSpotLights);
    }

    if (SETUP->ENABLE_GRID_BACKGROUND && !Components::get()->Scripting()->isExecuting()) {
        shaders.shaderOGLGrid->render(Components::get()->Window()->getBackgroundFramebuffer());
    }
}

void ComponentRender::postUpdate()
{
    auto sceneObjects = Brakeza::get()->copySceneObjects();

    for (auto &o: sceneObjects) {
        if (!o->isEnabled()) continue;
        if (!isInFrustum(o)) continue;
        o->postUpdate();
    }

    RenderAvatars();
    textWriter->flushTextBatchToFB("foreground");
}

void ComponentRender::RenderAvatars()
{
    if (!Config::get()->SHOW_AVATARS) return;
    if (!Config::get()->ENABLE_IMGUI) return;
    if (Components::get()->Scripting()->isExecuting()) return;

    auto* gui = Brakeza::get()->GUI();
    if (!gui) return;
    auto* atlas = gui->getTextureAtlas();
    if (!atlas) return;

    auto* window = Components::get()->Window();
    if (!window) return;

    const int screenW = Config::get()->screenWidth;
    const int screenH = Config::get()->screenHeight;
    const GLuint uiFBO = window->getUIFramebuffer();

    auto sceneObjects = Brakeza::get()->copySceneObjects();
    for (auto* obj : sceneObjects) {
        if (!obj->isEnabled() || obj->isRemoved()) continue;
        if (!obj->showAvatar) continue;
        if (!isAvatarTypeEnabled(obj->getTypeObject())) continue;

        GUIType::Sheet icon = obj->getIcon();
        Image* iconImage = atlas->getTextureByXY(icon.x, icon.y);
        if (!iconImage || !iconImage->isLoaded()) continue;

        glm::vec4 clip = Components::get()->Camera()->getGLMMat4ProjectionMatrix()
            * Components::get()->Camera()->getGLMMat4ViewMatrix()
            * glm::vec4(obj->getPosition().toGLM(), 1.0f);
        if (clip.w <= 0.0f) continue;

        glm::vec3 ndc = glm::vec3(clip) / clip.w;
        int sx = (int)((ndc.x + 1.0f) * 0.5f * (float)screenW);
        int sy = (int)((1.0f - ndc.y) * 0.5f * (float)screenH);

        if (sx < -32 || sx > screenW + 32 || sy < -32 || sy > screenH + 32) continue;

        const int avatarSize = 24;

        iconImage->DrawFlatAlpha(
            sx - avatarSize / 2,
            sy - avatarSize / 2,
            avatarSize, avatarSize,
            1.0f, uiFBO
        );
    }
}

bool ComponentRender::isAvatarTypeEnabled(ObjectType type)
{
    auto* cfg = Config::get();
    switch (type) {
        case ObjectType::Object3D:             return cfg->SHOW_AVATAR_OBJECT3D;
        case ObjectType::Mesh3D:               return cfg->SHOW_AVATAR_MESH3D;
        case ObjectType::Mesh3DAnimation:      return cfg->SHOW_AVATAR_MESH3D_ANIMATION;
        case ObjectType::LightPoint:           return cfg->SHOW_AVATAR_LIGHT_POINT;
        case ObjectType::LightSpot:            return cfg->SHOW_AVATAR_LIGHT_SPOT;
        case ObjectType::ParticleEmitter:      return cfg->SHOW_AVATAR_PARTICLE_EMITTER;
        case ObjectType::Image3DAnimation:     return cfg->SHOW_AVATAR_IMAGE3D_ANIMATION;
        case ObjectType::Image3DAnimation360:  return cfg->SHOW_AVATAR_IMAGE3D_ANIMATION360;
        case ObjectType::Image2DAnimation:     return cfg->SHOW_AVATAR_IMAGE2D_ANIMATION;
        case ObjectType::Image3D:              return cfg->SHOW_AVATAR_IMAGE3D;
        case ObjectType::Image2D:              return cfg->SHOW_AVATAR_IMAGE2D;
        case ObjectType::Swarm:                return cfg->SHOW_AVATAR_SWARM;
        case ObjectType::Sound3D:              return cfg->SHOW_AVATAR_SOUND3D;
    }
    return true;
}

Object3D* ComponentRender::hitTestAvatar(int screenX, int screenY) const
{
    if (!Config::get()->SHOW_AVATARS) return nullptr;
    if (!Config::get()->ENABLE_IMGUI) return nullptr;

    auto* window = Components::get()->Window();
    const int screenW = Config::get()->screenWidth;
    const int screenH = Config::get()->screenHeight;
    const int renderW = window->getWidthRender();
    const int renderH = window->getHeightRender();
    const int winW = window->getWidth();
    const int winH = window->getHeight();
    const int avatarSize = 24;

    Object3D* best = nullptr;
    float bestDepth = std::numeric_limits<float>::max();

    auto sceneObjects = Brakeza::get()->copySceneObjects();
    for (auto* obj : sceneObjects) {
        if (!obj->isEnabled() || obj->isRemoved()) continue;
        if (!obj->showAvatar) continue;
        if (!isAvatarTypeEnabled(obj->getTypeObject())) continue;

        glm::vec4 clip = Components::get()->Camera()->getGLMMat4ProjectionMatrix()
            * Components::get()->Camera()->getGLMMat4ViewMatrix()
            * glm::vec4(obj->getPosition().toGLM(), 1.0f);
        if (clip.w <= 0.0f) continue;

        glm::vec3 ndc = glm::vec3(clip) / clip.w;
        int sx = (int)((ndc.x + 1.0f) * 0.5f * (float)screenW);
        int sy = (int)((1.0f - ndc.y) * 0.5f * (float)screenH);

        // Compute window-space rect matching DrawFlat + FlipGlobalToWindow truncation chain
        int screenRectX = sx - avatarSize / 2;
        int screenRectY = sy - avatarSize / 2;
        int renderRectX = screenRectX * renderW / screenW;
        int renderRectY = screenRectY * renderH / screenH;
        int renderRectW = avatarSize * renderW / screenW;
        int renderRectH = avatarSize * renderH / screenH;
        int winRectX = renderRectX * winW / renderW;
        int winRectY = renderRectY * winH / renderH;
        int winRectW = renderRectW * winW / renderW;
        int winRectH = renderRectH * winH / renderH;

        if (screenX >= winRectX && screenX < winRectX + winRectW &&
            screenY >= winRectY && screenY < winRectY + winRectH)
        {
            float depth = ndc.z;
            if (depth < bestDepth) {
                bestDepth = depth;
                best = obj;
            }
        }
    }
    return best;
}

void ComponentRender::onEnd()
{
}

void ComponentRender::onSDLPollEvent(SDL_Event *event, bool &finish)
{
    selection.processSDLEvent(event);
}

void ComponentRender::updateFrustum()
{
    auto* cam = Components::get()->Camera();
    glm::mat4 vp = cam->getGLMMat4ProjectionMatrix() * cam->getGLMMat4ViewMatrix();

    // Gribb-Hartmann: extract 6 frustum pla     nes from the VP matrix (GLM column-major)
    auto extract = [&](float sign, int row) {
        FrustumPlane p;
        p.nx = vp[0][3] + sign * vp[0][row];
        p.ny = vp[1][3] + sign * vp[1][row];
        p.nz = vp[2][3] + sign * vp[2][row];
        p.d  = vp[3][3] + sign * vp[3][row];
        float len = sqrtf(p.nx*p.nx + p.ny*p.ny + p.nz*p.nz);
        if (len > 1e-5f) { p.nx /= len; p.ny /= len; p.nz /= len; p.d /= len; }
        return p;
    };

    frustumPlanes[0] = extract(+1.f, 0); // Left
    frustumPlanes[1] = extract(-1.f, 0); // Right
    frustumPlanes[2] = extract(+1.f, 1); // Bottom
    frustumPlanes[3] = extract(-1.f, 1); // Top
    frustumPlanes[4] = extract(+1.f, 2); // Near
    frustumPlanes[5] = extract(-1.f, 2); // Far
}

bool ComponentRender::isInFrustum(const Object3D *o, float radiusOverride)
{
    if (!Config::get()->ENABLE_FRUSTUM_CULLING) return true;

    auto type = o->getTypeObject();

    // Image2D is screen-space — never cull
    if (type == ObjectType::Image2D)
        return true;

    // Lights: cull by sphere radius (auto or manual)
    if (type == ObjectType::LightPoint || type == ObjectType::LightSpot) {
        auto *light = static_cast<const LightPoint*>(o);
        if (!light) return true;

        float lr;
        if (radiusOverride >= 0.0f) {
            lr = radiusOverride;
        } else if (light->frustumCullingEnabled) {
            lr = light->frustumCullingOffset;
        } else {
            return true;
        }

        const Vertex3D &lpos = o->getPosition();
        for (const auto& p : frustumPlanes) {
            if (p.nx * lpos.x + p.ny * lpos.y + p.nz * lpos.z + p.d < -lr)
                return false;
        }
        return true;
    }

    if (!o->getRenderSettings().frustumCulling)
        return true;

    const Vertex3D &pos = o->getPosition();
    float r = o->getBoundingRadius();

    for (const auto& p : frustumPlanes) {
        if (p.nx * pos.x + p.ny * pos.y + p.nz * pos.z + p.d < -r)
            return false;
    }
    return true;
}

void ComponentRender::onUpdateSceneObjects()
{
    auto sceneObjects = Brakeza::get()->copySceneObjects();

    sortFrameTime += Brakeza::get()->getDeltaTimeMicro();
    if (sortFrameTime >= Config::get()->SORT_OBJECTS_INTERVAL_MS) {
        std::sort(sceneObjects.begin(), sceneObjects.end(), compareDistances);
        sortFrameTime -= Config::get()->SORT_OBJECTS_INTERVAL_MS;
    }

    updateFrustum();

    // Pasada 1: scripts en TODOS los objetos activos, independientemente del frustum
    Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), "Scripts");
    for (const auto &o : sceneObjects) {
        if (!o->isEnabled()) continue;
        o->onUpdateScripts();
    }
    Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "Scripts");

    // Pasada 2: render solo para objetos visibles (scripts ya ejecutados, no se repiten)
    Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), "GBuffer");
    int visible = 0, culled = 0;
    for (const auto &o: sceneObjects) {
        if (!o->isEnabled()) continue;
        const bool inFrustum = isInFrustum(o);
        o->setVisibleInFrustum(inFrustum);
        if (!inFrustum) { ++culled; continue; }
        ++visible;
        o->onUpdate();
    }
    lastFrameVisible = visible;
    lastFrameCulled  = culled;
    Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "GBuffer");
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


void ComponentRender::DeleteRemovedObjects()
{
    auto &sceneObjects = Brakeza::get()->getSceneObjects();
    auto lock = Brakeza::get()->uniqueLockObjects();
    sceneObjects.erase(
        std::remove_if(
            sceneObjects.begin(),
            sceneObjects.end(), [](Object3D* object) {
                if (object->isRemoved()) {
                    auto *render = Components::get()->Render();
                    if (render->isObjectInSelection(object)) {
                        render->removeFromSelection(object);
                    }
                    Brakeza::get()->removeObjectFromIndex(object);
                    delete object;
                    return true;
                }
                return false;
            }
        ),
        sceneObjects.end()
    );
}

ShaderBaseCustom* ComponentRender::LoadShaderIntoScene(const std::string &filePath)
{
    auto metaInfo = ShadersGUI::ExtractShaderCustomCodeMetainfo(filePath);

    if (ShaderBaseCustom::getShaderTypeFromString(metaInfo.type) == SHADER_POSTPROCESSING ||
        ShaderBaseCustom::getShaderTypeFromString(metaInfo.type) == SHADER_NODE_POSTPROCESSING
    ) {
        auto shader = CreateCustomShaderFromDisk(metaInfo, nullptr);

        if (shader != nullptr) {
            AddShaderToScene(shader);
            return shader;
        }
    }

    LOG_ERROR("[Render] Error: Cannot apply shader to scene...");
    return nullptr;
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
    auto w = window->getWidthRender();
    auto h = window->getHeightRender();

    if (w <= 0 || h <= 0) {
        Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "PostProcessingShadersChain");
        return;
    }

    if (!Config::get()->ENABLE_POST_PROCESSING_CHAIN) {
        Components::get()->Render()->getShaders()->shaderOGLImage->renderTexture(
            window->getSceneTexture(), 0, 0, w, h, w, h, 1, true,
            window->getGlobalFramebuffer()
        );
        Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "PostProcessingShadersChain");
        return;
    }

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

    if (index >= 0 && static_cast<size_t>(index) < sceneShaders.size()) {
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

void ComponentRender::clearSceneShaders()
{
    for (auto s : sceneShaders) delete s;
    sceneShaders.clear();
    LOG_MESSAGE("[ComponentRender] clearSceneShaders: all scene shaders removed");
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

Vertex3D ComponentRender::getGlobalIlluminationDirection() const
{
    return Vertex3D::fromGLM(shaders.shaderOGLRender->getDirectionalLight().direction);
}

Vertex3D ComponentRender::getGlobalIlluminationAmbient() const
{
    return Vertex3D::fromGLM(shaders.shaderOGLRender->getDirectionalLight().ambient);
}

Vertex3D ComponentRender::getGlobalIlluminationDiffuse() const
{
    return Vertex3D::fromGLM(shaders.shaderOGLRender->getDirectionalLight().diffuse);
}

Vertex3D ComponentRender::getGlobalIlluminationSpecular() const
{
    return Vertex3D::fromGLM(shaders.shaderOGLRender->getDirectionalLight().specular);
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

void ComponentRender::DrawLine2D(int x1, int y1, int x2, int y2, const Color &c, float weight) const
{
    auto *win = Components::get()->Window();
    const float sx = (float)Config::get()->screenWidth  / (float)win->getWidth();
    const float sy = (float)Config::get()->screenHeight / (float)win->getHeight();
    shaders.shaderOGLLine->render(
        Point2D((int)(x1 * sx), (int)(y1 * sy)),
        Point2D((int)(x2 * sx), (int)(y2 * sy)),
        c,
        weight * (sx + sy) * 0.5f,
        win->getUIFramebuffer()
    );
}

void ComponentRender::DrawFilledRect(int x, int y, int w, int h, const Color &c) const
{
    auto *win = Components::get()->Window();
    const int rw = win->getWidthRender();
    const int rh = win->getHeightRender();
    const float rx = (float)rw / (float)win->getWidth();
    const float ry = (float)rh / (float)win->getHeight();
    shaders.shaderOGLRect->renderRect(
        (int)(x * rx), (int)(y * ry),
        (int)(w * rx), (int)(h * ry),
        rw, rh,
        c,
        win->getForegroundFramebuffer()
    );
}


void ComponentRender::DrawCircle2D(int x, int y, int size, float r, float g, float b, float a, float numWaves, float speed, float thickness, bool additive) const
{
    auto *win = Components::get()->Window();
    const int rw = win->getWidthRender();
    const int rh = win->getHeightRender();
    const float rx = (float)rw / (float)win->getWidth();
    const float ry = (float)rh / (float)win->getHeight();
    const int px = (int)((x - size / 2) * rx);
    const int py = (int)((y - size / 2) * ry);
    const int ps = (int)(size * rx);
    shaders.shaderCircle2D->renderCircle2D(
        px, py, ps, ps,
        rw, rh,
        Color(r, g, b, a),
        numWaves, speed, thickness,
        additive,
        win->getForegroundFramebuffer()
    );
}


void ComponentRender::DrawImage2D(const std::string &path, int x, int y, int w, int h)
{
    Image* img = getOrLoadImage(path);
    if (!img || !img->isLoaded()) return;

    auto *win = Components::get()->Window();
    const int rw = win->getWidthRender();
    const int rh = win->getHeightRender();
    const float rx = (float)rw / (float)win->getWidth();
    const float ry = (float)rh / (float)win->getHeight();
    shaders.shaderOGLImage->renderTexture(
        img->getOGLTextureID(),
        (int)(x * rx), (int)(y * ry),
        (int)(w * rx), (int)(h * ry),
        rw, rh,
        1.0f,
        false,
        win->getForegroundFramebuffer()
    );
}


GLuint ComponentRender::getImageGLTexture(const std::string& path)
{
    Image* img = imageCache.getOrLoad(path);
    if (!img || !img->isLoaded()) return 0;
    return img->getOGLTextureID();
}

void ComponentRender::DrawImage2DFromImage(Image *img, int x, int y, int w, int h) const
{
    if (img == nullptr || !img->isLoaded()) return;
    auto *win = Components::get()->Window();
    const int rw = win->getWidthRender();
    const int rh = win->getHeightRender();
    const float rx = (float)rw / (float)win->getWidth();
    const float ry = (float)rh / (float)win->getHeight();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shaders.shaderOGLImage->renderTexture(
        img->getOGLTextureID(),
        (int)(x * rx), (int)(y * ry),
        (int)(w * rx), (int)(h * ry),
        rw, rh,
        1.0f,
        false,
        win->getUIFramebuffer()
    );
    glDisable(GL_BLEND);
}

static GLuint s_fboOverride = 0;

void ComponentRender::setFBOOverride(GLuint fbo)  { s_fboOverride = fbo; }
void ComponentRender::clearFBOOverride()           { s_fboOverride = 0;   }

static GLuint resolveFB(const std::string& fb)
{
    if (s_fboOverride) return s_fboOverride;
    auto* win = Components::get()->Window();
    if (fb == "scene")      return win->getSceneFramebuffer();
    if (fb == "background") return win->getBackgroundFramebuffer();
    if (fb == "ui")         return win->getUIFramebuffer();
    if (fb == "global")     return win->getGlobalFramebuffer();
    return win->getForegroundFramebuffer();
}

GLuint ComponentRender::resolveEffectiveFBO(const std::string& fb) { return resolveFB(fb); }

void ComponentRender::DrawWidgetCacheToFB(GLuint tex, int rW, int rH, const std::string& fb) const
{
    shaders.shaderOGLImage->renderTexture(tex, 0, 0, rW, rH, rW, rH, 1.0f, true, resolveFB(fb));
}

void ComponentRender::DrawFilledRectToFB(int x, int y, int w, int h, const Color &c, const std::string &fb) const
{
    auto *win = Components::get()->Window();
    const int rw = win->getWidthRender();
    const int rh = win->getHeightRender();
    const float rx = (float)rw / (float)win->getWidth();
    const float ry = (float)rh / (float)win->getHeight();
    shaders.shaderOGLRect->renderRect(
        (int)(x * rx), (int)(y * ry),
        (int)(w * rx), (int)(h * ry),
        rw, rh,
        c,
        resolveFB(fb)
    );
}

void ComponentRender::DrawCircle2DToFB(int x, int y, int size, float r, float g, float b, float a, float numWaves, float speed, float thickness, bool additive, const std::string &fb) const
{
    auto *win = Components::get()->Window();
    const int rw = win->getWidthRender();
    const int rh = win->getHeightRender();
    const float rx = (float)rw / (float)win->getWidth();
    const float ry = (float)rh / (float)win->getHeight();
    const int px = (int)((x - size / 2) * rx);
    const int py = (int)((y - size / 2) * ry);
    const int ps = (int)(size * rx);
    shaders.shaderCircle2D->renderCircle2D(
        px, py, ps, ps,
        rw, rh,
        Color(r, g, b, a),
        numWaves, speed, thickness,
        additive,
        resolveFB(fb)
    );
}

Image* ComponentRender::getOrLoadImage(const std::string &path)
{
    auto it = renderImageCache.find(path);
    if (it != renderImageCache.end()) return it->second;
    Image* img = imageCache.getOrLoad(path);
    renderImageCache[path] = img;
    return img;
}

void ComponentRender::DrawImage2DToFB(const std::string &path, int x, int y, int w, int h, const std::string &fb, float alpha)
{
    DrawImage2DFromImageToFB(getOrLoadImage(path), x, y, w, h, fb, alpha);
}

void ComponentRender::DrawImage2DFromImageToFB(Image* img, int x, int y, int w, int h, const std::string &fb, float alpha)
{
    if (!img || !img->isLoaded()) return;

    auto *win = Components::get()->Window();
    const int rw = win->getWidthRender();
    const int rh = win->getHeightRender();
    const float rx = (float)rw / (float)win->getWidth();
    const float ry = (float)rh / (float)win->getHeight();
    shaders.shaderOGLImage->renderTexture(
        img->getOGLTextureID(),
        (int)(x * rx), (int)(y * ry),
        (int)(w * rx), (int)(h * ry),
        rw, rh,
        alpha,
        false,
        resolveFB(fb)
    );
}

void ComponentRender::drawGroundCircle(Object3D* obj, float r, float g, float b, float a, float radius) const
{
    if (!obj) return;
    shaders.shaderGroundCircle->draw(obj, Color(r, g, b, a), radius, Components::get()->Window()->getForegroundFramebuffer());
}

void ComponentRender::drawGroundCircle(Object3D* obj, float r, float g, float b, float a, float radius, float thickness) const
{
    if (!obj) return;
    shaders.shaderGroundCircle->draw(obj, Color(r, g, b, a), radius, Components::get()->Window()->getForegroundFramebuffer(), thickness);
}

void ComponentRender::drawGroundCircleToFB(Object3D* obj, float r, float g, float b, float a, float radius, const std::string& fb) const
{
    if (!obj) return;
    shaders.shaderGroundCircle->draw(obj, Color(r, g, b, a), radius, resolveFB(fb));
}

void ComponentRender::drawGroundBlob(Object3D* obj, float r, float g, float b, float a, float radius) const
{
    if (!obj) return;
    shaders.shaderGroundCircle->draw(obj, Color(r, g, b, a), radius, Components::get()->Window()->getForegroundFramebuffer(), 0.10f, true);
}

void ComponentRender::drawGroundBlobToFB(Object3D* obj, float r, float g, float b, float a, float radius, const std::string& fb) const
{
    if (!obj) return;
    shaders.shaderGroundCircle->draw(obj, Color(r, g, b, a), radius, resolveFB(fb), 0.10f, true);
}

void ComponentRender::drawGroundDecal(Object3D* obj, const std::string& texturePath, float r, float g, float b, float a, float radius) const
{
    if (!obj) return;
    shaders.shaderGroundDecal->draw(obj, texturePath, Color(r, g, b, a), radius, Components::get()->Window()->getForegroundFramebuffer());
}

void ComponentRender::drawGroundDecalToFB(Object3D* obj, const std::string& texturePath, float r, float g, float b, float a, float radius, const std::string& fb) const
{
    if (!obj) return;
    shaders.shaderGroundDecal->draw(obj, texturePath, Color(r, g, b, a), radius, resolveFB(fb));
}

void ComponentRender::drawAxisQuad(Object3D* obj, float r, float g, float b, float a, float halfSize, ShaderOGLAxisQuad::Axis axis) const
{
    if (!obj) return;
    shaders.shaderAxisQuad->draw(obj, Color(r, g, b, a), halfSize, Components::get()->Window()->getForegroundFramebuffer(), axis);
}

void ComponentRender::drawAxisQuadAt(const Vertex3D& pos, float r, float g, float b, float a, float halfSize, ShaderOGLAxisQuad::Axis axis) const
{
    shaders.shaderAxisQuad->drawAt(pos, Color(r, g, b, a), halfSize, Components::get()->Window()->getForegroundFramebuffer(), axis);
}

void ComponentRender::drawOutlineSubmesh(Object3D* obj, const std::string& submeshName, float r, float g, float b, float a, float thickness) const
{
    if (!obj) return;
    auto* mesh = dynamic_cast<Mesh3D*>(obj);
    if (!mesh) return;
    shaders.shaderOGLOutline->drawOutlineSubmesh(mesh, submeshName, Color(r, g, b, a), thickness, Components::get()->Window()->getForegroundFramebuffer());
}

void ComponentRender::clearOutlineBatch() const
{
    shaders.shaderOGLOutline->clearOutlineBatch();
}

void ComponentRender::drawOutlineSubmeshBatch(Object3D* obj, const std::string& submeshName, float r, float g, float b, float a, float thickness) const
{
    if (!obj) return;
    auto* mesh = dynamic_cast<Mesh3D*>(obj);
    if (!mesh) return;
    shaders.shaderOGLOutline->drawOutlineSubmeshBatch(mesh, submeshName, Color(r, g, b, a), thickness);
}

void ComponentRender::flushOutlines() const
{
    shaders.shaderOGLOutline->flushOutlines(Components::get()->Window()->getForegroundFramebuffer());
}

Vertex3D ComponentRender::getSubmeshCenter(Object3D* obj, const std::string& submeshName) const
{
    if (!obj) return Vertex3D::zero();
    auto* mesh = dynamic_cast<Mesh3D*>(obj);
    if (!mesh) return obj->getPosition();

    for (const auto& md : mesh->getMeshData()) {
        if (md.name == submeshName) {
            Vertex3D localCenter = md.localAabb.getCenter();
            glm::mat4 model = mesh->getModelMatrix();
            glm::vec4 world = model * glm::vec4(localCenter.x, localCenter.y, localCenter.z, 1.0f);
            return Vertex3D(world.x, world.y, world.z);
        }
    }
    return obj->getPosition();
}

void ComponentRender::DrawCircle3D(Vertex3D center, float radius, float r, float g, float b, float a) const
{
    constexpr int SEGMENTS = 32;
    constexpr float TWO_PI = 6.28318530718f;
    std::vector<Vector3D> lines;
    lines.reserve(SEGMENTS);
    for (int i = 0; i < SEGMENTS; i++) {
        float a0 = (float)i       / SEGMENTS * TWO_PI;
        float a1 = (float)(i + 1) / SEGMENTS * TWO_PI;
        Vertex3D v0(center.x + radius * cosf(a0), center.y, center.z + radius * sinf(a0));
        Vertex3D v1(center.x + radius * cosf(a1), center.y, center.z + radius * sinf(a1));
        lines.push_back(Vector3D(v0, v1));
    }
    shaders.shaderOGLLine3D->renderLines(lines, Components::get()->Window()->getForegroundFramebuffer(), Color(r, g, b, a));
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
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    setLastFrameBufferUsed(framebuffer);
    Profiler::get()->incrementFboChanges();
}

void ComponentRender::ChangeOpenGLProgram(GLuint programID)
{
    glUseProgram(programID);
    setLastProgramUsed(programID);
    Profiler::get()->incrementProgramChanges();
}

void ComponentRender::resizeShadersFramebuffers() const
{
    LOG_SUCCESS("[Render] Resizing framebuffers...");

    shaders.shaderOGLRender->Destroy();
    shaders.shaderOGLImage->Destroy();
    shaders.shaderOGLLine->Destroy();
    shaders.shaderOGLWireframe->Destroy();
    shaders.shaderOGLShading->Destroy();
    shaders.shaderOGLPoints->Destroy();
    shaders.shaderOGLOutline->Destroy();
    shaders.shaderOGLColor->Destroy();
    shaders.shaderOGLParticles->Destroy();
    shaders.shaderOGLDepthMap->Destroy();
    shaders.shaderOGLGBuffer->Destroy();
    shaders.shaderOGLLightPass->Destroy();
    shaders.shaderGroundCircle->Destroy();
    shaders.shaderGroundDecal->Destroy();
    shaders.shaderAxisQuad->Destroy();
    // shaderComputeParticles and shaderGPUParticles own no size-dependent resources
    // (no FBOs). Destroying them here would null their programID/VAO with no
    // rebuild path — they must survive a resize unchanged.

    if (Config::get()->ENABLE_SHADOW_MAPPING) {
        shaders.shaderShadowPass->createSpotLightsDepthTextures((int) shaders.shaderOGLRender->getShadowMappingSpotLights().size());
        shaders.shaderShadowPass->ResetFramebuffers();
    }

    // Scene (post-processing) shaders — code-based ones own a resultFramebuffer
    // and internalTexture that are sized at init.  PostProcessingManager recreates
    // its own ping-pong FBOs separately, but the shader's own FBO/texture and quad
    // matrices must also be updated so the GL state stays consistent after resize.
    for (auto shader : sceneShaders) {
        if (auto *codeShader = dynamic_cast<ShaderOGLCustomCodePostprocessing*>(shader)) {
            codeShader->Destroy();
        }
    }

    // Object shader chains — each Mesh3D owns a Mesh3DShaderChain with its own
    // ping/pong FBOs that mirror the GBuffer layout.  These must be resized too.
    auto window = Components::get()->Window();
    int w = window->getWidthRender();
    int h = window->getHeightRender();
    auto sceneObjects = Brakeza::get()->copySceneObjects();
    for (auto *obj : sceneObjects) {
        auto *mesh = dynamic_cast<Mesh3D*>(obj);
        if (mesh && !mesh->getCustomShaders().empty()) {
            auto *chain = mesh->GetShaderChain();
            if (chain) {
                chain->Resize(w, h);
            }
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentRender::FillOGLBuffers(std::vector<Mesh3DData> &meshes, bool withFeedbackBuffers)
{
    for (auto &m: meshes) {
        if (m.vertices.empty() || m.uvs.empty() || m.normals.empty()) {
            LOG_ERROR("[FillOGLBuffers] mesh with empty geometry (vertices=%zu uvs=%zu normals=%zu) — skipped",
                m.vertices.size(), m.uvs.size(), m.normals.size());
            continue;
        }

        glGenBuffers(1, &m.vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(m.vertices.size() * sizeof(glm::vec4)), m.vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &m.uvBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m.uvBuffer);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(m.uvs.size() * sizeof(glm::vec2)), m.uvs.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &m.normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m.normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(m.normals.size() * sizeof(glm::vec3)), m.normals.data(), GL_STATIC_DRAW);

        if (withFeedbackBuffers) {
            glGenBuffers(1, &m.feedbackBuffer);
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m.feedbackBuffer);
            glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, static_cast<GLuint>(m.vertices.size() * sizeof(glm::vec4)), m.vertices.data(), GL_DYNAMIC_COPY);

            glGenBuffers(1, &m.feedbackNormalBuffer);
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m.feedbackNormalBuffer);
            glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, static_cast<GLuint>(m.normals.size() * sizeof(glm::vec3)), m.normals.data(), GL_DYNAMIC_COPY);

            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
        }
    }
}

void ComponentRender::ClearShadowMaps() const
{
    auto numLights = (int) shaders.shaderOGLRender->getShadowMappingSpotLights().size();

    Components::get()->Render()->ChangeOpenGLFramebuffer(shaders.shaderShadowPass->getDirectionalLightDepthMapFBO());
    glClear(GL_DEPTH_BUFFER_BIT);

    if (numLights <= 0) return;

    Components::get()->Render()->ChangeOpenGLFramebuffer(shaders.shaderShadowPass->getSpotLightsDepthMapsFBO());
    glClear(GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < numLights; i++) {
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shaders.shaderShadowPass->getSpotLightsShadowMapArrayTextures(), 0, i);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void ComponentRender::RunShadowPass() const
{
    Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), "ShadowPass");

    if (!Config::get()->ENABLE_SHADOW_MAPPING || !Config::get()->ENABLE_LIGHTS) {
        Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "ShadowPass");
        return;
    }

    auto shadowPass  = shaders.shaderShadowPass;
    auto shaderRender = shaders.shaderOGLRender;

    std::vector<Mesh3D*> casters;
    auto sceneObjects = Brakeza::get()->copySceneObjects();
    for (auto* obj : sceneObjects) {
        if (!obj->isEnabled()) continue;

        if (auto* anim = dynamic_cast<Mesh3DAnimation*>(obj)) {
            if (anim->isEnableLights())
                casters.push_back(anim);
            continue;
        }
        if (auto* mesh = dynamic_cast<Mesh3D*>(obj)) {
            if (mesh->isEnableLights() && mesh->getRenderSettings().shadowMap)
                casters.push_back(mesh);
        }
    }

    if (!casters.empty()) {
        shadowPass->renderSceneDirectionalLight(casters, shaderRender->getDirectionalLight());

        const auto& spotLights = shaderRender->getShadowMappingSpotLights();
        for (int i = 0; i < static_cast<int>(spotLights.size()); i++) {
            shadowPass->renderSceneSpotLight(casters, spotLights[i], i);
        }
    }

    Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "ShadowPass");
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

    if (Config::get()->ENABLE_TRIANGLE_MODE_DEPTHMAP) {
        shaders.shaderOGLDepthMap->Render(gBuffer.depth, globalBuffer.foregroundFBO);
    }

    if (Config::get()->TRIANGLE_MODE_PICKING_COLORS) {
        shaders.shaderOGLImage->renderTexture(
            window->getPickingColorFramebuffer().rbgTexture, 0, 0, w, h, w, h, 1, true, globalBuffer.foregroundFBO
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

    delete uiManager;
    delete glyphAtlas;
    delete textWriter;
}

void ComponentRender::clearEngineCache()
{
    imageCache.resetStats();
    modelDataCache.resetStats();
    animationDataCache.resetStats();
    scriptDataCache.resetStats();
    imageCache.clear();
    modelDataCache.clear();
    animationDataCache.clear();
    scriptDataCache.clear();
}
