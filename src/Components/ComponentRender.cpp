#include "imgui.h"
#include <algorithm>
#include <chrono>
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
#include <unordered_map>
#include <iostream>
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
    CreateCameraUBO();

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

void ComponentRender::onUpdate()
{
    if (!isEnabled()) return;

    // Fase 3: un solo snapshot de escena para las dos cosas que lo necesitan aquí (extracción de
    // luces + scripts/GBuffer) -- antes cada una hacía su propio copySceneObjects(). Seguro
    // porque no corre nada entre medias que añada/quite objetos de la escena.
    auto sceneObjects = Brakeza::get()->copySceneObjects();

    shaders.shaderOGLRender->CreateUBOFromLights(sceneObjects);
    UpdateCameraUBO();

    auto numSpotLights = shaders.shaderOGLRender->getNumSpotLights();

    selection.update();
    onUpdateSceneObjects(sceneObjects);

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
    Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), "Transparencies");
    Profiler::get()->StartGpuMeasure("Transparencies");
    auto sceneObjects = Brakeza::get()->copySceneObjects();

    // Fase 1.1.1: este bucle también dibuja vía ShaderOGLRenderForward (objetos transparentes),
    // que comparte la misma caché de estado GL que el bucle de GBuffer -- invalidar antes (por si
    // RunShadowPass()/LightPass(), que corren entre medias, tocaron algo) y restaurar a un estado
    // conocido al salir, ya que justo después vienen FlipBuffersToGlobal()/
    // PostProcessingShadersChain() (FogOfWar incluido), que no participan de la caché.
    InvalidateRenderStateCache();
    for (auto &o: sceneObjects) {
        if (!o->isEnabled()) continue;
        if (!isInFrustum(o)) continue;
        o->postUpdate();
    }
    RestoreDefaultRenderState();
    Profiler::get()->EndGpuMeasure("Transparencies");
    Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "Transparencies");

    RenderAvatars();
    textWriter->flushTextBatchToFB("foreground");

    // Tooltips de la UI de juego, lo ÚLTIMO del frame en la capa "ui": los componentes se ejecutan
    // en orden fijo (Scripting antes que Render), así que aquí ya corrieron todos los onUpdate/
    // postUpdate de Lua y ningún widget se pinta encima. Antes lo llamaba HUDManager.lua a mitad de
    // frame y lo tapaba lo que otros scripts dibujaban después (y los widgets de otras capas).
    if (uiManager) uiManager->flushTooltip(Brakeza::get()->getDeltaTime());
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

void ComponentRender::onUpdateSceneObjects(std::vector<Object3D*> &sceneObjects)
{
    sortFrameTime += Brakeza::get()->getDeltaTimeMS();
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
    Profiler::get()->StartGpuMeasure("GBuffer");
    // Fase 1.1.1: Deferred/Forward comparten una caché de estado GL (ApplyBlend/ApplyDepthTest/...)
    // dentro de este bucle -- invalidar aquí para no fiarnos de lo que dejara el frame anterior
    // (o cualquier otro paso), y restaurar a un estado por defecto conocido al salir para que
    // RunShadowPass()/LightPass(), que no participan de la caché, sigan viendo lo mismo que veían
    // antes de esta optimización.
    // onUpdateSceneObjects() es static (sin `this`) -- pasar por la instancia, como ya hace
    // el resto del motor (ChangeOpenGLFramebuffer/ChangeOpenGLProgram) para llamar a métodos
    // no estáticos de ComponentRender desde aquí.
    Components::get()->Render()->InvalidateRenderStateCache();
    int visible = 0, culled = 0;
    for (const auto &o: sceneObjects) {
        if (!o->isEnabled()) continue;
        const bool inFrustum = isInFrustum(o);
        o->setVisibleInFrustum(inFrustum);
        if (!inFrustum) { ++culled; continue; }
        ++visible;
        o->onUpdate();
    }
    // Fase 3 (Etapa 1): los Mesh3D estáticos/Deferred se encolaron durante el bucle de arriba
    // (Mesh3D::onUpdate() -> EnqueueOpaque) en vez de dibujarse al instante -- se dibujan todos
    // aquí, ordenados. RestoreDefaultRenderState() se mueve a DESPUÉS del flush a propósito: es
    // el flush quien hace ahora el último draw antes de RunShadowPass()/LightPass(), no el
    // último objeto del bucle.
    Components::get()->Render()->FlushOpaqueQueue();
    // Los custom shaders de objeto (WaterRTS, etc.) van DESPUES del flush opaco a propósito: si
    // corrieran entrelazados por objeto (como antes de Fase 3), el flush opaco -- que se ejecuta
    // una sola vez al final para TODOS los objetos -- repintaría encima el material sin animar de
    // cualquier objeto procesado antes que el último de sceneObjects (visible como una "capa"
    // blanca estática y sin desplazar por encima del agua animada).
    Components::get()->Render()->FlushObjectShaderQueue();
    // Emisivos SIEMPRE los últimos en escribir el G-Buffer -- ver comentario de FlushEmissiveQueue.
    Components::get()->Render()->FlushEmissiveQueue();
    Components::get()->Render()->RestoreDefaultRenderState();
    // Fase 4b: picking (MOUSE_CLICK_SELECT_OBJECT3D) se encoló igual que el G-Buffer -- se dibuja
    // aquí, agrupado. Va después de RestoreDefaultRenderState() a propósito: escribe en su propio
    // FBO (picking, no el GBuffer) fijando su propio estado GL de forma incondicional en cada
    // draw, así que el orden respecto al resto de este bloque no cambia el resultado.
    Components::get()->Render()->FlushPickingQueue();
    lastFrameVisible = visible;
    lastFrameCulled  = culled;
    Profiler::get()->EndGpuMeasure("GBuffer");
    Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "GBuffer");
}

