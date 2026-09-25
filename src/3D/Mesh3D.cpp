#define GL_GLEXT_PROTOTYPES

#include <vector>
#include <mutex>
#include <assimp/cimport.h>
#include <glm/ext/matrix_float4x4.hpp>
#include "../../include/3D/Mesh3D.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Brakeza.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Render/Mesh3DShaderChain.h"
#include "../../include/OpenGL/Code/ShaderOGLCustomCodeMesh3D.h"
#include "../../include/OpenGL/ShaderOGLShadowPass.h"
#include <assimp/postprocess.h>

#include "../../include/Cache/ImageCache.h"
#include "../../include/Cache/ModelDataCache.h"
#include "../../include/Cache/ModelData.h"
#include "../../include/Components/Components.h"
#include "../../include/GUI/Objects/Mesh3DGUI.h"
#include "../../include/GUI/Objects/ShadersGUI.h"
#include "../../include/OpenGL/Nodes/ShaderNodesMesh3D.h"
#include "../../include/Render/JSONSerializerRegistry.h"
#include "../../include/Threads/ThreadJobMakeRigidBody.h"
#include <BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>

Mesh3D::Mesh3D()
{
    shaderChain = new Mesh3DShaderChain();
}

Mesh3D::Mesh3D(const FilePath::ModelFile& modelFile)
:
    sourceFile(modelFile)
{
    shaderChain = new Mesh3DShaderChain();
}

Mesh3D::~Mesh3D()
{
    LOG_VERBOSE("[Mesh3D] Destroying '%s'...", getName().c_str());

    for (auto &m : meshes) {
        for (auto triangle : m.modelTriangles) delete triangle;
        for (auto vertex : m.modelVertices) delete vertex;

        // vertexBuffer/uvBuffer/normalBuffer/vertexBoneDataBuffer son compartidos (Fase 2.1,
        // extendida a animacion) cuando sharedStaticGeometry esta a true: los libera el
        // destructor del recurso compartido (ModelData o AnimationData) cuando el ultimo
        // shared_ptr desaparece, no esta instancia. Sin eso (fallback), siguen siendo
        // propios de esta malla.
        if (!sharedStaticGeometry) {
            if (glIsBuffer(m.vertexBuffer))
                glDeleteBuffers(1, &m.vertexBuffer);

            if (glIsBuffer(m.uvBuffer))
                glDeleteBuffers(1, &m.uvBuffer);

            if (glIsBuffer(m.normalBuffer))
                glDeleteBuffers(1, &m.normalBuffer);

            if (glIsBuffer(m.vertexBoneDataBuffer))
                glDeleteBuffers(1, &m.vertexBoneDataBuffer);

            if (glIsBuffer(m.indexBuffer))
                glDeleteBuffers(1, &m.indexBuffer);
        }

        // feedbackBuffer/feedbackNormalBuffer son la pose ANIMADA actual de esta instancia
        // (salida del transform feedback) -- nunca se comparten, siempre propios.
        if (glIsBuffer(m.feedbackBuffer))
            glDeleteBuffers(1, &m.feedbackBuffer);

        if (glIsBuffer(m.feedbackNormalBuffer))
            glDeleteBuffers(1, &m.feedbackNormalBuffer);
    }

    UnregisterSubmeshPicking();

    if (!sharedTextures) {
        modelTextures.clear();
        modelSpecularTextures.clear();
    }
    
    if (shaderChain) {
        delete shaderChain;
        shaderChain = nullptr;
    }
}

