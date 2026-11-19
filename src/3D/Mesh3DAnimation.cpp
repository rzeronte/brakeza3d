/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <atomic>
#include "../../include/Render/ProfilerZones.h"
#include <assimp/postprocess.h>
#include "../../include/3D/Mesh3DAnimation.h"
#include "../../include/Brakeza.h"
#include "../../include/Threads/ThreadJobBase.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Render/Drawable.h"
#include "../../include/GUI/Inspectors/Mesh3DAnimationGUI.h"
#include "../../include/Cache/AnimationDataCache.h"
#include "../../include/Cache/SceneCache.h"
#include <functional>
#include <assimp/config.h>
#include "../../include/Misc/AssetPaths.h"
#include "../../include/Render/Profiler.h"
#include <chrono>
#include <cstdio>
#if defined(__SSE2__)
#include <emmintrin.h>
#endif

// Producto A * B de matrices de Assimp (4x4, por filas) y volcado a glm (por columnas = la transpuesta). Con SSE2 cada fila del
// resultado es a1*B0 + a2*B1 + a3*B2 + a4*B3 sumado en ESE orden, igual que el producto de Assimp: el resultado es idéntico bit a bit
// (se comprobó contra el producto de Assimp sobre 270000 poses). Sin SSE2 se usa el producto de Assimp.
static inline void MulAi(const aiMatrix4x4 &A, const aiMatrix4x4 &B, aiMatrix4x4 &Out)
{
#if defined(__SSE2__)
    const float *a = &A.a1, *b = &B.a1;
    float *o = &Out.a1;
    const __m128 b0 = _mm_loadu_ps(b), b1 = _mm_loadu_ps(b + 4), b2 = _mm_loadu_ps(b + 8), b3 = _mm_loadu_ps(b + 12);
    for (int i = 0; i < 4; i++) {
        __m128 r = _mm_mul_ps(_mm_set1_ps(a[i * 4 + 0]), b0);
        r = _mm_add_ps(r, _mm_mul_ps(_mm_set1_ps(a[i * 4 + 1]), b1));
        r = _mm_add_ps(r, _mm_mul_ps(_mm_set1_ps(a[i * 4 + 2]), b2));
        r = _mm_add_ps(r, _mm_mul_ps(_mm_set1_ps(a[i * 4 + 3]), b3));
        _mm_storeu_ps(o + i * 4, r);
    }
#else
    Out = A * B;
#endif
}

static inline void AiToGlm(const aiMatrix4x4 &M, glm::mat4 &Out)
{
#if defined(__SSE2__)
    const float *m = &M.a1;
    __m128 r0 = _mm_loadu_ps(m), r1 = _mm_loadu_ps(m + 4), r2 = _mm_loadu_ps(m + 8), r3 = _mm_loadu_ps(m + 12);
    _MM_TRANSPOSE4_PS(r0, r1, r2, r3);
    float *o = &Out[0][0];
    _mm_storeu_ps(o, r0); _mm_storeu_ps(o + 4, r1); _mm_storeu_ps(o + 8, r2); _mm_storeu_ps(o + 12, r3);
#else
    Out = Tools::aiMat4toGLMMat4(M);
#endif
}

// Cronometros de la animacion (solo con el detalle de scripts del profiler activo, p.ej. RenderPerfProbe):
// suman por categoria y vuelcan la media por consola cada ~2 s. Sin el flag cuestan una comprobacion.
namespace animPerf {
    using Clock = std::chrono::steady_clock;
    struct Acc {
        double hierarchyMs = 0, finalMs = 0, collidersMs = 0, bakeMs = 0, totalMs = 0;
        long calls = 0;
        Clock::time_point last = Clock::now();
    };
    inline Acc acc;
    inline bool on() { return Profiler::get()->isScriptDetailEnabled(); }
    inline Clock::time_point now(bool enabled) { return enabled ? Clock::now() : Clock::time_point(); }
    inline double since(bool enabled, Clock::time_point t0) {
        return enabled ? std::chrono::duration<double, std::milli>(Clock::now() - t0).count() : 0.0;
    }
    inline void flush(bool enabled) {
        if (!enabled) return;
        const double secs = std::chrono::duration<double>(Clock::now() - acc.last).count();
        if (secs < 2.0 || acc.calls == 0) return;
        std::printf("[AnimPerf] %.0f unidades/s | por unidad (us): total=%.1f jerarquia=%.1f finales=%.1f colliders=%.1f bake=%.1f | ms por segundo: total=%.1f jerarquia=%.1f finales=%.1f bake=%.1f\n",
            acc.calls / secs,
            acc.totalMs * 1000.0 / acc.calls, acc.hierarchyMs * 1000.0 / acc.calls, acc.finalMs * 1000.0 / acc.calls,
            acc.collidersMs * 1000.0 / acc.calls, acc.bakeMs * 1000.0 / acc.calls,
            acc.totalMs / secs, acc.hierarchyMs / secs, acc.finalMs / secs, acc.bakeMs / secs);
        std::fflush(stdout);
        acc = Acc();
    }
}

std::vector<Mesh3DAnimation::SharedAnimationLibrary> Mesh3DAnimation::sharedAnimationsFiles;   // vacio = desactivado; lo fija el proyecto ("sharedAnimationsFiles" en su JSON, ver ProjectLoader)

namespace {
constexpr unsigned int kModelImportFlags =
    aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenSmoothNormals | aiProcess_FlipUVs;

bool LibraryExists(const std::string& path)
{
    return !path.empty() && Tools::FileExists(AssetPaths::Resolve(path).c_str());
}

bool SharedAnimationsAvailable()
{
    for (const auto& lib : Mesh3DAnimation::sharedAnimationsFiles) {
        if (LibraryExists(lib.file)) return true;
    }
    return false;
}

// Importa el modelo. Si NO trae animaciones y existe la biblioteca compartida, se reimporta sin
// conservar los pivotes del FBX: asi cada hueso es UN nodo y sus canales se enlazan por nombre
// con los de la biblioteca (que se importa igual). Con pivotes, Assimp parte cada transformacion
// en nodos auxiliares (X_$AssimpFbx$_Rotation...) que un modelo sin animar puede no tener.
// Los modelos con animaciones propias se importan como siempre.
std::shared_ptr<Assimp::Importer> ImportModelScene(const std::string& filename)
{
    const std::string resolved = AssetPaths::Resolve(filename);   // proyecto primero, core de respaldo (la clave de cache sigue siendo filename)

    auto imp = std::make_shared<Assimp::Importer>();
    const aiScene* s = imp->ReadFile(resolved, kModelImportFlags);

    if (s && s->mNumAnimations == 0 && SharedAnimationsAvailable()) {
        auto imp2 = std::make_shared<Assimp::Importer>();
        imp2->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
        if (imp2->ReadFile(resolved, kModelImportFlags)) {
            return imp2;
        }
    }
    return imp;
}

// Las bibliotecas se mantienen cargadas mientras dure la sesion (SceneCache solo guarda referencias debiles): asi no se
// vuelven a parsear aunque se destruyan todas las instancias, y la precarga no se pierde antes de usarse.
std::mutex g_libKeepAliveMutex;
std::unordered_map<std::string, std::shared_ptr<Assimp::Importer>> g_libKeepAlive;

std::shared_ptr<Assimp::Importer> LoadSharedAnimationLibrary(const std::string& path)
{
    // Un solo parseo aunque muchas unidades la pidan a la vez (mismo patron que los modelos).
    auto keyMutex = animationDataCache.getKeyLoadMutex(path);
    std::lock_guard<std::mutex> lock(*keyMutex);

    auto imp = sceneCache.get(path);
    if (imp && imp->GetScene()) return imp;

    imp = std::make_shared<Assimp::Importer>();
    imp->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    if (!imp->ReadFile(AssetPaths::Resolve(path), 0)) {
        LOG_ERROR("[Mesh3DAnimation] No se pudo cargar la biblioteca de animaciones '%s': %s", path.c_str(), imp->GetErrorString());
        return nullptr;
    }
    sceneCache.store(path, imp);
    {
        std::lock_guard<std::mutex> keep(g_libKeepAliveMutex);
        g_libKeepAlive[path] = imp;
    }
    LOG_MESSAGE("[Mesh3DAnimation] Biblioteca de animaciones '%s' cargada (%u animaciones)", path.c_str(), imp->GetScene()->mNumAnimations);
    return imp;
}

}

Mesh3DAnimation::Mesh3DAnimation()
{
}

void Mesh3DAnimation::PreloadSharedAnimations()
{
    for (const auto& lib : sharedAnimationsFiles) {
        if (!LibraryExists(lib.file)) continue;
        const std::string path = lib.file;
        // Un trabajo por biblioteca: se parsean en paralelo entre si y con el resto de la carga.
        Brakeza::get()->PoolCompute().enqueue(std::make_shared<ThreadJobBase>(
            [path]() { LoadSharedAnimationLibrary(path); },
            []() {}
        ));
    }
}