void ComponentRender::UpdateFPS()
{
    // DRAW_FPS_RENDER solo debe controlar si se DIBUJA el contador (ver StatusBarGUI::
    // DrawFPSCounter), no si se CALCULA -- antes, desactivar "Show FPS" en el menú ImGui del
    // editor congelaba también el contador in-game del RTS (HUDManager.lua -> render:getFps()),
    // que depende de este mismo `fps` pero se activa con un toggle propio (RTSConfig.showFPS)
    // sin ninguna relación con DRAW_FPS_RENDER. El cálculo es un incremento + comparación por
    // frame, coste nulo -- no hay motivo para condicionarlo.
    frameTime += Brakeza::get()->getDeltaTimeMS();
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

    // Fase 1: liberar de verdad lo que quedó pendiente del frame anterior. Diferir el delete
    // real un frame completo evita un use-after-free conocido: este método corre en
    // ComponentRender::preUpdate(), que se ejecuta ANTES del onUpdate() de este mismo frame
    // (Render es el último componente en preUpdate, Scripting es el segundo en onUpdate).
    // Sin este retraso, un objeto marcado removed=true durante el onUpdate() del frame N se
    // borraba aquí mismo al principio del frame N+1, justo antes de que el onUpdate() de ESE
    // frame corriera — dejando sin ninguna ventana segura a cualquier otro objeto que guarde un
    // puntero crudo hacia él (p.ej. ParticleEmitter::followTarget / attachedLight, que llaman
    // ->isRemoved() cada frame para saber si deben soltar la referencia).
    if (!pendingDeleteObjects.empty()) {
        // Una sola línea de resumen en vez de 2-3 por objeto (esas van ahora a LOG_VERBOSE).
        const auto t0 = std::chrono::steady_clock::now();
        const size_t count = pendingDeleteObjects.size();
        for (Object3D *object : pendingDeleteObjects) {
            delete object;
        }
        pendingDeleteObjects.clear();
        const double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
        LOG_MESSAGE("[Render] Deleted %zu removed objects in %.1f ms", count, ms);
    }

    // Fase 2: los recién marcados removed=true salen ya de sceneObjects y del índice por nombre
    // (para que Brakeza:getObjectByName siga devolviendo nil de inmediato, como ya asumen los
    // scripts Lua), pero el delete se pospone a la Fase 1 de la PRÓXIMA llamada.
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
                    pendingDeleteObjects.push_back(object);
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
    Profiler::get()->StartGpuMeasure("PostProcessingShadersChain");

    auto window = Components::get()->Window();
    auto w = window->getWidthRender();
    auto h = window->getHeightRender();

    if (w <= 0 || h <= 0) {
        Profiler::get()->EndGpuMeasure("PostProcessingShadersChain");
        Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "PostProcessingShadersChain");
        return;
    }

    if (!Config::get()->ENABLE_POST_PROCESSING_CHAIN) {
        Components::get()->Render()->getShaders()->shaderOGLImage->renderTexture(
            window->getSceneTexture(), 0, 0, w, h, w, h, 1, true,
            window->getGlobalFramebuffer()
        );
        Profiler::get()->EndGpuMeasure("PostProcessingShadersChain");
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

    Profiler::get()->EndGpuMeasure("PostProcessingShadersChain");
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
static int    s_overrideW = 0, s_overrideH = 0;

void ComponentRender::setFBOOverride(GLuint fbo, int w, int h) { s_fboOverride = fbo; s_overrideW = w; s_overrideH = h; }
void ComponentRender::clearFBOOverride()                        { s_fboOverride = 0; s_overrideW = 0; s_overrideH = 0; }

void ComponentRender::getTargetSize(const std::string& fb, int& w, int& h)
{
    auto* win = Components::get()->Window();
    if (s_fboOverride && s_overrideW > 0 && s_overrideH > 0) { w = s_overrideW; h = s_overrideH; return; }
    if (fb == "ui") { w = win->getWidth(); h = win->getHeight(); return; }
    w = win->getWidthRender();
    h = win->getHeightRender();
}

ComponentRender::ScopedTargetViewport::ScopedTargetViewport(int w, int h)
{
    auto* win = Components::get()->Window();
    rw = win->getWidthRender();
    rh = win->getHeightRender();
    changed = (w != rw || h != rh);
    if (changed) glViewport(0, 0, w, h);
}

ComponentRender::ScopedTargetViewport::~ScopedTargetViewport()
{
    if (changed) glViewport(0, 0, rw, rh);
}

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

void ComponentRender::DrawWidgetCacheToFB(GLuint tex, int rW, int rH, const std::string& fb, float alpha) const
{
    // The cache texture has the size of its target layer (UIManager creates it with getTargetSize)
    int tw, th;
    getTargetSize(fb, tw, th);
    ScopedTargetViewport vp(tw, th);
    shaders.shaderOGLImage->renderTexture(tex, 0, 0, tw, th, tw, th, alpha, true, resolveFB(fb));
}

void ComponentRender::DrawFilledRectToFB(int x, int y, int w, int h, const Color &c, const std::string &fb) const
{
    auto *win = Components::get()->Window();
    int rw, rh;
    getTargetSize(fb, rw, rh);
    ScopedTargetViewport vp(rw, rh);   // renderRect sets the viewport itself; this puts it back
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
    int rw, rh;
    getTargetSize(fb, rw, rh);
    ScopedTargetViewport vp(rw, rh);
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

void ComponentRender::DrawImage2DNineSliceToFB(Image *img, float x, float y, float w, float h,
                                               const float slice[4], float sliceScale, const std::string &fb, float alpha)
{
    if (!img || !img->isLoaded()) return;
    const float imgW = (float)img->width(), imgH = (float)img->height();
    if (imgW <= 0.0f || imgH <= 0.0f || w <= 0.0f || h <= 0.0f) return;

    auto *win = Components::get()->Window();
    int rw, rh;
    getTargetSize(fb, rw, rh);
    ScopedTargetViewport vp(rw, rh);
    const float rx = (float)rw / (float)win->getWidth();
    const float ry = (float)rh / (float)win->getHeight();

    // Destination edges in TARGET px, each computed once and shared by the pieces on both sides
    // (piece width = next edge - edge) → no 1 px seams/overlaps from independent rounding.
    const float X0 = std::round(x * rx), X3 = std::round((x + w) * rx);
    const float Y0 = std::round(y * ry), Y3 = std::round((y + h) * ry);
    float bl = slice[0] * sliceScale * rx, br = slice[2] * sliceScale * rx;
    float bt = slice[1] * sliceScale * ry, bb = slice[3] * sliceScale * ry;
    // Panel smaller than two borders: shrink the borders proportionally so they don't overlap.
    if (bl + br > X3 - X0 && bl + br > 0.0f) { const float k = (X3 - X0) / (bl + br); bl *= k; br *= k; }
    if (bt + bb > Y3 - Y0 && bt + bb > 0.0f) { const float k = (Y3 - Y0) / (bt + bb); bt *= k; bb *= k; }
    const float X1 = X0 + std::round(bl), X2 = X3 - std::round(br);
    const float Y1 = Y0 + std::round(bt), Y2 = Y3 - std::round(bb);

    // Source cuts in UV (v from the top of the image, like the image quads).
    const float U[4] = { 0.0f, slice[0] / imgW, 1.0f - slice[2] / imgW, 1.0f };
    const float V[4] = { 0.0f, slice[1] / imgH, 1.0f - slice[3] / imgH, 1.0f };
    const float X[4] = { X0, X1, X2, X3 };
    const float Y[4] = { Y0, Y1, Y2, Y3 };

    const GLuint fbo = resolveFB(fb);
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            const int pw = (int)(X[col + 1] - X[col]);
            const int ph = (int)(Y[row + 1] - Y[row]);
            if (pw <= 0 || ph <= 0) continue;
            shaders.shaderOGLImage->renderTextureRegion(
                img->getOGLTextureID(),
                (int)X[col], (int)Y[row], pw, ph,
                rw, rh,
                U[col], V[row], U[col + 1], V[row + 1],
                alpha, fbo
            );
        }
    }
}