void Mesh3D::AssimpLoadGeometryFromFile(const FilePath::ModelFile &fileName)
{
    if (!Tools::FileExists(fileName.c_str())) {
        LOG_ERROR("[Mesh3D] Error import 3D file not exist");
        loadFailed = true;
        return;
    }

    // fnProcess() de ThreadJobLoadMesh3D llama a esta funcion desde threads distintos del pool.
    // get()+parse+store() no es atomico -- sin este lock, anadir el mismo fichero varias veces
    // seguidas (p.ej. 3 clicks rapidos en el menu Mesh3D) puede lanzar 3 jobs en paralelo que vean
    // los 3 cache MISS a la vez, cada uno subiendo su propio VBO/EBO -- geometria duplicada que
    // nunca comparte buffer, así que el batching/instancing de Fase 4 (sameBatch() en
    // ComponentRender::FlushOpaqueQueue) nunca los agrupa aunque sea literalmente el mismo modelo.
    // Lock POR FICHERO (antes global): ficheros distintos se cargan en paralelo.
    auto keyLoadMutex = modelDataCache.getKeyLoadMutex(fileName);
    std::unique_lock<std::mutex> cacheLoadLock(*keyLoadMutex);

    auto cached = modelDataCache.get(fileName);
    if (cached) {
        cacheLoadLock.unlock(); // ya cacheado: el clonado por instancia no necesita serializarse
        LOG_MESSAGE("[Mesh3D] Cache HIT for '%s'", fileName.c_str());
        cached->cloneInto(*this);
        sharedModel = cached;
        sharedStaticGeometry = true;
        UpdateBoundingBox();
        loaded = true;
        return;
    }

    LOG_MESSAGE("[Mesh3D] Cache MISS, loading '%s'...", fileName.c_str());

    Assimp::Importer assimpImporter;
    const aiScene *scene = assimpImporter.ReadFile(
        fileName,
        aiProcess_Triangulate |
        aiProcess_SortByPType |
        aiProcess_FlipUVs |
        aiProcess_GenSmoothNormals |
        aiProcess_FixInfacingNormals
    );

    if (!scene) {
        LOG_ERROR("[Mesh3D] ERROR loading '%s': %s", fileName.c_str(), assimpImporter.GetErrorString());
        loadFailed = true;
        return;
    }

    LOG_MESSAGE("[Mesh3D] Processing %d meshes in file...", scene->mNumMeshes);
    meshes.resize(scene->mNumMeshes);

    sourceFile = fileName;

    std::vector<MaterialEntryData> materialEntries;
    AssimpInitMaterials(scene, &materialEntries);
    ProcessNodes(scene, scene->mRootNode);

    auto modelData = std::make_shared<ModelData>();
    modelData->sourceFile = fileName;
    modelData->materials = std::move(materialEntries);

    for (size_t i = 0; i < meshes.size(); i++) {
        MeshEntryData entry;
        const auto& src = meshes[i];

        entry.vertices = src.vertices;
        entry.uvs = src.uvs;
        entry.normals = src.normals;
        entry.materialIndex = src.materialIndex;
        entry.name = src.name;

        for (Triangle* tri : src.modelTriangles) {
            entry.triangleVertices.push_back(tri->A);
            entry.triangleVertices.push_back(tri->B);
            entry.triangleVertices.push_back(tri->C);
        }

        modelData->meshes.push_back(std::move(entry));
    }

    modelDataCache.store(fileName, modelData);
    sharedModel = modelData;
    sharedStaticGeometry = true;

    LOG_MESSAGE("[Mesh3D] Stored '%s' in ModelDataCache (%zu meshes, %zu materials)",
        fileName.c_str(), modelData->meshes.size(), modelData->materials.size());

    UpdateBoundingBox();
    loaded = true;
}

void Mesh3D::AssimpInitMaterials(const aiScene *pScene, std::vector<MaterialEntryData>* outMaterialEntries)
{
    LOG_MESSAGE("[Mesh3D] Prepare to load %d materials", pScene->mNumMaterials);

    for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {

        aiMaterial *pMaterial = pScene->mMaterials[i];
        LOG_MESSAGE("[Mesh3D] Loading material: %s", pMaterial->GetName().C_Str());

        aiString Path;
        if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {
            std::string p(Path.data);
            LOG_MESSAGE("[Mesh3D] Material texture path: '%s'", p.c_str());

            std::string base_filename = p.substr(p.find_last_of("/\\") + 1);

            std::replace(p.begin(), p.end(), '\\', '/');
            if (p.substr(0, 2) == "./") p = p.substr(2);

            std::string fbxDir = std::string(sourceFile.c_str());
            fbxDir = fbxDir.substr(0, fbxDir.find_last_of("/\\") + 1);
            std::string relativePath = fbxDir + p;

            // Returns path with .png extension if the original doesn't exist but the .png variant does
            auto tryPng = [](const std::string& path) -> std::string {
                if (Tools::FileExists(path.c_str())) return path;
                auto dot = path.find_last_of('.');
                if (dot != std::string::npos) {
                    std::string png = path.substr(0, dot) + ".png";
                    if (Tools::FileExists(png.c_str())) return png;
                }
                return "";
            };

            std::string textureSubPath = Config::get()->TEXTURES_FOLDER + p;
            std::string baseSubPath    = Config::get()->TEXTURES_FOLDER + base_filename;
            const std::string chessPath = Config::get()->TEXTURES_FOLDER + "chessboard.png";

            std::string FullPath;
            std::string candidate;
            bool isAbsolute = p.size() >= 2 && p[1] == ':';
            if (isAbsolute && !(candidate = tryPng(p)).empty()) {
                FullPath = candidate;
            } else if (!(candidate = tryPng(relativePath)).empty()) {
                FullPath = candidate;
            } else if (!(candidate = tryPng(textureSubPath)).empty()) {
                FullPath = candidate;
            } else if (!(candidate = tryPng(baseSubPath)).empty()) {
                FullPath = candidate;
            } else {
                LOG_MESSAGE("[Mesh3D] Material[%d] texture not found '%s', using chessboard", i, base_filename.c_str());
                if (outMaterialEntries) outMaterialEntries->push_back({false, "", false, ""});
                auto *fallback = imageCache.getOrLoad(chessPath);
                modelTextures.push_back(fallback);
                modelSpecularTextures.push_back(fallback);
                continue;
            }

            LOG_MESSAGE("[Mesh3D] Loading '%s' as texture for mesh: %s", FullPath.c_str(), getName().c_str());

            if (outMaterialEntries) {
                outMaterialEntries->push_back({true, FullPath, true, FullPath});
            }

            modelTextures.push_back(imageCache.getOrLoad(FullPath));
            modelSpecularTextures.push_back(imageCache.getOrLoad(FullPath));
        } else {
            const std::string chessPath = Config::get()->TEXTURES_FOLDER + "chessboard.png";
            LOG_MESSAGE("[Mesh3D] Material[%d] has no diffuse texture, using chessboard", i);
            if (outMaterialEntries) {
                outMaterialEntries->push_back({false, "", false, ""});
            }

            auto *fallback = imageCache.getOrLoad(chessPath);
            modelTextures.push_back(fallback);
            modelSpecularTextures.push_back(fallback);
        }
    }

    sharedTextures = true;
}