void Mesh3DAnimation::AttachSharedAnimations()
{
    animScene = nullptr;
    animImporter.reset();
    animCopyLocal = false;
    retargetBones.clear();

    if (!scene || scene->mNumAnimations > 0) return;

    // Primera biblioteca cuyo esqueleto coincide con el del modelo (cada familia de esqueleto
    // tiene la suya). Cada una se parsea una sola vez (sceneCache) aunque se pruebe en cada modelo.
    for (const auto& libCfg : Mesh3DAnimation::sharedAnimationsFiles) {
        const std::string& libPath = libCfg.file;
        if (!LibraryExists(libPath)) continue;

        auto lib = LoadSharedAnimationLibrary(libPath);
        if (!lib || !lib->GetScene() || lib->GetScene()->mNumAnimations == 0) continue;

        retargetBones.clear();
        animImporter  = lib;
        animScene     = lib->GetScene();
        animCopyLocal = libCfg.copyLocalRotations;

        // Nodos de la biblioteca por nombre, con su rotacion de reposo en el mundo (una pasada).
        struct LibNode { const aiNode* node; aiMatrix3x3 world; aiMatrix3x3 parentWorld; };
        std::unordered_map<std::string, LibNode> libNodes;
        std::function<void(const aiNode*, const aiMatrix3x3&)> indexLib = [&](const aiNode* n, const aiMatrix3x3& Wp) {
            aiVector3D sc, ps; aiQuaternion q;
            n->mTransformation.Decompose(sc, q, ps);
            const aiMatrix3x3 W = Wp * q.GetMatrix();
            libNodes[n->mName.C_Str()] = LibNode{n, W, Wp};
            for (unsigned int i = 0; i < n->mNumChildren; i++) indexLib(n->mChildren[i], W);
        };
        indexLib(animScene->mRootNode, aiMatrix3x3());

        // Reposo propio de cada hueso (local y en el mundo) y su equivalente en la biblioteca.
        int totalBones = 0, matchedBones = 0;
        std::function<void(const aiNode*, const aiMatrix3x3&, int)> walk = [&](const aiNode* node, const aiMatrix3x3& Wp, int parentBone) {
            const std::string name = node->mName.C_Str();

            RetargetBone rb;
            aiQuaternion rot;
            node->mTransformation.Decompose(rb.scale, rot, rb.pos);
            const aiMatrix3x3 R = rot.GetMatrix();
            const aiMatrix3x3 W = Wp * R;

            int childParentBone = parentBone;
            if (boneMapping.find(name) != boneMapping.end()) {
                rb.idx              = totalBones++;
                rb.parentIdx        = parentBone;
                rb.isRoot           = parentBone < 0;
                rb.modelRestLocal   = R;
                rb.modelRestWorld   = W;
                rb.modelParentWorld = Wp;

                auto lib = libNodes.find(name);
                if (lib != libNodes.end()) {
                    matchedBones++;
                    rb.inLib = true;
                    aiVector3D lp, ls; aiQuaternion lr;
                    lib->second.node->mTransformation.Decompose(ls, lr, lp);
                    rb.libRestPos     = lp;
                    rb.libParentWorld = lib->second.parentWorld;
                    rb.libRestWorldInv = lib->second.world;
                    rb.libRestWorldInv.Transpose();           // rotacion ortonormal: inversa = traspuesta
                    if (rb.isRoot) {
                        const float lenLib = lp.Length();
                        if (lenLib > 1e-6f) rb.rootScale = rb.pos.Length() / lenLib;
                    }
                }
                retargetBones[node] = rb;
                childParentBone = rb.idx;
            }
            for (unsigned int i = 0; i < node->mNumChildren; i++) walk(node->mChildren[i], W, childParentBone);
        };
        walk(scene->mRootNode, aiMatrix3x3(), -1);
        retargetWl.assign(totalBones, aiMatrix3x3());
        retargetWt.assign(totalBones, aiMatrix3x3());

        // Solo si el esqueleto del modelo es (en su mayoria) el de la biblioteca: un modelo sin
        // animaciones que no sea un personaje Mixamo no debe heredar clips que no le sirven.
        if (totalBones == 0 || matchedBones * 2 < totalBones) {
            LOG_MESSAGE("[Mesh3DAnimation] '%s': esqueleto distinto al de la biblioteca (%d de %d huesos coinciden), no se usa",
                getName().c_str(), matchedBones, totalBones);
            animScene = nullptr;
            animImporter.reset();
            animCopyLocal = false;
            retargetBones.clear();
            retargetWl.clear();
            retargetWt.clear();
            continue;       // probar la siguiente biblioteca
        }

        LOG_MESSAGE("[Mesh3DAnimation] '%s' usa la biblioteca compartida '%s' (%zu huesos, %u animaciones)",
            getName().c_str(), libPath.c_str(), retargetBones.size(), animScene->mNumAnimations);
        return;
    }
}

void Mesh3DAnimation::onUpdate()
{
    Object3D::onUpdate();

    if (!isEnabled() || isRemoved()) {
        LOG_MESSAGE("Scene is disable or removed!");
        return;
    }

    if (scene == nullptr) {
        LOG_MESSAGE("Scene is null!");
        return;
    }

    BZ_ZONE_N("Anim: onUpdate");
    const bool ap = animPerf::on();
    const auto apT0 = animPerf::now(ap);
    UpdateFrameTransformations();
    animPerf::acc.totalMs += animPerf::since(ap, apT0);

    // La pose horneada (transform feedback, una llamada por submesh) solo si este frame la va a
    // leer alguien -- el G-Buffer, el picking y las sombras ya hacen el skinning en su shader con
    // las matrices de huesos (TBO). Ver NeedsBakedPose.
    {
        const auto t = animPerf::now(ap);
        UpdateOpenGLBones(NeedsBakedPose());
        animPerf::acc.bakeMs += animPerf::since(ap, t);
    }

    if (boneColliderEnabled) {
        const auto t = animPerf::now(ap);
        UpdateBoneColliders();
        animPerf::acc.collidersMs += animPerf::since(ap, t);
    }
    if (ap) { animPerf::acc.calls++; animPerf::flush(ap); }

    auto render = Components::get()->Render();
    auto window = Components::get()->Window();

    // Ver Mesh3D::onUpdate() -- mismo motivo: Outline toca GL_BLEND/DEPTH_TEST/CULL_FACE por su
    // cuenta, sin pasar por la caché compartida de Fase 1.1.1 (Deferred/Forward, más abajo).
    if (isGUISelected() && !Components::get()->Scripting()->isExecuting()) {
        render->getShaders()->shaderOGLOutline->drawOutline(this, Color::green(), 0.1f, window->getUIFramebuffer());
        render->InvalidateRenderStateCache();
    }

    if (render->getLastRightClickedObject() == this && !isGUISelected()) {
        render->getShaders()->shaderOGLOutline->drawOutline(this, Color(1.0f, 0.5f, 0.0f, 1.0f), 0.1f, window->getForegroundFramebuffer());
        render->InvalidateRenderStateCache();
    }

    GLuint fbo =  Config::get()->ENABLE_LIGHTS ? window->getGBuffer().FBO : window->getSceneFramebuffer();

    if (Config::get()->TRIANGLE_MODE_TEXTURIZED) {
        if (!isTransparent()) {
            if (Config::get()->ENABLE_LIGHTS && isEnableLights()) {
                // Fase 3 (Etapa 2): encolar en vez de dibujar al instante -- ver
                // ComponentRender::EnqueueOpaque/FlushOpaqueQueue, mismo mecanismo que Mesh3D.
                render->EnqueueOpaque(this, true, fbo);
            } else {
                render->getShaders()->shaderOGLRender->renderMesh(this, true, fbo);
            }
        }
    }

    if (Config::get()->TRIANGLE_MODE_PIXELS) {
        render->getShaders()->shaderOGLPoints->renderMeshAnimation(this, fbo);
    }

    if (Config::get()->TRIANGLE_MODE_SHADING) {
        render->getShaders()->shaderOGLShading->renderMesh(this, true, fbo);
        render->InvalidateRenderStateCache();
    }

    if (Config::get()->TRIANGLE_MODE_WIREFRAME) {
        render->getShaders()->shaderOGLWireframe->renderMesh(this, true, Color::gray(), fbo);
        render->InvalidateRenderStateCache();
    }

    if (Config::get()->DRAW_MESH3D_AABB) {
        UpdateBoundingBox();
        Drawable::drawAABB(&aabb, Color::white());
    }

    if (Config::get()->DRAW_MESH3D_OCTREE && octree != nullptr) {
        Drawable::drawOctree(octree);
    }

    if (Config::get()->DRAW_MESH3D_GRID && grid != nullptr) {
        Drawable::drawGrid3D(grid);
    }

    for (auto s: customShaders) {
        s->Render(window->getSceneFramebuffer(), window->getGlobalTexture());
    }

    if (scene && Config::get()->DRAW_ANIMATION_BONES) {
        DrawBones(scene->mRootNode, nullptr);
    }

    if (Config::get()->MOUSE_CLICK_SELECT_OBJECT3D) {
        // Instancing con skinning: encolado igual que Mesh3D::onUpdate()
        // -- se dibuja junto en ComponentRender::FlushPickingQueue(), que ya invalida la caché de
        // estado una sola vez al final si la cola no estaba vacía (no hace falta aquí).
        render->EnqueuePicking(this, true, window->getPickingColorFramebuffer().FBO);
    }
}