void ComponentRender::DrawImage2DFromImageToFB(Image* img, int x, int y, int w, int h, const std::string &fb, float alpha)
{
    if (!img || !img->isLoaded()) return;

    auto *win = Components::get()->Window();
    int rw, rh;
    getTargetSize(fb, rw, rh);
    ScopedTargetViewport vp(rw, rh);
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

// Tinte translúcido de un submesh (ShaderOGLColor::RenderTint) sobre la capa foreground. Mismo
// criterio de nombre que el contorno (ShaderOGLOutline::drawOutlineSubmesh): "BUILDING_12" casa con
// "BUILDING_12", "BUILDING_12.001"...
void ComponentRender::drawFillSubmesh(Object3D* obj, const std::string& submeshName, float r, float g, float b, float a) const
{
    if (!obj) return;
    auto* mesh = dynamic_cast<Mesh3D*>(obj);
    if (!mesh) return;

    std::string prefix = submeshName;
    const auto dot = submeshName.rfind('.');
    if (dot != std::string::npos) prefix = submeshName.substr(0, dot);

    const GLuint fbo = Components::get()->Window()->getForegroundFramebuffer();
    for (const auto& mm : mesh->getMeshData()) {
        if (mm.name.rfind(prefix, 0) != 0) continue;
        shaders.shaderOGLColor->RenderTint(mesh->getModelMatrix(), mm.vertexBuffer, mm.uvBuffer, mm.normalBuffer,
            static_cast<int>(mm.vertices.size()), Color(r, g, b, 1.0f), a, fbo, mm.indexBuffer, mm.indexCount);
    }
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
    if (framebuffer == lastFrameBufferUsed) return;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    setLastFrameBufferUsed(framebuffer);
    Profiler::get()->incrementFboChanges();
}

void ComponentRender::ChangeOpenGLProgram(GLuint programID)
{
    if (programID == lastProgramUsed) return;
    glUseProgram(programID);
    setLastProgramUsed(programID);
    Profiler::get()->incrementProgramChanges();
}

void ComponentRender::ApplyDepthTest(bool value)
{
    if (rsValid && rsDepthTest == value) return;
    value ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    rsDepthTest = value;
}

void ComponentRender::ApplyDepthFunc(GLenum value)
{
    if (rsValid && rsDepthFunc == value) return;
    glDepthFunc(value);
    rsDepthFunc = value;
}

void ComponentRender::ApplyDepthMask(bool value)
{
    if (rsValid && rsDepthMask == value) return;
    glDepthMask(value ? GL_TRUE : GL_FALSE);
    rsDepthMask = value;
}

void ComponentRender::ApplyBlend(bool value)
{
    if (rsValid && rsBlend == value) return;
    value ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    rsBlend = value;
}

void ComponentRender::ApplyBlendFunc(GLenum src, GLenum dst)
{
    if (rsValid && rsBlendSrc == src && rsBlendDst == dst) return;
    glBlendFunc(src, dst);
    rsBlendSrc = src;
    rsBlendDst = dst;
}

void ComponentRender::ApplyCulling(bool value)
{
    if (rsValid && rsCull == value) return;
    value ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    rsCull = value;
}

void ComponentRender::InvalidateRenderStateCache()
{
    rsValid = false;
}

// Fuerza el estado "por defecto" incondicionalmente (sin consultar la caché) y deja la caché
// reflejándolo. Se llama al SALIR de un bucle cacheado (GBuffer, transparencias) para que
// cualquier paso no migrado que corra después (ShadowPass, LightPass, PostProcessingShadersChain
// -- FogOfWar incluido) reciba exactamente el mismo estado que recibía antes de esta caché,
// sin importar qué RenderSettings tuviera el último objeto dibujado.
void ComponentRender::RestoreDefaultRenderState()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);

    rsBlend = true;
    rsBlendSrc = GL_SRC_ALPHA;
    rsBlendDst = GL_ONE_MINUS_SRC_ALPHA;
    rsDepthTest = true;
    rsDepthFunc = GL_LESS;
    rsDepthMask = true;
    rsCull = true;
    rsValid = true;
}