void Mesh3D::ProcessNodes(const aiScene *scene, const aiNode *node)
{
    std::string nodeName = node->mName.C_Str();
    unsigned int numMeshes = node->mNumMeshes;

    for (unsigned int x = 0; x < numMeshes; x++) {
        int idMesh = (int) node->mMeshes[x];
        this->LoadMesh(idMesh, scene->mMeshes[idMesh], nodeName);
    }

    for (unsigned int j = 0; j < node->mNumChildren; j++) {
        ProcessNodes(scene, node->mChildren[j]);
    }
}

void Mesh3D::LoadMesh(int meshId, const aiMesh *mesh, const std::string &nodeName)
{
    LOG_MESSAGE("[Mesh3D] Loading mesh: %d |  Vertices: %d", meshId, mesh->mNumVertices);

    if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
        LOG_MESSAGE("Skip mesh non triangle: %s", mesh->mPrimitiveTypes);
        return;
    }

    meshes[meshId].materialIndex = (int) mesh->mMaterialIndex;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    std::vector<Vertex3D> localMeshVertices(mesh->mNumVertices);
    for (unsigned int j = 0; j < mesh->mNumVertices; j++) {

        aiVector3t vf = mesh->mVertices[j];

        Vertex3D v(vf.x, vf.y, vf.z);

        const aiVector3D *pTexCoord = mesh->HasTextureCoords(0) ? &mesh->mTextureCoords[0][j] : &Zero3D;
        v.u = pTexCoord->x;
        v.v = pTexCoord->y;

        localMeshVertices[j] = v;
        meshes[meshId].vertices.emplace_back(v.toGLM4());
        meshes[meshId].uvs.emplace_back(v.u, v.v);

        const aiVector3D *pNormal = mesh->mNormals ? &mesh->mNormals[j] : &Zero3D;
        meshes[meshId].normals.emplace_back(pNormal->x, pNormal->y, pNormal->z);
    }

    for (unsigned int k = 0; k < mesh->mNumFaces; k++) {
        const aiFace &Face = mesh->mFaces[k];

        if (Face.mNumIndices < 3) continue;

        Vertex3D V1 = localMeshVertices.at(Face.mIndices[0]);
        Vertex3D V2 = localMeshVertices.at(Face.mIndices[1]);
        Vertex3D V3 = localMeshVertices.at(Face.mIndices[2]);

        meshes[meshId].modelTriangles.push_back(new Triangle(V3, V2, V1, this));
    }

    meshes[meshId].name = nodeName.empty() ? mesh->mName.C_Str() : nodeName;
    unsigned int sid = Brakeza::getNextUniqueObjectId();
    meshes[meshId].submeshPickingId = sid;
    meshes[meshId].submeshPickingColor = Color::idToColor(sid);

    float maxX = -FLT_MAX, minX = FLT_MAX, maxY = -FLT_MAX, minY = FLT_MAX, maxZ = -FLT_MAX, minZ = FLT_MAX;
    for (const auto& v : meshes[meshId].vertices) {
        maxX = std::max(maxX, v.x); minX = std::min(minX, v.x);
        maxY = std::max(maxY, v.y); minY = std::min(minY, v.y);
        maxZ = std::max(maxZ, v.z); minZ = std::min(minZ, v.z);
    }
    meshes[meshId].localAabb.max = Vertex3D(maxX, maxY, maxZ);
    meshes[meshId].localAabb.min = Vertex3D(minX, minY, minZ);
    meshes[meshId].localAabb.updateVertices();
}

void Mesh3D::updateSubmeshFrustumVisibility()
{
    glm::mat4 model = getModelMatrix();
    for (auto& m : meshes) {
        AABB3D worldAabb;
        for (int i = 0; i < 8; i++) {
            glm::vec4 wp = model * glm::vec4(m.localAabb.vertices[i].toGLM(), 1.0f);
            worldAabb.vertices[i] = Vertex3D(wp.x / wp.w, wp.y / wp.w, wp.z / wp.w);
        }
        m.visibleInFrustum = Frustum::isAABBVisibleInFrustum(&worldAabb);
    }
}