void Mesh3DAnimation::postUpdate()
{
    Object3D::postUpdate();

    auto render = Components::get()->Render();
    auto window = Components::get()->Window();

    if (Config::get()->TRIANGLE_MODE_TEXTURIZED) {
        if (isTransparent()) {
            render->getShaders()->shaderOGLRender->renderMesh(this, true, window->getSceneFramebuffer());
        }
    }
}

// Consumidores que todavía leen feedbackBuffer (la pose horneada): render no instanciado (sin
// luces), transparentes (postUpdate), modos de depuración (puntos/shading/wireframe), shaders de
// objeto propios y el outline (selección en el editor, clic derecho). Si alguno cambia a skinning
// por TBO, sale de aquí. Si se añade un consumidor nuevo de feedbackBuffer, tiene que entrar aquí:
// sin él, leería la pose de la última vez que se horneó.
bool Mesh3DAnimation::NeedsBakedPose() const
{
    const auto* cfg = Config::get();
    if (cfg->TRIANGLE_MODE_PIXELS || cfg->TRIANGLE_MODE_SHADING || cfg->TRIANGLE_MODE_WIREFRAME) return true;
    if (cfg->TRIANGLE_MODE_TEXTURIZED && (isTransparent() || !(cfg->ENABLE_LIGHTS && isEnableLights()))) return true;
    if (!customShaders.empty()) return true;
    if (isGUISelected()) return true;
    if (Components::get()->Render()->getLastRightClickedObject() == this) return true;
    return false;
}

void Mesh3DAnimation::UpdateOpenGLBones(bool bake)
{
    if (!scene) return;
    BZ_ZONE_N("Anim: UpdateOpenGLBones (glm cache + bake)");

    boneTransformCachePerMesh.resize(meshes.size());

    auto shaderBones = Components::get()->Render()->getShaders()->shaderOGLBonesTransforms;
    auto fbo = Components::get()->Window()->getSceneFramebuffer();

    for (size_t meshIdx = 0; meshIdx < meshes.size(); meshIdx++) {
        auto &m = meshes[meshIdx];
        if (m.vertices.empty()) continue;
        if (meshIdx >= meshBoneFinalTransforms.size()) continue;

        const auto &meshTransforms = meshBoneFinalTransforms[meshIdx];
        auto &glmCache = boneTransformCachePerMesh[meshIdx];
        if (glmCache.size() < meshTransforms.size()) {
            glmCache.resize(meshTransforms.size(), glm::mat4(1.0f));
        }
        for (size_t b = 0; b < meshTransforms.size(); b++) {
            AiToGlm(meshTransforms[b], glmCache[b]);
        }

        if (bake) shaderBones->render(m, glmCache, fbo);
    }
}

void Mesh3DAnimation::onUpdateCulled()
{
    Object3D::onUpdateCulled();

    if (!isEnabled() || isRemoved()) return;
    BZ_ZONE_N("Anim: onUpdateCulled");

    // Fuera de cámara no se calculan huesos ni se dibuja, pero el reloj de la animación tiene que
    // seguir corriendo: si se congela, una unidad que muere fuera de pantalla deja su animación de
    // muerte en el frame 0 y la reproduce entera al volver a entrar en cámara (varias "muriendo de
    // golpe"), y los scripts que esperan isAnimationEnds() se quedan atascados.
    AdvanceAnimationClock();
}

// Avanza runningTime/finished y aplica removeAtEndAnimation. Devuelve false si el objeto se ha
// marcado para eliminar (no hay que seguir actualizándolo este frame).
bool Mesh3DAnimation::AdvanceAnimationClock()
{
    const aiScene* as = animationsScene();
    if (!scene || !as || !as->HasAnimations()) return false;

    CheckIfEndAnimation();

    if (isRemoveAtEndAnimation() && isAnimationEnds()) {
        getParent()->setRemoved(true);
        return false;
    }

    return true;
}

void Mesh3DAnimation::UpdateFrameTransformations()
{
    if (!AdvanceAnimationClock()) return;

    UpdateBonesFinalTransformations(runningTime);
}

void Mesh3DAnimation::CheckIfEndAnimation()
{
    auto timeIncrement = Brakeza::get()->getDeltaTime() * animation_speed;
    finished = false;
    auto maxAnimationTime = getCurrentAnimationMaxTime();

    if (runningTime + timeIncrement >= maxAnimationTime) {
        finished = true;
        if (loop) {
            runningTime = 0.000;
        } else {
            runningTime = maxAnimationTime - 0.01f;
        }

        return;
    }

    runningTime += timeIncrement;
}

void Mesh3DAnimation::restartAnimation()
{
    nodeAnimCache.clear();
    runningTime = 0;
    finished = false;
}

float Mesh3DAnimation::getAnimationDuration() const
{
    const float speed = animation_speed > 0.0001f ? animation_speed : 1.0f;
    return getCurrentAnimationMaxTime() / speed;
}

float Mesh3DAnimation::getCurrentAnimationMaxTime() const
{
    const aiScene* as = animationsScene();
    if (!as || as->mNumAnimations == 0 || indexCurrentAnimation >= static_cast<int>(as->mNumAnimations)) return 1.0f;
    return static_cast<float>(
        as->mAnimations[indexCurrentAnimation]->mDuration / as->mAnimations[indexCurrentAnimation]->mTicksPerSecond
    );
}

bool Mesh3DAnimation::AssimpLoadAnimation(const std::string &filename)
{
    LOG_MESSAGE("[AssimpLoadAnimation] Load animation from %s", filename.c_str());

    if (!Tools::FileExists(filename.c_str())) {
        LOG_MESSAGE("[AssimpLoadAnimation] ERROR: File not found %s", filename.c_str());
        return false;
    }

    boneColliderIndex = 0;
    boneColliderEnabled = true;
    setSourceFile(filename);

    // get()-miss -> parse -> store() no es atómico: sin este lock, N unidades que piden el mismo
    // modelo a la vez (roster inicial, 125 civiles...) ven todas MISS y parsean el FBX (~32 MB,
    // 4-6 s) N veces en paralelo -- medido hasta x5 por fichero en la carga.
    // Lock POR FICHERO: los demás esperan al primero y
    // entran por la rama HIT; ficheros distintos siguen cargándose en paralelo.
    auto keyLoadMutex = animationDataCache.getKeyLoadMutex(filename);
    std::unique_lock<std::mutex> cacheLoadLock(*keyLoadMutex);

    auto cached = animationDataCache.get(filename);
    if (cached) {
        LOG_MESSAGE("[AssimpLoadAnimation] Cache HIT for '%s'", filename.c_str());

        sharedImporter = sceneCache.get(filename);
        if (!sharedImporter) {
            // SceneCache expirado (todas las instancias previas destruidas): recargar
            sharedImporter = ImportModelScene(filename);
            sceneCache.store(filename, sharedImporter);
        }
        // Escena ya disponible (cacheada o recargada bajo el lock): el trabajo por instancia de
        // abajo no necesita serializarse entre instancias del mismo modelo.
        cacheLoadLock.unlock();
        scene = sharedImporter->GetScene();
        if (!scene) {
            LOG_MESSAGE("[AssimpLoadAnimation] ERROR getting scene from SceneCache for '%s'", filename.c_str());
            return false;
        }

        globalInverseTransform = scene->mRootNode->mTransformation;
        globalInverseTransform.Inverse();

        meshes.resize(scene->mNumMeshes);
        meshVertices.resize(scene->mNumMeshes);
        meshVerticesBoneData.resize(scene->mNumMeshes);

        cached->cloneInto(*this);
        sharedAnimModel = cached;
        sharedStaticGeometry = true;

        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[i];
            if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) continue;

            meshVertices[i].assign(mesh->mNumVertices, Vertex3D());

            std::vector<Vertex3D> localMeshNormals(mesh->mNumVertices);
            LoadMeshVertex(i, mesh, meshVertices[i], localMeshNormals);

            // Bookkeeping por instancia (boneMapping/boneInfo/numBones) -- sigue haciendo falta
            // siempre, BoneInfo::WorldTransform se anima de forma independiente por instancia.
            // El peso por vértice YA expandido por cara se reutiliza de la caché en vez de
            // volver a recorrer Assimp (ver AnimationMeshEntry::boneData).
            std::vector<VertexBoneData> localMeshBones(mesh->mNumVertices);
            LoadMeshBones(i, mesh, localMeshBones, /*skipWeights=*/true);

            if (i < cached->meshes.size()) {
                meshVerticesBoneData[i] = cached->meshes[i].boneData;
            } else {
                meshVerticesBoneData[i].clear();
            }
        }

        AttachSharedAnimations();

        loaded = true;
        return true;
    }

    LOG_MESSAGE("[AssimpLoadAnimation] Cache MISS, loading '%s'...", filename.c_str());

    sharedImporter = ImportModelScene(filename);
    scene = sharedImporter->GetScene();

    if (!scene) {
        LOG_MESSAGE("[AssimpLoadAnimation] ERROR loading '%s': %s", filename.c_str(), sharedImporter->GetErrorString());
        return false;
    }

    sceneCache.store(filename, sharedImporter);

    std::vector<MaterialEntryData> materialEntries;
    AssimpInitMaterials(scene, &materialEntries);

    ReadNodesFromRoot();

    auto animData = std::make_shared<AnimationData>();
    animData->sourceFile = filename;
    animData->materials = std::move(materialEntries);

    for (size_t i = 0; i < meshes.size(); i++) {
        AnimationMeshEntry entry;
        const auto& srcM = meshes[i];

        entry.vertices = srcM.vertices;
        entry.uvs = srcM.uvs;
        entry.normals = srcM.normals;
        entry.materialIndex = srcM.materialIndex;
        entry.name = srcM.name;
        // Copia (no move): esta instancia también sigue usando su propio meshVerticesBoneData[i].
        entry.boneData = meshVerticesBoneData[i];

        for (Triangle* tri : srcM.modelTriangles) {
            entry.triangleVertices.push_back(tri->A);
            entry.triangleVertices.push_back(tri->B);
            entry.triangleVertices.push_back(tri->C);
        }

        animData->meshes.push_back(std::move(entry));
    }

    animationDataCache.store(filename, animData);
    sharedAnimModel = animData;
    sharedStaticGeometry = true;

    LOG_MESSAGE("[AssimpLoadAnimation] Stored in AnimationDataCache (%zu meshes, %zu materials)",
        animData->meshes.size(), animData->materials.size());

    AttachSharedAnimations();

    loaded = true;
    return true;
}