// Fase 3 (Etapa 1 estático + Etapa 2 animado): un RenderQueueEntry por submesh visible, en vez
// de dibujar al instante como hacía ShaderOGLRenderDeferred::renderMesh(). useFeedbackBuffer=true
// (Mesh3DAnimation) ya NO usa feedbackBuffer aquí (Fase 1 de instancing con skinning, ver
// .claude/plans) -- ver el comentario dentro de la función. indexBuffer/indexCount se fuerzan a 0
// para unidades animadas porque su geometría quedó fuera del EBO deduplicado en Fase 2.2.
void ComponentRender::EnqueueOpaque(Mesh3D *o, bool useFeedbackBuffer, GLuint fbo)
{
    // useFeedbackBuffer==true solo lo pasa Mesh3DAnimation::onUpdate() -- se reutiliza como señal
    // de "esto es una unidad animada" para el camino de instancing con skinning (Fase 1, ver
    // .claude/plans): en vez del feedbackBuffer horneado por-instancia (que bloqueaba el batching
    // porque nunca coincide entre instancias), se encola el vertexBuffer COMPARTIDO (bind-pose,
    // Fase 2.1) más las matrices de huesos de este frame, ya calculadas por UpdateOpenGLBones()
    // (que corre antes en Mesh3DAnimation::onUpdate(), sin coste CPU nuevo aquí). feedbackBuffer
    // sigue existiendo y sigue usándose sin cambios para picking/shadow/modos debug.
    auto* anim = useFeedbackBuffer ? dynamic_cast<Mesh3DAnimation*>(o) : nullptr;

    // Emisión desactivada por defecto: solo los Mesh3D que la activen explícitamente van a la cola
    // emisiva (ver FlushEmissiveQueue); el resto sigue exactamente el camino de siempre.
    const bool emissive = o->isEmissionEnabled() && o->getEmissionIntensity() > 0.0f;
    auto &queue = emissive ? emissiveQueue : opaqueQueue;

    const auto& textures = o->getModelTextures();
    const auto& specTextures = o->getModelSpecularTextures();
    const auto& meshData = o->getMeshData();
    for (size_t meshIdx = 0; meshIdx < meshData.size(); meshIdx++) {
        const auto& m = meshData[meshIdx];
        if (!m.visibleInFrustum) continue;
        if (m.materialIndex < 0 || (size_t)m.materialIndex >= textures.size() ||
            (size_t)m.materialIndex >= specTextures.size()) continue;
        auto* tex = textures[m.materialIndex];
        auto* specTex = specTextures[m.materialIndex];
        if (!tex || !specTex) continue;

        RenderQueueEntry entry;
        entry.o = o;
        entry.texId = tex->getOGLTextureID();
        entry.specTexId = specTex->getOGLTextureID();
        entry.vertexBuffer = m.vertexBuffer;
        entry.uvBuffer = m.uvBuffer;
        entry.normalBuffer = m.normalBuffer;
        entry.size = static_cast<int>(m.vertices.size());
        entry.alpha = o->getAlpha();
        entry.emission = emissive ? o->getEmissionIntensity() : 0.0f;
        entry.fbo = fbo;
        entry.indexBuffer = anim ? 0 : m.indexBuffer;
        entry.indexCount = anim ? 0 : m.indexCount;
        if (anim) {
            const auto& boneCache = anim->getBoneTransformCache(meshIdx);
            // Un submesh sin huesos (adjunto estático a un esqueleto animado) no necesita
            // skinning -- se deja isSkinned=false y cae al camino normal (sin cambios visuales,
            // evita un renderInstancedSkinned con bonesPerInstance<=0 que no dibujaría nada).
            if (!boneCache.empty()) {
                entry.isSkinned = true;
                entry.vertexBoneDataBuffer = m.vertexBoneDataBuffer;
                entry.boneMatrices = &boneCache;
                entry.boneCount = static_cast<int>(boneCache.size());
            }
        }
        queue.push_back(entry);
    }
}