void Mesh3D::onUpdate()
{
    Object3D::onUpdate();

    if (isRemoved()) return;

    auto render = Components::get()->Render();
    auto window = Components::get()->Window();

    // Inicializar shader chain solo si hay custom shaders que la van a usar.
    // SIN esta condición, cada Mesh3D alloca ~91 MB de VRAM (2 FBOs full-screen)
    // aunque nunca tenga shaders → GPU OOM/TDR con 50+ objetos en escena.
    if (shaderChain && !shaderChain->isInitialized() && !customShaders.empty()) {
        shaderChain->Initialize(window->getWidthRender(), window->getHeightRender());
    }

    auto sceneFramebuffer = window->getSceneFramebuffer();

    // ShaderOGLOutline toca GL_BLEND/DEPTH_TEST/CULL_FACE por su cuenta (técnica de stencil
    // multi-pasada) sin pasar por la caché compartida de Fase 1.1.1 (Deferred/Forward, más abajo
    // en este mismo bucle por objeto) -- invalidarla tras cada uso para que el próximo Apply* no
    // se fíe de un valor obsoleto.
    if (isGUISelected() && !Components::get()->Scripting()->isExecuting()) {
        render->getShaders()->shaderOGLOutline->drawOutline(this, Color::green(), 0.1f, window->getForegroundFramebuffer());
        render->InvalidateRenderStateCache();
    }

    if (render->getLastRightClickedObject() == this && !isGUISelected()) {
        render->getShaders()->shaderOGLOutline->drawOutline(this, Color(1.0f, 0.5f, 0.0f, 1.0f), 0.1f, window->getForegroundFramebuffer());
        render->InvalidateRenderStateCache();
    }

    if (highlighted) {
        render->getShaders()->shaderOGLOutline->drawOutline(this, Color(1.0f, 0.5f, 0.0f, 1.0f), 0.1f, window->getForegroundFramebuffer());
        render->InvalidateRenderStateCache();
    }

    if (frustumCullSubmeshes) updateSubmeshFrustumVisibility();

    GLuint fbo = Config::get()->ENABLE_LIGHTS ? window->getGBuffer().FBO : sceneFramebuffer;

    if (Config::get()->TRIANGLE_MODE_TEXTURIZED ) {
        if (!isTransparent() ) {
            if (Config::get()->ENABLE_LIGHTS && enableLights) {
                // Fase 3 (Etapa 1): encolar en vez de dibujar al instante -- ComponentRender::
                // FlushOpaqueQueue() dibuja todo ordenado al final de la Pasada 2. Mismos filtros/
                // datos que antes (ver ComponentRender::EnqueueOpaque), solo cambia el cuándo.
                if (isRenderPipelineDefault())
                    render->EnqueueOpaque(this, false, fbo);
            } else {
                render->getShaders()->shaderOGLRender->renderMesh(this, false, fbo);
            }
        }
    }

    // Wireframe/Shading (debug) tocan GL_BLEND directamente sin pasar por la caché -- invalidar
    // tras usarlos, igual que con Outline. Points solo toca GL_POINT_SPRITE/
    // GL_VERTEX_PROGRAM_POINT_SIZE (confirmado), no hace falta.
    if (Config::get()->TRIANGLE_MODE_WIREFRAME) {
        render->getShaders()->shaderOGLWireframe->renderMesh(this, false, Color::gray(), fbo);
        render->InvalidateRenderStateCache();
    }

    if (Config::get()->TRIANGLE_MODE_PIXELS) {
        render->getShaders()->shaderOGLPoints->renderMesh(this, false, fbo);
    }

    if (Config::get()->TRIANGLE_MODE_SHADING) {
        render->getShaders()->shaderOGLShading->renderMesh(this, false, fbo);
        render->InvalidateRenderStateCache();
    }

    if (Config::get()->DRAW_MESH3D_AABB) {
        UpdateBoundingBox();
        Drawable::drawAABB(&aabb, Color::white());
    }

    if (Config::get()->DRAW_MESH3D_OCTREE && this->octree != nullptr) {
        Drawable::drawOctree(octree);
    }

    if (Config::get()->DRAW_MESH3D_GRID && this->grid != nullptr) {
        Drawable::drawGrid3D(grid);
    }

    if (Config::get()->MOUSE_CLICK_SELECT_OBJECT3D) {
        // Fase 4b: encolado igual que el G-Buffer (EnqueueOpaque más arriba) -- se dibuja todo
        // junto en FlushPickingQueue(), agrupando instancias del mismo modelo en una sola
        // llamada instanciada en vez de un draw por objeto cada frame.
        render->EnqueuePicking(this, false, window->getPickingColorFramebuffer().FBO);
    }

    // Encolado, NO se dibuja aquí -- ver ComponentRender::FlushObjectShaderQueue() y el comentario
    // en EnqueueObjectShaders sobre por qué debe ir después del flush del pase opaco.
    if (!customShaders.empty()) {
        render->EnqueueObjectShaders(this);
    }
}

void Mesh3D::postUpdate()
{
    Object3D::postUpdate();

    auto render = Components::get()->Render();
    auto window = Components::get()->Window();
    if (Config::get()->TRIANGLE_MODE_TEXTURIZED) {
        if (isTransparent()) {
            render->getShaders()->shaderOGLRender->renderMesh(this, false, window->getSceneFramebuffer());
        }
    }
}

void Mesh3D::RunObjectShaders() const
{
    if (customShaders.empty() || !shaderChain) return;
    if (!isVisibleInFrustum()) return;

    auto window = Components::get()->Window();
    shaderChain->ProcessChain(this, customShaders, window->getGBuffer().FBO);
    // ShaderOGLCustomCodeMesh3D::renderMesh() toca GL_DEPTH_TEST/GL_BLEND/GL_CULL_FACE directo,
    // fuera de la caché de Fase 1.1.1 -- invalidar para que el próximo Apply* (FlushOpaqueQueue)
    // no se fíe de un valor obsoleto, igual que ya hacen Outline/Wireframe/Shading/Color.
    Components::get()->Render()->InvalidateRenderStateCache();
}