void Mesh3DAnimation::ReadNodesFromRoot()
{
    // Transformación raíz
    globalInverseTransform = scene->mRootNode->mTransformation;
    globalInverseTransform.Inverse();
    meshes.resize(scene->mNumMeshes);

    // Para cada malla preparamos sus array de "Vertices" y "VertexBoneData"
    meshVertices.resize(scene->mNumMeshes);
    meshVerticesBoneData.resize(scene->mNumMeshes);

    // Procesamos los nodos desde el nodo raíz
    ProcessNodeAnimation(scene->mRootNode);
}

void Mesh3DAnimation::ProcessNodeAnimation(aiNode *node)
{
    // Procesamos las mallas del nodo dado
    for (unsigned int x = 0; x < node->mNumMeshes; x++) {
        int idMesh = static_cast<int>(node->mMeshes[x]);
        this->ProcessMeshAnimation(idMesh, scene->mMeshes[idMesh]);
    }

    // Continuamos recursivamente por los hijos del nodo dado
    for (unsigned int j = 0; j < node->mNumChildren; j++) {
        ProcessNodeAnimation(node->mChildren[j]);
    }
}

void Mesh3DAnimation::ProcessMeshAnimation(int i, aiMesh *mesh)
{
    if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
        LOG_MESSAGE("Skip mesh non triangle");
        return;
    }

    LOG_MESSAGE("[ProcessMeshAnimation] Loading mesh animation with materialIndex: %d", mesh->mMaterialIndex);
    meshes[i].materialIndex = mesh->mMaterialIndex;

    std::vector<VertexBoneData> localMeshBones(mesh->mNumVertices);
    std::vector<Vertex3D> localMeshVertices(mesh->mNumVertices);
    std::vector<Vertex3D> localMeshNormals(mesh->mNumVertices);

    LoadMeshBones(i, mesh, localMeshBones);
    LoadMeshVertex(i, mesh, localMeshVertices, localMeshNormals);

    meshVertices[i] = localMeshVertices;

    std::vector<VertexBoneData> expandedBoneData;
    expandedBoneData.reserve(mesh->mNumFaces * 3);

    for (unsigned int k = 0; k < mesh->mNumFaces; k++) {
        const aiFace &Face = mesh->mFaces[k];

        if (Face.mNumIndices < 3) continue;

        auto index1 = Face.mIndices[0];
        auto index2 = Face.mIndices[1];
        auto index3 = Face.mIndices[2];

        Vertex3D V1 = meshVertices[i][index1];
        Vertex3D V2 = meshVertices[i][index2];
        Vertex3D V3 = meshVertices[i][index3];

        meshes[i].modelTriangles.push_back(new Triangle(V1, V2, V3, this));

        Vertex3D normalV1 = localMeshNormals[index1];
        Vertex3D normalV2 = localMeshNormals[index2];
        Vertex3D normalV3 = localMeshNormals[index3];

        meshes[i].vertices.emplace_back(V1.x, V1.y, V1.z, 1.0f);
        meshes[i].vertices.emplace_back(V2.x, V2.y, V2.z, 1.0f);
        meshes[i].vertices.emplace_back(V3.x, V3.y, V3.z, 1.0f);

        meshes[i].uvs.emplace_back(V1.u, V1.v);
        meshes[i].uvs.emplace_back(V2.u, V2.v);
        meshes[i].uvs.emplace_back(V3.u, V3.v);

        meshes[i].normals.emplace_back(normalV1.x, normalV1.y, normalV1.z);
        meshes[i].normals.emplace_back(normalV2.x, normalV2.y, normalV2.z);
        meshes[i].normals.emplace_back(normalV3.x, normalV3.y, normalV3.z);

        expandedBoneData.push_back(localMeshBones[index1]);
        expandedBoneData.push_back(localMeshBones[index2]);
        expandedBoneData.push_back(localMeshBones[index3]);
    }

    meshVerticesBoneData[i] = std::move(expandedBoneData);
}

void Mesh3DAnimation::LoadMeshBones(int meshId, aiMesh *mesh, std::vector<VertexBoneData> &meshVertexBoneData, bool skipWeights)
{
    for (int i = 0; i < static_cast<int>(mesh->mNumBones); i++) {
        int BoneIndex;
        std::string BoneName(mesh->mBones[i]->mName.data);

        if (boneMapping.find(BoneName) == boneMapping.end()) {
            BoneIndex = numBones;
            numBones++;
            BoneInfo bi = BoneInfo();
            boneInfo.push_back(bi);

            boneMapping[BoneName] = BoneIndex;

            boneInfo[BoneIndex].name = mesh->mBones[i]->mName.C_Str();
            //LOG_MESSAGE("[Mesh3DAnimation] Loading BoneInfo %s", boneInfo[BoneIndex].name.c_str());
        } else {
            BoneIndex = static_cast<int>(boneMapping[BoneName]);
        }

        // El offset (bind pose) se guarda SIEMPRE para este mesh en concreto, aunque el
        // hueso ya estuviera registrado por otro mesh anterior -- cada mesh tiene su propia
        // relacion con el esqueleto en el momento del bind (ver comentario en BoneInfo).
        boneInfo[BoneIndex].BoneOffsetByMesh[meshId] = mesh->mBones[i]->mOffsetMatrix;

        // skipWeights=true: cache-hit de AnimationData, el peso por vértice ya calculado se
        // reutiliza de AnimationMeshEntry::boneData (ver AssimpLoadAnimation) -- solo hace
        // falta el bookkeeping de arriba (boneMapping/boneInfo/numBones), que sí es por
        // instancia.
        if (skipWeights) continue;

        for (int j = 0; j < static_cast<int>(mesh->mBones[i]->mNumWeights); j++) {
            unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = mesh->mBones[i]->mWeights[j].mWeight;

            meshVertexBoneData[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
}

void Mesh3DAnimation::LoadMeshVertex(int meshId, aiMesh *mesh, std::vector<Vertex3D> &meshVertex,  std::vector<Vertex3D> &meshNormal)
{
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
        aiVector3t vf = mesh->mVertices[j];
        Vertex3D v(vf.x, vf.y, vf.z);

        const aiVector3D *pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][j]) : &Zero3D;
        v.u = pTexCoord->x;
        v.v = pTexCoord->y;

        meshVertex[j] = v;
        aiVector3t n = mesh->mNormals[j];
        meshNormal[j] = Vertex3D::fromAssimp(n);
    }
}

void Mesh3DAnimation::UpdateBonesFinalTransformations(float TimeInSeconds)
{
    const aiScene* as = animationsScene();
    if (!scene || !as || !as->HasAnimations() || indexCurrentAnimation >= static_cast<int>(as->mNumAnimations)) return;

    BZ_ZONE_N("Anim: UpdateBonesFinalTransformations");
    auto TicksPerSecond = static_cast<float>(
        as->mAnimations[indexCurrentAnimation]->mTicksPerSecond != 0 ? as->mAnimations[indexCurrentAnimation]->mTicksPerSecond : 0.25f
    );
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    auto AnimationTime = static_cast<float>(fmod(TimeInTicks, as->mAnimations[indexCurrentAnimation]->mDuration));

    const bool ap = animPerf::on();
    const auto t0 = animPerf::now(ap);
    {
        BZ_ZONE_N("Anim: ReadNodeHierarchy");
        ReadNodeHierarchyCached(AnimationTime);
    }
    const auto t1 = animPerf::now(ap);
    {
        BZ_ZONE_N("Anim: ComputeMeshBoneFinalTransforms");
        ComputeMeshBoneFinalTransforms();
    }
    if (ap) {
        animPerf::acc.hierarchyMs += std::chrono::duration<double, std::milli>(t1 - t0).count();
        animPerf::acc.finalMs     += animPerf::since(ap, t1);
    }
}