// Ordena por (vertexBuffer, texId) -- ya agrupa por modelo/material gracias a la geometría GPU
// compartida de Fase 2.1 (misma malla = mismo vertexBuffer) -- y dibuja llamando a
// ShaderOGLRenderDeferred::render() con los mismos argumentos que ya recibía antes de esta cola.
void ComponentRender::DrawRenderQueue(std::vector<RenderQueueEntry> &queue)
{
    std::sort(queue.begin(), queue.end(), [](const RenderQueueEntry &a, const RenderQueueEntry &b) {
        if (a.vertexBuffer != b.vertexBuffer) return a.vertexBuffer < b.vertexBuffer;
        return a.texId < b.texId;
    });

    // Fase 4: runs consecutivos (tras el sort de arriba) que comparten todo salvo la matriz de
    // modelo se dibujan con UNA llamada instanciada en vez de una por entrada. alpha/drawOffset
    // entran en la comparación a propósito -- si difieren, esos objetos simplemente no se
    // agrupan y siguen su camino individual de siempre, ningún objeto pierde su valor real.
    // Fase 1 (instancing con skinning): isSkinned/vertexBoneDataBuffer/boneCount también entran
    // en la comparación -- agrupar animación exige además mismo esqueleto (implícito por
    // vertexBuffer igual, pero se verifica explícito por seguridad).
    auto sameBatch = [](const RenderQueueEntry &a, const RenderQueueEntry &b) {
        return a.vertexBuffer == b.vertexBuffer && a.uvBuffer == b.uvBuffer &&
               a.normalBuffer == b.normalBuffer && a.texId == b.texId &&
               a.specTexId == b.specTexId && a.indexBuffer == b.indexBuffer &&
               a.indexCount == b.indexCount && a.fbo == b.fbo && a.alpha == b.alpha &&
               a.o->getDrawOffset() == b.o->getDrawOffset() &&
               a.isSkinned == b.isSkinned && a.vertexBoneDataBuffer == b.vertexBoneDataBuffer &&
               a.boneCount == b.boneCount && a.emission == b.emission;
    };

    auto* deferred = getShaderOGLRenderDeferred();
    size_t i = 0;
    while (i < queue.size()) {
        size_t j = i + 1;
        while (j < queue.size() && sameBatch(queue[i], queue[j])) ++j;

        const auto &first = queue[i];
        if (first.isSkinned) {
            // Las entradas skinned van SIEMPRE por el camino instanciado, incluso un run de
            // tamaño 1 -- el feedbackBuffer horneado por-instancia ya no se usa aquí, así que no
            // hay un "render() sin skinning" válido para esta rama (ver ComponentRender::
            // EnqueueOpaque). gl_InstanceID vale 0 igual con un solo elemento en el TBO.
            std::vector<glm::mat4> models;
            std::vector<glm::mat4> allBoneMatrices;
            models.reserve(j - i);
            allBoneMatrices.reserve((j - i) * first.boneCount);
            for (size_t k = i; k < j; k++) {
                models.push_back(queue[k].o->getModelMatrix());
                const auto &bones = *queue[k].boneMatrices;
                allBoneMatrices.insert(allBoneMatrices.end(), bones.begin(), bones.end());
            }
            deferred->renderInstancedSkinned(
                first.texId, first.specTexId, first.vertexBuffer, first.uvBuffer, first.normalBuffer,
                first.vertexBoneDataBuffer, first.size, first.alpha, first.o->getDrawOffset(), first.fbo,
                first.boneCount, models, allBoneMatrices, first.emission
            );
        } else if (j - i == 1) {
            deferred->render(
                first.o, first.texId, first.specTexId, first.vertexBuffer, first.uvBuffer, first.normalBuffer,
                first.size, first.alpha, first.fbo, first.indexBuffer, first.indexCount, first.emission
            );
        } else {
            std::vector<glm::mat4> models;
            models.reserve(j - i);
            for (size_t k = i; k < j; k++) models.push_back(queue[k].o->getModelMatrix());
            deferred->renderInstanced(
                first.texId, first.specTexId, first.vertexBuffer, first.uvBuffer, first.normalBuffer,
                first.size, first.alpha, first.o->getDrawOffset(), first.fbo, first.indexBuffer, first.indexCount,
                models, first.emission
            );
        }
        i = j;
    }

    queue.clear();
}

void ComponentRender::FlushOpaqueQueue()
{
    DrawRenderQueue(opaqueQueue);
}