void Mesh3D::BuildOctree(int depth)
{
    LOG_MESSAGE("Building Octree for %s", getName().c_str());

    UpdateBoundingBox();

    delete octree;

    octree = new Octree(aabb, depth);
}

void Mesh3D::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    Mesh3DGUI::DrawPropertiesGUI(this);
}

void Mesh3D::makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    LOG_MESSAGE("[Mesh3D] makeRigidBodyFromTriangleMesh for %s", getName().c_str());

    setMass(mass);

    // Posición Y rotación (sin escala -- las triangulaciones de abajo ya vienen escaladas a mano
    // o son mesh local-space consumido con la malla ya orientada). Antes esto solo ponía la
    // posición con setIdentity() en la rotación, ignorándola por completo: el collider quedaba
    // sin rotar aunque la malla visual sí lo estuviera (p.ej. "city", con el offset de import
    // FBX habitual) -- el bug de "los colisionables no están bien rotados".
    btTransform transformation = Tools::GLMMatrixToBulletTransformNoScale(getModelMatrix());

    btVector3 inertia(0, 0, 0);

    auto shape = getTriangleMeshFromMesh3D(inertia);
    btTriangleInfoMap* triangleInfoMap = new btTriangleInfoMap();
    btGenerateInternalEdgeInfo((btBvhTriangleMeshShape*)shape, triangleInfoMap);

    ((btBvhTriangleMeshShape*)shape)->setTriangleInfoMap(triangleInfoMap);

    btRigidBody::btRigidBodyConstructionInfo info(
        mass,
        new btDefaultMotionState(transformation),
        shape,
        inertia
    );


    body = new btRigidBody(info);
    body->activate(true);
    body->setContactProcessingThreshold(BT_LARGE_FLOAT);
    body->setUserPointer(this);
    body->setUserIndex(Config::CollisionSource::OBJECT_COLLIDER);
    body->setAngularFactor(angularFactor.toBullet());
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    if (mass <= 0) {
        body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
    }

    world->addRigidBody(body, collisionGroup, collisionMask);
}

void Mesh3D::makeRigidBodyFromTriangleMeshFromConvexHull(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    LOG_MESSAGE("[Mesh3D] makeRigidBodyFromTriangleMeshFromConvexHull for %s", getName().c_str());

    setMass(mass);

    // Posición Y rotación (sin escala -- las triangulaciones de abajo ya vienen escaladas a mano
    // o son mesh local-space consumido con la malla ya orientada). Antes esto solo ponía la
    // posición con setIdentity() en la rotación, ignorándola por completo: el collider quedaba
    // sin rotar aunque la malla visual sí lo estuviera (p.ej. "city", con el offset de import
    // FBX habitual) -- el bug de "los colisionables no están bien rotados".
    btTransform transformation = Tools::GLMMatrixToBulletTransformNoScale(getModelMatrix());

    btVector3 inertia(0, 0, 0);
    btCollisionShape* shape = getConvexHullShapeFromMesh(inertia);
    if (mass > 0) {
        shape->calculateLocalInertia(mass, inertia); // Calcula el tensor de inercia
    }

    btRigidBody::btRigidBodyConstructionInfo info(
        mass,
        new btDefaultMotionState(transformation),
        shape,
        inertia
    );

    body = new btRigidBody(info);
    body->activate(true);
    body->setContactProcessingThreshold(BT_LARGE_FLOAT);
    body->setUserPointer(this);
    body->setUserIndex(Config::CollisionSource::OBJECT_COLLIDER);
    body->setRestitution(restitution);
    body->setActivationState(ACTIVE_TAG);
    body->setLinearFactor(linearFactor.toBullet());
    body->setAngularFactor(angularFactor.toBullet());
    body->setFriction(friction);
    body->setDamping(linearDamping, angularDamping);
    body->setCcdMotionThreshold(ccdMotionThreshold);
    body->setCcdSweptSphereRadius(ccdSweptSphereRadius);

    world->addRigidBody(body, collisionGroup, collisionMask);
}