// Transformación local de un nodo en el instante AnimationTime: la del propio nodo, o la animada (propia o retargeteada
// desde la biblioteca compartida). pNodeAnim = canal del nodo en la animación actual (nullptr = sin canal) y rbPtr = su
// reposo retargeteado (nullptr = no es hueso de una animación compartida). Lo comparten el recorrido recursivo y el
// recorrido con el esqueleto precalculado, para que ambos den exactamente lo mismo.
aiMatrix4x4 Mesh3DAnimation::NodeLocalTransform(float AnimationTime, const aiNode *pNode, const aiNodeAnim *pNodeAnim, const RetargetBone *rbPtr)
{
    aiMatrix4x4 NodeTransformation;
    NodeTransformation = pNode->mTransformation;

    if (animScene) {
        // Animaciones compartidas (ver RetargetBone): movimiento de la biblioteca sobre el reposo
        // del modelo. Los nodos que no son huesos del modelo conservan su transformacion.
        if (rbPtr != nullptr) {
            const RetargetBone& rb = *rbPtr;

            const aiMatrix3x3 WlParent = rb.isRoot ? rb.libParentWorld   : retargetWl[rb.parentIdx];
            const aiMatrix3x3 WtParent = rb.isRoot ? rb.modelParentWorld : retargetWt[rb.parentIdx];

            if (pNodeAnim != nullptr && rb.inLib && pNodeAnim->mNumRotationKeys > 0) {
                aiQuaternion RotationQ;
                CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);

                aiMatrix3x3 Local;
                if (animCopyLocal) {
                    // Biblioteca de animaciones copiadas entre armatures: rotacion local tal cual.
                    Local = RotationQ.GetMatrix();
                } else {
                    // Giro en el mundo respecto al reposo de la biblioteca, aplicado al reposo del modelo.
                    const aiMatrix3x3 Wl = WlParent * RotationQ.GetMatrix();
                    const aiMatrix3x3 Wt = (Wl * rb.libRestWorldInv) * rb.modelRestWorld;
                    retargetWl[rb.idx] = Wl;
                    retargetWt[rb.idx] = Wt;

                    aiMatrix3x3 WtParentInv = WtParent;
                    WtParentInv.Transpose();
                    Local = WtParentInv * Wt;
                }

                aiVector3D Translation = rb.pos;
                // Modo copy: la animacion se copio entre armatures con las posiciones de TODOS los huesos
                // animados (no solo la cadera), asi que se aplica el desplazamiento de cada hueso.
                if ((rb.isRoot || animCopyLocal) && pNodeAnim->mNumPositionKeys > 0) {
                    // Desplazamiento respecto al reposo de la biblioteca, escalado, sobre el reposo del
                    // modelo: asi se conserva el desfase propio de la cadera de cada personaje.
                    aiVector3D Animated;
                    CalcInterpolatedPosition(Animated, AnimationTime, pNodeAnim);
                    Translation = rb.pos + (Animated - rb.libRestPos) * (animCopyLocal ? 1.0f : rb.rootScale);
                }

                aiMatrix4x4 ScalingM;
                aiMatrix4x4::Scaling(rb.scale, ScalingM);
                aiMatrix4x4 TranslationM;
                aiMatrix4x4::Translation(Translation, TranslationM);
                NodeTransformation = TranslationM * aiMatrix4x4(Local) * ScalingM;
            } else {
                // Sin movimiento en la biblioteca (hueso que no tiene): se queda en su reposo.
                retargetWl[rb.idx] = WlParent * rb.modelRestLocal;
                retargetWt[rb.idx] = WtParent * rb.modelRestLocal;
            }
        } else if (animCopyLocal && pNodeAnim != nullptr) {
            // Nodo que no es hueso (p. ej. el armature "RTS") con animacion propia en la biblioteca: en una
            // familia copiada entre armatures ese movimiento a nivel de objeto es parte de la animacion.
            aiVector3D Scaling;
            CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
            aiMatrix4x4 ScalingM;
            aiMatrix4x4::Scaling(Scaling, ScalingM);

            aiQuaternion RotationQ;
            CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);

            aiVector3D Translation;
            CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
            aiMatrix4x4 TranslationM;
            aiMatrix4x4::Translation(Translation, TranslationM);

            NodeTransformation = TranslationM * aiMatrix4x4(RotationQ.GetMatrix()) * ScalingM;
        }
    } else if (pNodeAnim != nullptr) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        aiMatrix4x4 ScalingM = aiMatrix4x4();
        aiMatrix4x4::Scaling(Scaling, ScalingM);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        aiMatrix4x4 RotationM = aiMatrix4x4(RotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        aiMatrix4x4 TranslationM;
        aiMatrix4x4::Translation(Translation, TranslationM);

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    return NodeTransformation;
}

// ── Esqueleto precalculado ───────────────────────────────────────────────────────────────────────
// El recorrido por nombre (recursivo) resolvía, en cada nodo y cada frame, el canal de animación, el reposo retargeteado
// y el hueso del modelo (tres búsquedas en mapas de string). Aquí se resuelven una vez por animación y el recorrido por
// frame es un bucle plano sobre un vector. Se invalida solo (SkeletonCacheValid) si cambia la animación o el esqueleto.
bool Mesh3DAnimation::SkeletonCacheValid() const
{
    return skelValid
        && skelKeyThis == this
        && skelKeyScene == scene
        && skelKeyAnimScene == animScene
        && skelKeyAnim == indexCurrentAnimation
        && skelKeyRetarget == retargetBones.size()
        && skelKeyBones == boneInfo.size()
        && skelKeyMeshes == meshes.size();
}

void Mesh3DAnimation::BuildSkeletonCache()
{
    skelNodes.clear();

    const aiAnimation *pAnimation = animationsScene()->mAnimations[indexCurrentAnimation];

    // Preorden con los hijos en su orden (lo que hacía la recursión): el padre siempre se evalúa antes que sus hijos,
    // que es lo que necesitan tanto la transformación global como retargetWl/retargetWt.
    struct Item { const aiNode *node; int parent; };
    std::vector<Item> stack;
    stack.push_back({scene->mRootNode, -1});
    while (!stack.empty()) {
        const Item item = stack.back();
        stack.pop_back();

        SkelNode sn;
        sn.node   = item.node;
        sn.parent = item.parent;
        sn.anim   = FindNodeAnim(pAnimation, item.node->mName.data);
        if (animScene) {
            auto it = retargetBones.find(item.node);
            if (it != retargetBones.end()) sn.rb = &it->second;
        }
        auto bit = boneMapping.find(item.node->mName.data);
        if (bit != boneMapping.end()) sn.boneIndex = static_cast<int>(bit->second);

        const int idx = static_cast<int>(skelNodes.size());
        skelNodes.push_back(sn);
        for (int c = static_cast<int>(item.node->mNumChildren) - 1; c >= 0; --c) {
            stack.push_back({item.node->mChildren[c], idx});
        }
    }
    skelGlobal.assign(skelNodes.size(), aiMatrix4x4());

    // Offset de bind por (mesh, hueso): lo que ComputeMeshBoneFinalTransforms buscaba en un mapa por cada hueso y frame.
    // Se copian a un vector contiguo: los nodos del unordered_map están dispersos en memoria y recorrerlos por cada
    // (mesh, hueso) y frame, de unas 200 unidades, era más lento que el propio producto de matrices.
    skelOffsetStore.assign(meshes.size() * static_cast<size_t>(numBones), aiMatrix4x4());
    skelMeshOffsets.assign(meshes.size(), std::vector<const aiMatrix4x4*>());
    for (size_t m = 0; m < meshes.size(); m++) {
        auto &offsets = skelMeshOffsets[m];
        offsets.assign(numBones, nullptr);
        for (int b = 0; b < numBones && b < static_cast<int>(boneInfo.size()); b++) {
            auto it = boneInfo[b].BoneOffsetByMesh.find(static_cast<int>(m));
            if (it == boneInfo[b].BoneOffsetByMesh.end()) continue;
            aiMatrix4x4 &dst = skelOffsetStore[m * static_cast<size_t>(numBones) + static_cast<size_t>(b)];
            dst = it->second;
            offsets[b] = &dst;
        }
    }
    meshBoneFinalTransforms.clear();   // se reasignan (a identidad) en ComputeMeshBoneFinalTransforms

    skelKeyThis      = this;
    skelKeyScene     = scene;
    skelKeyAnimScene = animScene;
    skelKeyAnim      = indexCurrentAnimation;
    skelKeyRetarget  = retargetBones.size();
    skelKeyBones     = boneInfo.size();
    skelKeyMeshes    = meshes.size();
    skelValid        = true;
}

void Mesh3DAnimation::ReadNodeHierarchyCached(float AnimationTime)
{
    if (!SkeletonCacheValid()) BuildSkeletonCache();

    const aiMatrix4x4 Identity;
    for (size_t i = 0; i < skelNodes.size(); i++) {
        const SkelNode &sn = skelNodes[i];

        const aiMatrix4x4 NodeTransformation = NodeLocalTransform(AnimationTime, sn.node, sn.anim, sn.rb);
        const aiMatrix4x4 &Parent = sn.parent >= 0 ? skelGlobal[sn.parent] : Identity;
        skelGlobal[i] = Parent * NodeTransformation;

        if (sn.boneIndex >= 0) {
            boneInfo[sn.boneIndex].WorldTransform = globalInverseTransform * skelGlobal[i];
        }
    }
}