// Último escritor del G-Buffer del frame (va después de FlushObjectShaderQueue). Sin Mesh3D
// emisivos no hace nada: el G-Buffer se queda con sus 3 draw buffers de siempre y LightPass()
// no lee gEmission -- coste cero. Con emisivos: activa el 4º draw buffer, lo limpia a 0 (el
// resto de la escena queda sin emisión), dibuja solo los emisivos y restaura los 3 de siempre.
void ComponentRender::FlushEmissiveQueue()
{
    emissionUsedThisFrame = !emissiveQueue.empty();
    if (!emissionUsedThisFrame) return;

    // Bind explícito, no ChangeOpenGLFramebuffer(): glDrawBuffers es estado DEL FBO enlazado y la
    // caché de lastFrameBufferUsed puede estar desfasada tras los custom shaders de objeto.
    const auto &gBuffer = Components::get()->Window()->getGBuffer();
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.FBO);
    setLastFrameBufferUsed(gBuffer.FBO);

    static const GLenum withEmission[4] = {
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
    };
    glDrawBuffers(4, withEmission);
    static const GLfloat zero[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearBufferfv(GL_COLOR, 3, zero);   // índice 3 = 4º draw buffer = gBuffer.emission

    DrawRenderQueue(emissiveQueue);

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.FBO);   // mismo motivo que arriba
    setLastFrameBufferUsed(gBuffer.FBO);
    glDrawBuffers(3, withEmission);
}

// El G-Buffer del pase opaco (FlushOpaqueQueue) y el de los custom shaders de objeto (WaterRTS,
// etc.) se dibujan ahora en dos pasadas separadas por objeto (ver Mesh3D::onUpdate) -- este objeto
// solo se encola aquí, se dibuja en FlushObjectShaderQueue() DESPUES de FlushOpaqueQueue() para
// que su escritura (con vértices ya desplazados por su propio vertex shader) sea la que quede, y
// no al revés.
void ComponentRender::EnqueueObjectShaders(Mesh3D *o)
{
    objectShaderQueue.push_back(o);
}

// Ver comentario de EnqueueObjectShaders. Nada que agrupar/instanciar aquí -- cada Mesh3D con
// custom shaders tiene su propia shaderChain (ping-pong FBOs por instancia), no comparte estado
// con las demás entradas de la cola.
void ComponentRender::FlushObjectShaderQueue()
{
    for (auto* o : objectShaderQueue) {
        o->RunObjectShaders();
    }
    objectShaderQueue.clear();
}

// Fase 4b: mismo criterio que EnqueueOpaque, pero para el pase de picking (ShaderOGLColor).
// MOUSE_CLICK_SELECT_OBJECT3D suele estar activo permanentemente (hover picking) -- antes esto
// dibujaba cada submesh al instante, uno por objeto y frame; ahora se encola y se vacía junto al
// resto en FlushPickingQueue().
void ComponentRender::EnqueuePicking(Mesh3D *o, bool useFeedbackBuffer, GLuint fbo)
{
    // Mismo mecanismo que EnqueueOpaque (Fase 1): useFeedbackBuffer==true solo lo pasa
    // Mesh3DAnimation::onUpdate(), señal de "unidad animada" para el picking instanciado con
    // skinning vía TBO (Fase 2) en vez de un feedbackBuffer por-instancia.
    auto* anim = useFeedbackBuffer ? dynamic_cast<Mesh3DAnimation*>(o) : nullptr;

    const auto& meshData = o->getMeshData();
    for (size_t meshIdx = 0; meshIdx < meshData.size(); meshIdx++) {
        const auto& m = meshData[meshIdx];
        if (!m.visibleInFrustum) continue;

        PickingQueueEntry entry;
        entry.o = o;
        entry.vertexBuffer = m.vertexBuffer;
        entry.uvBuffer = m.uvBuffer;
        entry.normalBuffer = m.normalBuffer;
        entry.size = static_cast<int>(m.vertices.size());
        // Unidades animadas: color de picking a nivel de OBJETO (getPickingColor()), no de
        // submesh -- mismo mecanismo que usaba el código antiguo (renderMesh con
        // getPickingColor()). submeshPickingColor (AnimationData::cloneInto) no está
        // garantizado no-cero para todas las instancias/submeshes animados -- un id=0 ahí se
        // interpreta como "nada" en el framebuffer de picking y la unidad queda inseleccionable
        // para siempre. Mesh3D estático sigue usando submeshPickingColor, que sí es fiable.
        entry.color = anim ? o->getPickingColor().toGLM() : m.submeshPickingColor.toGLM();
        entry.fbo = fbo;
        entry.indexBuffer = anim ? 0 : m.indexBuffer;
        entry.indexCount = anim ? 0 : m.indexCount;
        if (anim) {
            const auto& boneCache = anim->getBoneTransformCache(meshIdx);
            if (!boneCache.empty()) {
                entry.isSkinned = true;
                entry.vertexBoneDataBuffer = m.vertexBoneDataBuffer;
                entry.boneMatrices = &boneCache;
                entry.boneCount = static_cast<int>(boneCache.size());
            }
        }
        pickingQueue.push_back(entry);
    }
}