btRigidBody* Mesh3D::BuildRigidBodyFromTriangleMeshOnly(float mass)
{
    setMass(mass);

    // Posición Y rotación (sin escala -- las triangulaciones de abajo ya vienen escaladas a mano
    // o son mesh local-space consumido con la malla ya orientada). Antes esto solo ponía la
    // posición con setIdentity() en la rotación, ignorándola por completo: el collider quedaba
    // sin rotar aunque la malla visual sí lo estuviera (p.ej. "city", con el offset de import
    // FBX habitual) -- el bug de "los colisionables no están bien rotados".
    btTransform transformation = Tools::GLMMatrixToBulletTransformNoScale(getModelMatrix());

    btVector3 inertia(0, 0, 0);

    auto shape = getTriangleMeshFromMesh3D(inertia);
    btTriangleInfoMap* triangleInfoMap = new btTriangleInfoMap();
    btGenerateInternalEdgeInfo((btBvhTriangleMeshShape*)shape, triangleInfoMap);
    ((btBvhTriangleMeshShape*)shape)->setTriangleInfoMap(triangleInfoMap);

    btRigidBody::btRigidBodyConstructionInfo info(
        mass,
        new btDefaultMotionState(transformation),
        shape,
        inertia
    );

    auto* b = new btRigidBody(info);
    b->activate(true);
    b->setContactProcessingThreshold(BT_LARGE_FLOAT);
    b->setUserPointer(this);
    b->setUserIndex(Config::CollisionSource::OBJECT_COLLIDER);
    b->setAngularFactor(angularFactor.toBullet());
    b->setCollisionFlags(b->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    if (mass <= 0) {
        b->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
    }

    return b;
}

btRigidBody* Mesh3D::BuildRigidBodyFromConvexHullOnly(float mass)
{
    setMass(mass);

    // Posición Y rotación (sin escala -- las triangulaciones de abajo ya vienen escaladas a mano
    // o son mesh local-space consumido con la malla ya orientada). Antes esto solo ponía la
    // posición con setIdentity() en la rotación, ignorándola por completo: el collider quedaba
    // sin rotar aunque la malla visual sí lo estuviera (p.ej. "city", con el offset de import
    // FBX habitual) -- el bug de "los colisionables no están bien rotados".
    btTransform transformation = Tools::GLMMatrixToBulletTransformNoScale(getModelMatrix());

    btVector3 inertia(0, 0, 0);
    btCollisionShape* shape = getConvexHullShapeFromMesh(inertia);
    if (mass > 0) {
        shape->calculateLocalInertia(mass, inertia);
    }

    btRigidBody::btRigidBodyConstructionInfo info(
        mass,
        new btDefaultMotionState(transformation),
        shape,
        inertia
    );

    auto* b = new btRigidBody(info);
    b->activate(true);
    b->setContactProcessingThreshold(BT_LARGE_FLOAT);
    b->setUserPointer(this);
    b->setUserIndex(Config::CollisionSource::OBJECT_COLLIDER);
    b->setRestitution(restitution);
    b->setActivationState(ACTIVE_TAG);
    b->setLinearFactor(linearFactor.toBullet());
    b->setAngularFactor(angularFactor.toBullet());
    b->setFriction(friction);
    b->setDamping(linearDamping, angularDamping);
    b->setCcdMotionThreshold(ccdMotionThreshold);
    b->setCcdSweptSphereRadius(ccdSweptSphereRadius);

    return b;
}

void Mesh3D::makeGhostBody(btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    auto *convexHullShape = new btConvexHullShape();

    UpdateBoundingBox();
    for (auto &m: meshes) {
        for (auto & modelTriangle : m.modelTriangles) {
            btVector3 a, b, c;
            a = btVector3(modelTriangle->A.x, modelTriangle->A.y, modelTriangle->A.z);
            b = btVector3(modelTriangle->B.x, modelTriangle->B.y, modelTriangle->B.z);
            c = btVector3(modelTriangle->C.x, modelTriangle->C.y, modelTriangle->C.z);
            convexHullShape->addPoint(a, false);
            convexHullShape->addPoint(b, false);
            convexHullShape->addPoint(c, false);
        }
    }
    convexHullShape->recalcLocalAabb();

    ghostObject = new btPairCachingGhostObject();
    ghostObject->setWorldTransform(Tools::GLMMatrixToBulletTransform(getModelMatrix()));
    ghostObject->setCollisionShape(convexHullShape);
    ghostObject->setUserPointer(this);
    ghostObject->setUserIndex(Config::CollisionSource::OBJECT_COLLIDER);
    ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

    world->addCollisionObject(ghostObject, collisionGroup, collisionMask);
}

void Mesh3D::SetupGhostCollider(CollisionShape modeShape)
{
    LOG_MESSAGE("[Mesh3D] setupGhostCollider for %s", getName().c_str());

    RemoveCollisionObject();

    setCollisionMode(GHOST);
    setCollisionShape(modeShape);

    if (getCollisionShape() == SIMPLE_SHAPE || getCollisionShape() == CAPSULE_SHAPE) {
        makeSimpleGhostBody(
            getPosition(),
            getModelMatrix(),
            simpleShapeSize,
            Brakeza::get()->getComponentsManager()->Collisions()->getDynamicsWorld(),
            collisionGroup,
            collisionMask
        );
    }

    if (getCollisionShape() == TRIANGLE_MESH_SHAPE) {
        makeGhostBody(
            Brakeza::get()->getComponentsManager()->Collisions()->getDynamicsWorld(),
            collisionGroup,
            collisionMask
        );
    }
}

void Mesh3D::SetupRigidBodyCollider(CollisionShape modeShape)
{
    std::lock_guard<std::mutex> lock(mtx);

    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobMakeRigidBody>(this, modeShape));
}