// Combina el WorldTransform (compartido) de cada hueso con el offset propio de CADA mesh
// para producir la matriz de skinning real que usa ese mesh. Un mesh que no use un hueso
// concreto se queda con la identidad para ese slot (da igual: sus vertices nunca tendran
// peso > 0 para ese hueso).
void Mesh3DAnimation::ComputeMeshBoneFinalTransforms()
{
    if (!SkeletonCacheValid()) BuildSkeletonCache();

    size_t meshCount = meshes.size();
    if (meshBoneFinalTransforms.size() != meshCount) meshBoneFinalTransforms.resize(meshCount);

    for (size_t meshId = 0; meshId < meshCount; meshId++) {
        auto &transforms = meshBoneFinalTransforms[meshId];
        // Los huesos que este mesh no usa se quedan en identidad: solo se reasigna si cambió el nº de huesos.
        if (transforms.size() != static_cast<size_t>(numBones)) transforms.assign(numBones, aiMatrix4x4());

        const auto &offsets = skelMeshOffsets[meshId];
        for (int b = 0; b < numBones; b++) {
            if (offsets[b] != nullptr) {
                MulAi(boneInfo[b].WorldTransform, *offsets[b], transforms[b]);
            }
        }
    }
}

const aiNodeAnim *Mesh3DAnimation::FindNodeAnim(const aiAnimation *pAnimation, const std::string& NodeName)
{
    if (nodeAnimCache.empty()) {
        for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
            nodeAnimCache[pAnimation->mChannels[i]->mNodeName.C_Str()] = pAnimation->mChannels[i];
        }
    }
    auto it = nodeAnimCache.find(NodeName);
    return it != nodeAnimCache.end() ? it->second : nullptr;
}

unsigned int Mesh3DAnimation::FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);
    unsigned int lo = 0, hi = pNodeAnim->mNumRotationKeys - 2;
    while (lo < hi) {
        unsigned int mid = (lo + hi + 1) / 2;
        if (static_cast<float>(pNodeAnim->mRotationKeys[mid].mTime) <= AnimationTime)
            lo = mid;
        else
            hi = mid - 1;
    }
    return lo;
}

unsigned int Mesh3DAnimation::FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    unsigned int lo = 0, hi = pNodeAnim->mNumPositionKeys - 2;
    while (lo < hi) {
        unsigned int mid = (lo + hi + 1) / 2;
        if (static_cast<float>(pNodeAnim->mPositionKeys[mid].mTime) <= AnimationTime)
            lo = mid;
        else
            hi = mid - 1;
    }
    return lo;
}

void Mesh3DAnimation::UpdateForBone(Vertex3D &V, int meshID, int vertexID)
{
    if (numBones == 0) return;
    if (meshID < 0 || static_cast<size_t>(meshID) >= meshBoneFinalTransforms.size()) return;
    const auto &meshTransforms = meshBoneFinalTransforms[meshID];

    glm::mat4 BoneTransform(0);
    for (int n = 0; n < NUM_BONES_PER_VERTEX; n++) {
        auto boneData = meshVerticesBoneData[meshID][vertexID];
        unsigned int boneId = boneData.IDs[n];
        float weight = boneData.Weights[n];
        if (weight <= 0.0f || boneId >= meshTransforms.size()) continue;
        //LOG_MESSAGE("ID: %d, Weight: %f, vertexID: %d", boneId, weight, vertexID);
        BoneTransform += Tools::aiMat4toGLMMat4(meshTransforms[boneId]) * weight;
    }

    V = Vertex3D::fromGLM(BoneTransform * V.toGLM4());
}

unsigned int Mesh3DAnimation::FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);
    unsigned int lo = 0, hi = pNodeAnim->mNumScalingKeys - 2;
    while (lo < hi) {
        unsigned int mid = (lo + hi + 1) / 2;
        if (static_cast<float>(pNodeAnim->mScalingKeys[mid].mTime) <= AnimationTime)
            lo = mid;
        else
            hi = mid - 1;
    }
    return lo;
}

void Mesh3DAnimation::CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    unsigned int NextRotationIndex = (RotationIndex + 1);
    //assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    auto DeltaTime = static_cast<float>(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    auto Factor = DeltaTime > 0.0f
        ? (AnimationTime - static_cast<float>(pNodeAnim->mRotationKeys[RotationIndex].mTime)) / DeltaTime
        : 0.0f;
    Factor = Factor < 0.0f ? 0.0f : (Factor > 1.0f ? 1.0f : Factor);
    const aiQuaternion &StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion &EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;

    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);

    Out = Out.Normalize();
}

void Mesh3DAnimation::CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    unsigned int NextPositionIndex = (PositionIndex + 1);

    auto DeltaTime = static_cast<float>(pNodeAnim->mPositionKeys[NextPositionIndex].mTime -
                                        pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = DeltaTime > 0.0f
        ? (AnimationTime - static_cast<float>(pNodeAnim->mPositionKeys[PositionIndex].mTime)) / DeltaTime
        : 0.0f;
    Factor = Factor < 0.0f ? 0.0f : (Factor > 1.0f ? 1.0f : Factor);
    const aiVector3D &Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D &End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void Mesh3DAnimation::CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    unsigned int NextScalingIndex = ScalingIndex + 1;
    auto DeltaTime = static_cast<float>(pNodeAnim->mScalingKeys[NextScalingIndex].mTime -
                                        pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = DeltaTime > 0.0f
        ? (AnimationTime - static_cast<float>(pNodeAnim->mScalingKeys[ScalingIndex].mTime)) / DeltaTime
        : 0.0f;
    Factor = Factor < 0.0f ? 0.0f : (Factor > 1.0f ? 1.0f : Factor);
    const aiVector3D &Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D &End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    Out = Start + Factor * (End - Start);
}

void Mesh3DAnimation::DrawBones(aiNode *node, Vertex3D *lastBonePosition)
{
    if (boneMapping.find(node->mName.C_Str()) != boneMapping.end()) {
        int idCurrentNode = static_cast<int>(boneMapping[node->mName.C_Str()]);

        aiMatrix4x4 mT = boneInfo[idCurrentNode].WorldTransform;
        aiVector3D aBonePosition;

        aBonePosition = mT * aBonePosition;

        Vertex3D bonePosition = Vertex3D::fromAssimp(aBonePosition);
        Transforms::objectSpace(bonePosition, bonePosition, this);

        if (lastBonePosition) {
            auto render = Components::get()->Render();
            auto window = Components::get()->Window();
            render->getShaders()->shaderOGLLine3D->render(
                *lastBonePosition,
                bonePosition,
                window->getForegroundFramebuffer(),
                Color::yellow()
            );
        }

        lastBonePosition = &bonePosition;
    }

    for (int j = 0; j < static_cast<int>(node->mNumChildren); j++) {
        DrawBones(node->mChildren[j], lastBonePosition);
    }
}

void Mesh3DAnimation::setRemoveAtEndAnimation(bool removeAtEnds)
{
    removeOnAnimationEnd = removeAtEnds;
}

void Mesh3DAnimation::DrawPropertiesGUI()
{
    Mesh3D::DrawPropertiesGUI();
    Mesh3DAnimationDrawerGUI::DrawPropertiesGUI(this);
}

void Mesh3DAnimation::setAnimationSpeed(float value)
{
    animation_speed = value;
}

Mesh3DAnimation::~Mesh3DAnimation()
{
    meshVerticesBoneData.clear();
    meshVertices.clear();
    boneMapping.clear();
    boneInfo.clear();

    for (auto &c : boneMappingColliders) {
        for (auto &bone : c.boneColliderInfo) {
            if (bone.ghostObject != nullptr) {
                Components::get()->Collisions()->getDynamicsWorld()->removeCollisionObject(bone.ghostObject);
                delete bone.ghostObject;
            }
            if (bone.convexHullShape != nullptr) {
                delete bone.convexHullShape;
            }
        }
    }
}

void Mesh3DAnimation::FillOGLBuffers()
{
    LOG_MESSAGE("[Mesh3DAnimation] Filling buffers (with feedback)...");

    if (!sharedAnimModel) {
        // Sin AnimationData compartido (carga fallida): comportamiento antiguo.
        ComponentRender::FillOGLBuffers(meshes, true);
        RegisterSubmeshPicking();
        return;
    }

    // Fase 2.1 extendida a animacion: vertexBuffer/uvBuffer/normalBuffer (pose de reposo,
    // solo lectura para el transform feedback) se comparten entre instancias del mismo
    // fichero. feedbackBuffer/feedbackNormalBuffer (la pose ANIMADA actual) son siempre
    // por instancia -- se crean aqui igual que antes, nunca compartidos.
    for (size_t i = 0; i < meshes.size() && i < sharedAnimModel->meshes.size(); i++) {
        auto &m = meshes[i];
        auto &shared = sharedAnimModel->meshes[i];

        if (m.vertices.empty() || m.uvs.empty() || m.normals.empty()) {
            LOG_ERROR("[Mesh3DAnimation] FillOGLBuffers: mesh with empty geometry (vertices=%zu uvs=%zu normals=%zu) — skipped",
                m.vertices.size(), m.uvs.size(), m.normals.size());
            continue;
        }

        if (!glIsBuffer(shared.vertexBuffer)) {
            glGenBuffers(1, &shared.vertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, shared.vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(shared.vertices.size() * sizeof(glm::vec4)), shared.vertices.data(), GL_STATIC_DRAW);

            glGenBuffers(1, &shared.uvBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, shared.uvBuffer);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(shared.uvs.size() * sizeof(glm::vec2)), shared.uvs.data(), GL_STATIC_DRAW);

            glGenBuffers(1, &shared.normalBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, shared.normalBuffer);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(shared.normals.size() * sizeof(glm::vec3)), shared.normals.data(), GL_STATIC_DRAW);
        }

        m.vertexBuffer = shared.vertexBuffer;
        m.uvBuffer = shared.uvBuffer;
        m.normalBuffer = shared.normalBuffer;

        // Salida del transform feedback: siempre propia de esta instancia.
        glGenBuffers(1, &m.feedbackBuffer);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m.feedbackBuffer);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, static_cast<GLuint>(m.vertices.size() * sizeof(glm::vec4)), m.vertices.data(), GL_DYNAMIC_COPY);

        glGenBuffers(1, &m.feedbackNormalBuffer);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m.feedbackNormalBuffer);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, static_cast<GLuint>(m.normals.size() * sizeof(glm::vec3)), m.normals.data(), GL_DYNAMIC_COPY);

        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
    }

    RegisterSubmeshPicking();
}