// Ordena por vertexBuffer -- mismo motivo que en el opaco: misma malla compartida (Fase 2.1) =
// mismo vertexBuffer. El color NO entra en la comparación de batch (a diferencia de texId/alpha
// en FlushOpaqueQueue): es el dato POR-INSTANCIA que sube junto a la matriz de modelo para poder
// distinguir qué objeto concreto fue pulsado -- nunca se pierde, cada instancia sigue llevando el
// suyo aunque se dibuje en el mismo draw instanciado que otras.
void ComponentRender::FlushPickingQueue()
{
    std::sort(pickingQueue.begin(), pickingQueue.end(), [](const PickingQueueEntry &a, const PickingQueueEntry &b) {
        return a.vertexBuffer < b.vertexBuffer;
    });

    auto sameBatch = [](const PickingQueueEntry &a, const PickingQueueEntry &b) {
        return a.vertexBuffer == b.vertexBuffer && a.uvBuffer == b.uvBuffer &&
               a.normalBuffer == b.normalBuffer && a.indexBuffer == b.indexBuffer &&
               a.indexCount == b.indexCount && a.fbo == b.fbo &&
               a.isSkinned == b.isSkinned && a.vertexBoneDataBuffer == b.vertexBoneDataBuffer &&
               a.boneCount == b.boneCount;
    };

    auto* colorShader = getShaders()->shaderOGLColor;
    size_t i = 0;
    while (i < pickingQueue.size()) {
        size_t j = i + 1;
        while (j < pickingQueue.size() && sameBatch(pickingQueue[i], pickingQueue[j])) ++j;

        const auto &first = pickingQueue[i];
        if (first.isSkinned) {
            // Igual que en FlushOpaqueQueue (Fase 1): las entradas skinned van siempre por el
            // camino instanciado, incluso un run de tamaño 1.
            std::vector<glm::mat4> models;
            std::vector<glm::vec3> colors;
            std::vector<glm::mat4> allBoneMatrices;
            models.reserve(j - i);
            colors.reserve(j - i);
            allBoneMatrices.reserve((j - i) * first.boneCount);
            for (size_t k = i; k < j; k++) {
                models.push_back(pickingQueue[k].o->getModelMatrix());
                colors.push_back(pickingQueue[k].color);
                const auto &bones = *pickingQueue[k].boneMatrices;
                allBoneMatrices.insert(allBoneMatrices.end(), bones.begin(), bones.end());
            }
            colorShader->RenderColorInstancedSkinned(
                first.vertexBuffer, first.uvBuffer, first.normalBuffer, first.vertexBoneDataBuffer,
                first.size, first.fbo, first.boneCount, models, colors, allBoneMatrices
            );
        } else if (j - i == 1) {
            colorShader->RenderColor(
                first.o->getModelMatrix(), first.vertexBuffer, first.uvBuffer, first.normalBuffer,
                first.size, Color(first.color.r, first.color.g, first.color.b, 1.0f), false, first.fbo,
                first.indexBuffer, first.indexCount
            );
        } else {
            std::vector<glm::mat4> models;
            std::vector<glm::vec3> colors;
            models.reserve(j - i);
            colors.reserve(j - i);
            for (size_t k = i; k < j; k++) {
                models.push_back(pickingQueue[k].o->getModelMatrix());
                colors.push_back(pickingQueue[k].color);
            }
            colorShader->RenderColorInstanced(
                first.vertexBuffer, first.uvBuffer, first.normalBuffer, first.size,
                first.fbo, first.indexBuffer, first.indexCount, models, colors
            );
        }
        i = j;
    }

    if (!pickingQueue.empty()) InvalidateRenderStateCache();
    pickingQueue.clear();
}

void ComponentRender::CreateCameraUBO()
{
    // projection + view, 2 mat4 (64 bytes c/u, alineado a 16 en std140) = 128 bytes, sin huecos.
    glGenBuffers(1, &cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, cameraUBO);
}