void Mesh3D::DrawImGuiCollisionShapeSelector()
{
    auto flags = ImGuiComboFlags_None;
    const char* items[] = { "SIMPLE", "CAPSULE_SHAPE", "TRIANGLE",  };
    int item_current_idx = collisionShape;
    const char* combo_preview_value = items[item_current_idx];

    auto comboTitle = "Shape##" + getName();
    if (ImGui::BeginCombo(comboTitle.c_str(), combo_preview_value, flags)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = item_current_idx == n;
            if (ImGui::Selectable(items[n], is_selected)) {
                if (!is_selected) {
                    item_current_idx = n;
                    switch (n) {
                        case 0: {
                            if (collisionMode == GHOST) {
                                SetupGhostCollider(SIMPLE_SHAPE);
                            }

                            if (collisionMode == BODY) {
                                SetupRigidBodyCollider(SIMPLE_SHAPE);
                            }

                            break;
                        }
                        case 1: {
                            if (collisionMode == GHOST) {
                                SetupGhostCollider(CAPSULE_SHAPE);
                            }

                            if (collisionMode == BODY) {
                                SetupRigidBodyCollider(CAPSULE_SHAPE);
                            }
                            break;
                        }
                        case 2: {
                            if (collisionMode == GHOST) {
                                SetupGhostCollider(TRIANGLE_MESH_SHAPE);
                            }

                            if (collisionMode == BODY) {
                                SetupRigidBodyCollider(TRIANGLE_MESH_SHAPE);
                            }

                            break;
                        }
                        default: {
                            LOG_MESSAGE("[Mesh3D] Error: Cannot process this shader type!");
                        }
                    }
                }
            }
        }
        ImGui::EndCombo();
    }

    if (getCollisionShape() == SIMPLE_SHAPE) {
        if (ImGui::Button("Capture from AABB")) {
            UpdateBoundingBox();
            simpleShapeSize = aabb.size().getScaled(0.5f);
            UpdateShapeCollider();
        }
    }
}

void Mesh3D::BuildGrid3D(int sizeX, int sizeY, int sizeZ)
{
    UpdateBoundingBox();

    delete grid;

    grid = new Grid3D(aabb, sizeX, sizeY, sizeZ);
}

void Mesh3D::FillGrid3DFromGeometry()
{
    grid->Reset(grid->getNumberCubesX(), grid->getNumberCubesY(), grid->getNumberCubesZ());
    for (auto &m: meshes) {
        grid->doTestForNonEmptyGeometry(m.modelTriangles);
    }
}

void Mesh3D::AddCustomShader(ShaderBaseCustom *s)
{
    customShaders.emplace_back(s);
}

void Mesh3D::LoadShader(const FilePath::ShaderConfigFile &jsonFilename)
{
    auto metaInfo = ShadersGUI::ExtractShaderCustomCodeMetainfo(jsonFilename);

    if (ShaderBaseCustom::getShaderTypeFromString(metaInfo.type) == SHADER_OBJECT ||
    ShaderBaseCustom::getShaderTypeFromString(metaInfo.type) == SHADER_NODE_OBJECT
    ) {
        auto shader = ComponentRender::CreateCustomShaderFromDisk(metaInfo, this);

        if (shader != nullptr) {
            AddCustomShader(shader);
            return;
        }
    }

    LOG_ERROR("[Mesh3D] Error: Cannot apply shader to Mesh3D...");
}

void Mesh3D::RemoveShader(int index)
{
    if (index >= 0 && index < (int)customShaders.size()) {
        customShaders.erase(customShaders.begin() + index);
    }
}

void Mesh3D::MoveShaderUp(ShaderBaseCustom* shader)
{
    if (!shader || customShaders.size() < 2) return;

    auto it = std::find(customShaders.begin(), customShaders.end(), shader);
    if (it == customShaders.end() || it == customShaders.begin()) return;

    std::iter_swap(it, it - 1);
}

void Mesh3D::MoveShaderDown(ShaderBaseCustom* shader)
{
    if (!shader || customShaders.size() < 2) return;

    auto it = std::find(customShaders.begin(), customShaders.end(), shader);
    if (it == customShaders.end() || it == customShaders.end() - 1) return;

    std::iter_swap(it, it + 1);
}

void Mesh3D::ShadowMappingPass()
{
    auto render = Components::get()->Render();
    auto shaderShadowPass = render->getShaders()->shaderShadowPass;
    auto shaderRender = render->getShaders()->shaderOGLRender;

    // Directional Light
    shaderShadowPass->renderMeshIntoDirectionalLightTexture(this, false, shaderRender->getDirectionalLight());

    // SpotLights
    const auto shadowSpotLights = shaderRender->getShadowMappingSpotLights();

    for (unsigned int i = 0; i < shadowSpotLights.size() ; i++) {
        shaderShadowPass->renderMeshIntoArrayTextures(this, false, shadowSpotLights[i], i);
    }
}

void Mesh3D::UpdateBoundingBox()
{
    glm::mat4 mvpMatrix = getModelMatrix();

    float maxX = -FLT_MAX, minX = FLT_MAX, maxY = -FLT_MAX, minY = FLT_MAX, maxZ = -FLT_MAX, minZ = FLT_MAX;

    for (auto &m: meshes) {
        for (auto &vertex : m.vertices) {
            glm::vec4 transformedVertex = mvpMatrix * vertex;
            transformedVertex /= transformedVertex.w;

            maxX = std::max(maxX, transformedVertex.x);
            minX = std::min(minX, transformedVertex.x);
            maxY = std::max(maxY, transformedVertex.y);
            minY = std::min(minY, transformedVertex.y);
            maxZ = std::max(maxZ, transformedVertex.z);
            minZ = std::min(minZ, transformedVertex.z);
        }
    }

    this->aabb.max.x = maxX;
    this->aabb.max.y = maxY;
    this->aabb.max.z = maxZ;

    this->aabb.min.x = minX;
    this->aabb.min.y = minY;
    this->aabb.min.z = minZ;

    this->aabb.updateVertices();

    // Bounding sphere: max distance from object position to any AABB corner
    const Vertex3D& pos = getPosition();
    float maxR2 = 0.f;
    for (const auto& v : this->aabb.vertices) {
        float dx = v.x - pos.x, dy = v.y - pos.y, dz = v.z - pos.z;
        float r2 = dx*dx + dy*dy + dz*dz;
        if (r2 > maxR2) maxR2 = r2;
    }
    boundingRadius = sqrtf(maxR2);
}