void Mesh3DAnimation::FillAnimationBoneDataOGLBuffers()
{
    if (sharedAnimModel) {
        // Fase 2.1 extendida a animacion: vertexBoneDataBuffer (pesos/indices de hueso por
        // vertice) tambien es igual entre instancias del mismo fichero -- se comparte igual
        // que vertexBuffer/uvBuffer/normalBuffer en FillOGLBuffers(). meshVerticesBoneData
        // (el dato de CPU) se sigue calculando por instancia por ahora; solo se evita
        // repetir la subida a GPU.
        for (size_t i = 0; i < meshes.size() && i < sharedAnimModel->meshes.size(); i++) {
            if (meshes[i].vertices.empty()) continue;
            auto &shared = sharedAnimModel->meshes[i];

            if (!glIsBuffer(shared.vertexBoneDataBuffer)) {
                glGenBuffers(1, &shared.vertexBoneDataBuffer);
                glBindBuffer(GL_ARRAY_BUFFER, shared.vertexBoneDataBuffer);
                glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(meshVerticesBoneData[i].size() * sizeof(VertexBoneData)), meshVerticesBoneData[i].data(), GL_STATIC_DRAW);
            }

            meshes[i].vertexBoneDataBuffer = shared.vertexBoneDataBuffer;
        }
        return;
    }

    for (int i = 0; i < (int)meshes.size(); i++) {
        if (meshes[i].vertices.empty()) continue;

        if (!glIsBuffer(meshes[i].vertexBoneDataBuffer)) {
            glGenBuffers(1, &meshes[i].vertexBoneDataBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, meshes[i].vertexBoneDataBuffer);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(meshVerticesBoneData[i].size() * sizeof(VertexBoneData)), meshVerticesBoneData[i].data(), GL_STATIC_DRAW);
        }
    }
}

void Mesh3DAnimation::UpdateBoundingBox()
{
    glm::mat4 mvpMatrix = getModelMatrix();

    float maxX = -FLT_MAX, minX = FLT_MAX, maxY = -FLT_MAX, minY = FLT_MAX, maxZ = -FLT_MAX, minZ = FLT_MAX;

    std::vector<Vertex3D> vertices;

    for (int i = 0; i < static_cast<int>(scene->mNumMeshes); i++) {

        if (meshVertices[i].empty()) continue;

        for (unsigned int k = 0; k < scene->mMeshes[i]->mNumFaces; k++) {

            const aiFace &Face = scene->mMeshes[i]->mFaces[k];

            auto v1Index= Face.mIndices[0];
            auto v2Index= Face.mIndices[1];
            auto v3Index= Face.mIndices[2];

            Vertex3D V1 = meshVertices[i][v1Index];
            Vertex3D V2 = meshVertices[i][v2Index];
            Vertex3D V3 = meshVertices[i][v3Index];

            UpdateForBone(V1, i, static_cast<int>(v1Index));
            UpdateForBone(V2, i, static_cast<int>(v2Index));
            UpdateForBone(V3, i, static_cast<int>(v3Index));

            vertices.emplace_back(V1);
            vertices.emplace_back(V2);
            vertices.emplace_back(V3);
        }
    }

    for (auto &vertex : vertices) {
        glm::vec4 transformedVertex = mvpMatrix * vertex.toGLM4();
        transformedVertex /= transformedVertex.w;

        Vertex3D V(transformedVertex.x, transformedVertex.y, transformedVertex.z);

        maxX = std::max(maxX, V.x);
        minX = std::min(minX, V.x);
        maxY = std::max(maxY, V.y);
        minY = std::min(minY, V.y);
        maxZ = std::max(maxZ, V.z);
        minZ = std::min(minZ, V.z);
    }

    this->aabb.max.x = maxX;
    this->aabb.max.y = maxY;
    this->aabb.max.z = maxZ;

    this->aabb.min.x = minX;
    this->aabb.min.y = minY;
    this->aabb.min.z = minZ;

    this->aabb.updateVertices();

    const Vertex3D& pos = getPosition();
    float maxR2 = 0.f;
    for (const auto& v : this->aabb.vertices) {
        float dx = v.x - pos.x, dy = v.y - pos.y, dz = v.z - pos.z;
        float r2 = dx*dx + dy*dy + dz*dz;
        if (r2 > maxR2) maxR2 = r2;
    }
    boundingRadius = sqrtf(maxR2);
}

void Mesh3DAnimation::setAnimationByName(const std::string& name)
{
    const aiScene* as = animationsScene();
    if (!as) return;
    for (int i = 0; i < static_cast<int>(as->mNumAnimations); i++) {
        if (name == as->mAnimations[i]->mName.C_Str()) {
            setIndexCurrentAnimation(i);
            return;
        }
    }
    // DIAGNOSTICO TEMPORAL (2026-09-20): sin match, esta funcion no hacia NADA -- ni error, ni
    // fallback -- dejando la animacion actual (o el indice 0 por defecto tras la carga) puesta
    // para siempre, indistinguible en Lua de un cambio de animacion exitoso (RTSUtils.playAnim/
    // CivilianManager.setAnim marcan su cache de "animacion ya aplicada" igual en ambos casos).
    // Sospecha: policias/civiles quedan pegados en la PRIMERA animacion del FBX (aparenta "dando
    // punetazos") porque el nombre pedido desde Lua no hace match exacto contra
    // scene->mAnimations[i]->mName tal y como lo parsea Assimp para ese fichero en concreto.
    LOG_MESSAGE("[Mesh3DAnimation] setAnimationByName: NO MATCH para '%s' (mesh '%s', %u animaciones disponibles)",
        name.c_str(), this->getName().c_str(), as->mNumAnimations);
    for (unsigned int i = 0; i < as->mNumAnimations; i++) {
        LOG_MESSAGE("[Mesh3DAnimation]   disponible[%u]: '%s'", i, as->mAnimations[i]->mName.C_Str());
    }
}

void Mesh3DAnimation::createBonesMappingColliders(const std::string &name)
{
    BonesMappingColliders bmc;
    bmc.nameMapping = name;
    bmc.boneColliderInfo.resize(this->numBones);

    for (int i = 0; i < (int)boneInfo.size(); i++) {
        auto ci = bmc.boneColliderInfo[i];
        bmc.boneColliderInfo[i].ghostObject = nullptr;
        bmc.boneColliderInfo[i].shape = BONE_SPHERE;
        bmc.boneColliderInfo[i].enabled = false;
        bmc.boneColliderInfo[i].name = boneInfo[i].name;
        bmc.boneColliderInfo[i].boneId = i;
    }

    boneMappingColliders.push_back(bmc);

    if (boneColliderIndex <= 0) {
        boneColliderIndex = 0;
    }
}

void Mesh3DAnimation::SetMappingBoneColliderInfo(
    const std::string& mappingName,
    unsigned int boneId,
    bool enabled,
    BoneCollisionShape shape = BONE_SPHERE
) {
    LOG_MESSAGE("[Mesh3DAnimation] Setting Bone %d for %s to %d", boneId, mappingName.c_str(), enabled);

    int im;
    auto mapping = getBonesMappingByName(mappingName, im);
    auto &ci = mapping->boneColliderInfo[boneId];

    ci.boneId = boneId;
    ci.shape = shape;
    ci.enabled = enabled;
    ci.name = boneInfo[boneId].name;

    auto world = Components::get()->Collisions()->getDynamicsWorld();

    if (enabled) {
        if (ci.ghostObject == nullptr ) {
            createBoneGhostBody(im, boneId, shape, ci);
        } else {
            world->removeCollisionObject(ci.ghostObject);
            ci.ghostObject = nullptr;
        }
    } else {
        if (ci.ghostObject != nullptr ) {
            world->removeCollisionObject(ci.ghostObject);
            ci.ghostObject = nullptr;
        }
    }
}