void ComponentRender::UpdateCameraUBO() const
{
    auto camera = Components::get()->Camera();
    glm::mat4 projection = camera->getGLMMat4ProjectionMatrix();
    glm::mat4 view = camera->getGLMMat4ViewMatrix();

    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &projection[0][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view[0][0]);
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

void ComponentRender::BuildDedupedStaticGeometry(
    const std::vector<glm::vec4> &vertices,
    const std::vector<glm::vec3> &normals,
    const std::vector<glm::vec2> &uvs,
    GLuint &outVertexBuffer,
    GLuint &outUvBuffer,
    GLuint &outNormalBuffer,
    GLuint &outIndexBuffer,
    GLsizei &outIndexCount
)
{
    // Fase 2.2: el stream de entrada esta "expandido" (Assimp se importa sin
    // aiProcess_JoinIdenticalVertices, ver Mesh3D::AssimpLoadGeometryFromFile), asi que suele
    // haber vertices con posicion+uv+normal IDENTICOS repetidos en varias caras. Los deduplicamos
    // aqui por comparacion exacta (mismos floats, no aproximada) y generamos un EBO que preserva
    // el mismo orden/cantidad de triangulos que antes tenia glDrawArrays.
    struct VertexKey {
        glm::vec4 position;
        glm::vec3 normal;
        glm::vec2 uv;
        bool operator==(const VertexKey &o) const {
            return position == o.position && normal == o.normal && uv == o.uv;
        }
    };
    struct VertexKeyHash {
        size_t operator()(const VertexKey &k) const {
            std::hash<float> h;
            size_t seed = 0;
            auto combine = [&](float f) { seed ^= h(f) + 0x9e3779b9u + (seed << 6) + (seed >> 2); };
            combine(k.position.x); combine(k.position.y); combine(k.position.z); combine(k.position.w);
            combine(k.normal.x); combine(k.normal.y); combine(k.normal.z);
            combine(k.uv.x); combine(k.uv.y);
            return seed;
        }
    };

    std::vector<glm::vec4> dedupVertices;
    std::vector<glm::vec3> dedupNormals;
    std::vector<glm::vec2> dedupUvs;
    std::vector<GLuint> indices;
    indices.reserve(vertices.size());
    dedupVertices.reserve(vertices.size());
    dedupNormals.reserve(vertices.size());
    dedupUvs.reserve(vertices.size());

    std::unordered_map<VertexKey, GLuint, VertexKeyHash> lookup;
    lookup.reserve(vertices.size());

    for (size_t i = 0; i < vertices.size(); i++) {
        VertexKey key{vertices[i], normals[i], uvs[i]};
        auto it = lookup.find(key);
        if (it != lookup.end()) {
            indices.push_back(it->second);
            continue;
        }
        auto newIndex = static_cast<GLuint>(dedupVertices.size());
        dedupVertices.push_back(vertices[i]);
        dedupNormals.push_back(normals[i]);
        dedupUvs.push_back(uvs[i]);
        lookup.emplace(key, newIndex);
        indices.push_back(newIndex);
    }

    glGenBuffers(1, &outVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, outVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(dedupVertices.size() * sizeof(glm::vec4)), dedupVertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &outUvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, outUvBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(dedupUvs.size() * sizeof(glm::vec2)), dedupUvs.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &outNormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, outNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(dedupNormals.size() * sizeof(glm::vec3)), dedupNormals.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &outIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    outIndexCount = static_cast<GLsizei>(indices.size());
}

void ComponentRender::FillOGLBuffers(std::vector<Mesh3DData> &meshes, bool withFeedbackBuffers)
{
    for (auto &m: meshes) {
        if (m.vertices.empty() || m.uvs.empty() || m.normals.empty()) {
            LOG_ERROR("[FillOGLBuffers] mesh with empty geometry (vertices=%zu uvs=%zu normals=%zu) — skipped",
                m.vertices.size(), m.uvs.size(), m.normals.size());
            continue;
        }

        if (!withFeedbackBuffers) {
            // Pipeline estatico (sin transform feedback): EBO deduplicado, Fase 2.2.
            BuildDedupedStaticGeometry(
                m.vertices, m.normals, m.uvs,
                m.vertexBuffer, m.uvBuffer, m.normalBuffer,
                m.indexBuffer, m.indexCount
            );
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

        // Solo se llega aqui con withFeedbackBuffers == true (el caso false hizo continue arriba).
        glGenBuffers(1, &m.feedbackBuffer);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m.feedbackBuffer);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, static_cast<GLuint>(m.vertices.size() * sizeof(glm::vec4)), m.vertices.data(), GL_DYNAMIC_COPY);

        glGenBuffers(1, &m.feedbackNormalBuffer);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m.feedbackNormalBuffer);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, static_cast<GLuint>(m.normals.size() * sizeof(glm::vec3)), m.normals.data(), GL_DYNAMIC_COPY);

        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
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
    Profiler::get()->StartGpuMeasure("ShadowPass");

    if (!Config::get()->ENABLE_SHADOW_MAPPING || !Config::get()->ENABLE_LIGHTS) {
        Profiler::get()->EndGpuMeasure("ShadowPass");
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
            if (anim->isEnableLights() && anim->getRenderSettings().shadowMap)
                casters.push_back(anim);
            continue;
        }
        if (auto* mesh = dynamic_cast<Mesh3D*>(obj)) {
            if (mesh->isEnableLights() && mesh->getRenderSettings().shadowMap)
                casters.push_back(mesh);
        }
    }

    if (!casters.empty()) {
        // AABB mundial por submesh, calculado una vez por caster/frame y reutilizado
        // para cullear contra el frustum de CADA luz (direccional y cada spot), en vez
        // del frustum de cámara: un caster fuera de cámara puede seguir proyectando
        // sombra sobre algo visible, así que la cámara no es el frustum correcto aquí.
        std::vector<std::vector<AABB3D>> casterSubmeshWorldAabbs;
        casterSubmeshWorldAabbs.reserve(casters.size());
        for (auto* mesh : casters) {
            glm::mat4 model = mesh->getModelMatrix();
            std::vector<AABB3D> submeshAabbs;
            submeshAabbs.reserve(mesh->getMeshData().size());
            for (const auto& m : mesh->getMeshData()) {
                AABB3D worldAabb;
                for (int i = 0; i < 8; i++) {
                    glm::vec4 wp = model * glm::vec4(m.localAabb.vertices[i].toGLM(), 1.0f);
                    worldAabb.vertices[i] = Vertex3D(wp.x / wp.w, wp.y / wp.w, wp.z / wp.w);
                }
                submeshAabbs.push_back(worldAabb);
            }
            casterSubmeshWorldAabbs.push_back(std::move(submeshAabbs));
        }

        shadowPass->renderSceneDirectionalLight(casters, casterSubmeshWorldAabbs, shaderRender->getDirectionalLight());

        const auto& spotLights = shaderRender->getShadowMappingSpotLights();
        for (int i = 0; i < static_cast<int>(spotLights.size()); i++) {
            shadowPass->renderSceneSpotLight(casters, casterSubmeshWorldAabbs, spotLights[i], i);
        }
    }

    Profiler::get()->EndGpuMeasure("ShadowPass");
    Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "ShadowPass");
}

void ComponentRender::LightPass() const
{
    Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), "LightPass");
    Profiler::get()->StartGpuMeasure("LightPass");

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
            globalBuffer.sceneFBO,
            gBuffer.emission,
            emissionUsedThisFrame
        );
    }

    Profiler::get()->EndGpuMeasure("LightPass");
    Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), "LightPass");
}

void ComponentRender::FlipBuffersToGlobal() const
{
    Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), "FlipBuffersToGlobal");
    Profiler::get()->StartGpuMeasure("FlipBuffersToGlobal");

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

    Profiler::get()->EndGpuMeasure("FlipBuffersToGlobal");
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