btBvhTriangleMeshShape *Mesh3D::getTriangleMeshFromMesh3D(btVector3 inertia) const
{
    auto *triangleMesh = new btTriangleMesh();
    Vertex3D sv = getScaleV();

    for (auto &m: meshes) {
        for (auto & modelTriangle : m.modelTriangles) {
            btVector3 a(modelTriangle->A.x * sv.x, modelTriangle->A.y * sv.y, modelTriangle->A.z * sv.z);
            btVector3 b(modelTriangle->B.x * sv.x, modelTriangle->B.y * sv.y, modelTriangle->B.z * sv.z);
            btVector3 c(modelTriangle->C.x * sv.x, modelTriangle->C.y * sv.y, modelTriangle->C.z * sv.z);
            triangleMesh->addTriangle(a, b, c, false);
        }
    }

    auto *shape = new btBvhTriangleMeshShape(triangleMesh, true, true);
    if (mass > 0) {
        shape->calculateLocalInertia(mass, inertia);
    }

    return shape;
}

btConvexHullShape *Mesh3D::getConvexHullShapeFromMesh(btVector3 inertia)
{
    LOG_MESSAGE("[Mesh3D] Creating btConvexHullShape for object '%s' | Nº meshes: %d", getName().c_str(), (int) meshes.size());
    auto *convexHull = new btConvexHullShape();
    for (auto &m: meshes) {
        for (auto &modelTriangle: m.modelTriangles) {
            btVector3 a = modelTriangle->A.toBullet();
            btVector3 b = modelTriangle->B.toBullet();
            btVector3 c = modelTriangle->C.toBullet();
            convexHull->addPoint(a);
            convexHull->addPoint(b);
            convexHull->addPoint(c);
        }
    }

    if (mass > 0) {
        convexHull->calculateLocalInertia(mass, inertia);
    }

    return convexHull;
}

void Mesh3D::FillOGLBuffers()
{
    LOG_MESSAGE("[Mesh3D] Filling buffers...");

    if (!sharedModel) {
        // Sin ModelData compartido (carga fallida o instancia no venida de fichero
        // cacheado): comportamiento antiguo, buffers propios de esta instancia.
        ComponentRender::FillOGLBuffers(meshes);
        RegisterSubmeshPicking();
        return;
    }

    // Fase 2.1: geometria compartida entre todas las instancias del mismo fichero.
    for (size_t i = 0; i < meshes.size() && i < sharedModel->meshes.size(); i++) {
        auto &m = meshes[i];
        auto &shared = sharedModel->meshes[i];

        if (m.vertices.empty() || m.uvs.empty() || m.normals.empty()) {
            LOG_ERROR("[Mesh3D] FillOGLBuffers: mesh with empty geometry (vertices=%zu uvs=%zu normals=%zu) — skipped",
                m.vertices.size(), m.uvs.size(), m.normals.size());
            continue;
        }

        if (!glIsBuffer(shared.vertexBuffer)) {
            // Primera instancia que carga este modelo: sube la geometria compartida una vez,
            // deduplicada con EBO (Fase 2.2).
            ComponentRender::BuildDedupedStaticGeometry(
                shared.vertices, shared.normals, shared.uvs,
                shared.vertexBuffer, shared.uvBuffer, shared.normalBuffer,
                shared.indexBuffer, shared.indexCount
            );
        }

        m.vertexBuffer = shared.vertexBuffer;
        m.uvBuffer = shared.uvBuffer;
        m.normalBuffer = shared.normalBuffer;
        m.indexBuffer = shared.indexBuffer;
        m.indexCount = shared.indexCount;
    }

    RegisterSubmeshPicking();
}

void Mesh3D::RegisterSubmeshPicking()
{
    auto render = Components::get()->Render();
    for (const auto &m : meshes) {
        if (m.submeshPickingId != 0) {
            render->registerSubmesh(m.submeshPickingId, this, m.name);
        }
    }
}

void Mesh3D::UnregisterSubmeshPicking()
{
    Components::get()->Render()->unregisterSubmeshes(this);
}

void Mesh3D::setSourceFile(const FilePath::ModelFile &sourceFile)
{
    Mesh3D::sourceFile = sourceFile;
}

void Mesh3D::setRenderPipelineDefault(bool value)
{
    renderDefaultPipeline = value;
}

void Mesh3D::InitializeShaderChain(int screenWidth, int screenHeight)
{
    if (shaderChain) {
        shaderChain->Initialize(screenWidth, screenHeight);
    }
}

void Mesh3D::ProcessShaderChain(GLuint finalFBO)
{
    if (shaderChain && !customShaders.empty() && isVisibleInFrustum()) {
        shaderChain->ProcessChain(this, customShaders, finalFBO);
    }
}

void Mesh3D::CleanupShaderChain()
{
    if (shaderChain) {
        delete shaderChain;
        shaderChain = nullptr;
    }
}