void Mesh3DAnimation::createBoneGhostBody(int bmIndex, unsigned int boneId, const BoneCollisionShape &shape, BoneColliderInfo &ci)
{
    aiVector3t<float> scaling, rotationAxis, position;
    float rotationAngle;

    aiMatrix4x4t<float> FinalTransformation = boneInfo[boneId].WorldTransform;
    FinalTransformation.Decompose(scaling, rotationAxis, rotationAngle, position);

    ci.position = Vertex3D(position.x, position.y, position.z);
    ci.size = Vertex3D(0.1, 0.1, 0.1);

    btTransform transformation;
    transformation.setOrigin((getPosition() + ci.position).toBullet());

    btMatrix3x3 brakezaRotation;
    brakezaRotation.setIdentity();

    btQuaternion qRotation;
    brakezaRotation.getRotation(qRotation);
    transformation.setRotation(qRotation);

    btConvexHullShape *convexHullShape = nullptr;
    switch (shape) {
        case BONE_SPHERE: {
            convexHullShape = reinterpret_cast<btConvexHullShape *>(new btSphereShape(ci.size.x));;
            break;
        }
        case BONE_CAPSULE: {
            convexHullShape = reinterpret_cast<btConvexHullShape *>(new btCapsuleShape(ci.size.x, ci.size.y));;
            break;
        }
        case BONE_CUBE: {
            convexHullShape = reinterpret_cast<btConvexHullShape *>(new btBoxShape(ci.size.toBullet()));;
            break;
        }
    }

    if (convexHullShape == nullptr) {
        printf("Collider Shape not valid!!. Exiting...");
        exit(-1);
    }

    ci.ghostObject = new btPairCachingGhostObject();
    ci.ghostObject->setCollisionShape(convexHullShape);
    ci.ghostObject->setWorldTransform(transformation);
    ci.ghostObject->setUserPointer(this);
    ci.ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
    ci.ghostObject->setUserIndex(Config::CollisionSource::BONE_COLLIDER);
    ci.ghostObject->setUserIndex2(bmIndex);

    Components::get()->Collisions()->getDynamicsWorld()->addCollisionObject(
        ci.ghostObject,
        btBroadphaseProxy::DefaultFilter,
        btBroadphaseProxy::DefaultFilter
    );
}

void Mesh3DAnimation::UpdateBoneColliders()
{
    if (boneColliderIndex < 0) return;

    for (auto &bc : boneMappingColliders) {
        for (auto &b : bc.boneColliderInfo) {
            if (b.enabled) {
                if (b.ghostObject == nullptr) {
                    continue;
                }

                aiMatrix4x4 mT = boneInfo[b.boneId].WorldTransform;
                aiVector3D aBonePosition;

                aBonePosition = mT * aBonePosition;

                Vertex3D bonePosition = Vertex3D::fromAssimp(aBonePosition);
                Transforms::objectSpace(bonePosition, bonePosition, this);

                btTransform transformation;
                transformation.setOrigin(bonePosition.toBullet());

                btMatrix3x3 brakezaRotation; //rotation.toBulletMat3();
                brakezaRotation.setIdentity();

                btQuaternion qRotation;
                brakezaRotation.getRotation(qRotation);
                transformation.setRotation(qRotation);
                b.ghostObject->setWorldTransform(transformation);
            }
        }
    }
}

BonesMappingColliders *Mesh3DAnimation::getBonesMappingByName(const std::string& name, int &index)
{
    for (int i = 0; i < (int)boneMappingColliders.size(); i++) {
        if (boneMappingColliders[i].nameMapping == name) {
            index = i;
            return &boneMappingColliders[i];
        }
    }

    return nullptr;
}

const std::vector<BonesMappingColliders> *Mesh3DAnimation::getBoneMappingColliders() const {
    return &boneMappingColliders;
}

Vertex3D Mesh3DAnimation::getBoneWorldPosition(const std::string& boneName) const
{
    auto it = boneMapping.find(boneName);
    if (it == boneMapping.end()) return getPosition();

    unsigned int boneId = it->second;
    if (boneId >= boneInfo.size()) return getPosition();

    aiMatrix4x4 mT      = boneInfo[boneId].WorldTransform;
    aiVector3D  aBonePos;

    aBonePos = mT * aBonePos;

    Vertex3D bonePosition = Vertex3D::fromAssimp(aBonePos);
    Transforms::objectSpace(bonePosition, bonePosition, const_cast<Mesh3DAnimation*>(this));
    return bonePosition;
}

M3 Mesh3DAnimation::getBoneWorldRotation(const std::string& boneName) const
{
    auto it = boneMapping.find(boneName);
    if (it == boneMapping.end()) return getRotation();

    unsigned int boneId = it->second;
    if (boneId >= boneInfo.size()) return getRotation();

    // WorldTransform = globalInverseTransform * GlobalTransformation (sin offset de mesh).
    // Upper-left 3x3 de esto da la orientacion del hueso en model space.
    aiMatrix4x4 mLocal = boneInfo[boneId].WorldTransform;

    // Normalize columns to strip scale
    float lenX = std::sqrt(mLocal.a1*mLocal.a1 + mLocal.b1*mLocal.b1 + mLocal.c1*mLocal.c1);
    float lenY = std::sqrt(mLocal.a2*mLocal.a2 + mLocal.b2*mLocal.b2 + mLocal.c2*mLocal.c2);
    float lenZ = std::sqrt(mLocal.a3*mLocal.a3 + mLocal.b3*mLocal.b3 + mLocal.c3*mLocal.c3);
    if (lenX < 1e-6f) lenX = 1.0f;
    if (lenY < 1e-6f) lenY = 1.0f;
    if (lenZ < 1e-6f) lenZ = 1.0f;

    M3 localRot(
        mLocal.a1/lenX, mLocal.a2/lenY, mLocal.a3/lenZ,
        mLocal.b1/lenX, mLocal.b2/lenY, mLocal.b3/lenZ,
        mLocal.c1/lenX, mLocal.c2/lenY, mLocal.c3/lenZ
    );

    return getRotation() * localRot;
}

std::vector<std::string> Mesh3DAnimation::getBoneNames() const
{
    std::vector<std::string> names;
    names.reserve(boneMapping.size());
    for (const auto& kv : boneMapping) names.push_back(kv.first);
    return names;
}

void Mesh3DAnimation::removeBonesColliderMapping(const std::string& name)
{
    int im;
    auto bm= getBonesMappingByName(name, im);
    LOG_MESSAGE("Deleting bone collider mapping: %s, new boneColliderIndex: %d", name.c_str(), boneColliderIndex);

    if (bm == nullptr) return;

    auto world = Components::get()->Collisions()->getDynamicsWorld();

    for (auto &b : bm->boneColliderInfo) {
        if (b.ghostObject == nullptr) continue;
        world->removeCollisionObject(b.ghostObject);
        b.ghostObject = nullptr;
        b.enabled = false;
    }

    for (auto it = boneMappingColliders.begin(); it != boneMappingColliders.end();) {
        if (it->nameMapping == name) {
            it = boneMappingColliders.erase(it);
        } else {
            ++it;
        }
    }

    if (boneMappingColliders.empty()) {
        boneColliderIndex = -1;
    } else if (boneColliderIndex >= (int)boneMappingColliders.size()) {
        boneColliderIndex = static_cast<int>(boneMappingColliders.size()) - 1; // Ajustar al último elemento válido
    }
}

void Mesh3DAnimation::ResolveCollision(CollisionInfo with)
{
    if (Config::get()->LOG_COLLISION_OBJECTS) {
        auto *object = static_cast<Object3D *>(with.with);

        if (with.source == Config::CollisionSource::OBJECT_COLLIDER) {
            LOG_MESSAGE("Mesh3DAnimation: Collision %s with object: %s",  getName().c_str(), object->getName().c_str());
        }

        if (with.source == Config::CollisionSource::BONE_COLLIDER) {
            LOG_MESSAGE("Mesh3DAnimation: Collision %s Bone Collider: %s, with object: %s",  getName().c_str(), boneMappingColliders[with.boneIndexMapping].nameMapping.c_str(), object->getName().c_str());
        }
    }

    if (Components::get()->Scripting()->isExecuting()) {
        RunResolveCollisionScripts(with);
    }
}

void Mesh3DAnimation::ShadowMappingPass()
{
    auto render = Components::get()->Render();
    auto shaderShadowPass = render->getShaders()->shaderShadowPass;
    auto shaderRender = render->getShaders()->shaderOGLRender;

    // Directional Light
    shaderShadowPass->renderMeshIntoDirectionalLightTexture(this, true, shaderRender->getDirectionalLight());

    // SpotLights
    const auto shadowSpotLights = shaderRender->getShadowMappingSpotLights();
    const auto numSpotLights = static_cast<int>(shadowSpotLights.size());

    for (int i = 0; i < numSpotLights; i++) {
        shaderShadowPass->renderMeshIntoArrayTextures(this, true, shadowSpotLights[i], i);
    }
}

void Mesh3DAnimation::setLoop(bool value)
{
    loop = value;
}

void Mesh3DAnimation::setIndexCurrentAnimation(int indexCurrentAnimation)
{
    if (Mesh3DAnimation::indexCurrentAnimation != indexCurrentAnimation) {
        nodeAnimCache.clear();
        runningTime = 0;
    }
    Mesh3DAnimation::indexCurrentAnimation = indexCurrentAnimation;
}
