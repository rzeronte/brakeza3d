#define GL_GLEXT_PROTOTYPES

#include <vector>
#include <assimp/cimport.h>
#include <glm/ext/matrix_float4x4.hpp>
#include "../../include/3D/Mesh3D.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Brakeza.h"
#include "../../include/Render/Drawable.h"
#include "../../include/OpenGL/ShaderOGLCustomMesh3D.h"
#include "../../include/OpenGL/ShaderOGLShadowPass.h"
#include <assimp/postprocess.h>

#include "../../include/Components/Components.h"
#include "../../include/GUI/Objects/Mesh3DGUI.h"
#include "../../include/Render/JSONSerializerRegistry.h"

Mesh3D::Mesh3D()
{
    luaEnvironment["this"] = this;
}

Mesh3D::Mesh3D(std::string modelFile)
:
    sourceFile(modelFile)
{
}

Mesh3D::~Mesh3D()
{
    Logging::Message("[Mesh3D] Destroying '%s'...", getName().c_str());

    for (auto &m : meshes) {
        for (auto triangle : m.modelTriangles) delete triangle;
        for (auto vertex : m.modelVertices) delete vertex;

        if (glIsBuffer(m.vertexBuffer))
            glDeleteBuffers(1, &m.vertexBuffer);

        if (glIsBuffer(m.uvBuffer))
            glDeleteBuffers(1, &m.uvBuffer);

        if (glIsBuffer(m.normalBuffer))
            glDeleteBuffers(1, &m.normalBuffer);
    }

    if (!sharedTextures) {
        for (auto texture : modelTextures) delete texture;
        for (auto texture : modelSpecularTextures) delete texture;
    }
}

void Mesh3D::AssimpLoadGeometryFromFile(const std::string &fileName)
{
    Logging::Message("[Mesh3D] Loading geometry for %s...", fileName.c_str());

    if (!Tools::FileExists(fileName.c_str())) {
        Logging::Error("[Mesh3D] Error import 3D file not exist");
        return;
    }

    Assimp::Importer assimpImporter;
    const aiScene *scene = assimpImporter.ReadFile(
        fileName,
        aiProcess_Triangulate |
        aiProcess_SortByPType |
        aiProcess_FlipUVs |
        aiProcess_OptimizeMeshes
    );

    if (!scene) {
        Logging::Message("[Mesh3D] Error import 3D file for ASSIMP");
        exit(-1);
    }

    Logging::Message("[Mesh3D] Processing %d meshes in file...", scene->mNumMeshes);
    meshes.resize(scene->mNumMeshes);

    AssimpInitMaterials(scene);
    ProcessNodes(scene, scene->mRootNode);

    sourceFile = fileName;
    //ComponentRender::FillOGLBuffers(meshes);
}

void Mesh3D::AssimpInitMaterials(const aiScene *pScene)
{
    Logging::Message("[Mesh3D] Prepare to load %d materials", pScene->mNumMaterials);

    for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {

        aiMaterial *pMaterial = pScene->mMaterials[i];
        Logging::Message("[Mesh3D] Loading material: %s", pMaterial->GetName().C_Str());

        if (std::string(pMaterial->GetName().C_Str()) == AI_DEFAULT_MATERIAL_NAME) {
            continue;
        }
        //if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE)  >= 1) {
        aiString Path;
        if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {
            std::string p(Path.data);

            std::string base_filename = p.substr(p.find_last_of("/\\") + 1);

            if (p.substr(0, 2) == ".\\") {
                p = p.substr(2, p.size() - 2);
            }

            std::string FullPath = Config::get()->TEXTURES_FOLDER + base_filename;
            Logging::Message("[Mesh3D] Loading '%s' as texture for mesh: %s", FullPath.c_str(), getName().c_str());

            modelTextures.emplace_back(new Image(FullPath));
            modelSpecularTextures.push_back(new Image(FullPath));
        } else {
            Logging::Message("[Mesh3D] ERROR: mMaterial[%s]: Not valid color", i);
        }
        //}
    }
}

void Mesh3D::ProcessNodes(const aiScene *scene, const aiNode *node)
{
    unsigned int numMeshes = node->mNumMeshes;

    for (unsigned int x = 0; x < numMeshes; x++) {
        int idMesh = (int) node->mMeshes[x];
        this->LoadMesh(idMesh, scene->mMeshes[idMesh]);
    }

    for (unsigned int j = 0; j < node->mNumChildren; j++) {
        ProcessNodes(scene, node->mChildren[j]);
    }
}

void Mesh3D::LoadMesh(int meshId, const aiMesh *mesh)
{
    Logging::Message("[Mesh3D] Loading mesh: %d |  Vertices: %d", meshId, mesh->mNumVertices);

    if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
        Logging::Message("Skip mesh non triangle: %s", mesh->mPrimitiveTypes);
        return;
    }

    meshes[meshId].materialIndex = (int) mesh->mMaterialIndex;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    std::vector<Vertex3D> localMeshVertices(mesh->mNumVertices);
    for (unsigned int j = 0; j < mesh->mNumVertices; j++) {

        aiVector3t vf = mesh->mVertices[j];

        Vertex3D v(vf.x, vf.y, vf.z);

        const aiVector3D *pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][j]) : &Zero3D;
        v.u = pTexCoord->x;
        v.v = pTexCoord->y;

        localMeshVertices[j] = v;
        meshes[meshId].vertices.emplace_back(v.toGLM4());
        meshes[meshId].uvs.emplace_back(v.u, v.v);

        aiVector3t vn = mesh->mNormals[j];
        meshes[meshId].normals.emplace_back(vn.x, vn.y, vn.z);
    }

    for (unsigned int k = 0; k < mesh->mNumFaces; k++) {
        const aiFace &Face = mesh->mFaces[k];

        if (Face.mNumIndices < 3) continue;

        Vertex3D V1 = localMeshVertices.at(Face.mIndices[0]);
        Vertex3D V2 = localMeshVertices.at(Face.mIndices[1]);
        Vertex3D V3 = localMeshVertices.at(Face.mIndices[2]);

        meshes[meshId].modelTriangles.push_back(new Triangle(V3, V2, V1, this));
    }
}

void Mesh3D::onUpdate()
{
    Object3D::onUpdate();

    if (isRemoved()) return;

    auto render = Components::get()->Render();
    auto window = Components::get()->Window();

    auto sceneFramebuffer = window->getSceneFramebuffer();

    if (isGUISelected()) {
        render->getShaders()->shaderOGLOutline->drawOutline(this, Color::green(), 0.1f, window->getUIFramebuffer());
    }

    if (Config::get()->TRIANGLE_MODE_TEXTURIZED && isRender()) {
        if (Config::get()->ENABLE_LIGHTS && isEnableLights()) {
            render->getShaderOGLRenderDeferred()->renderMesh(this, false, window->getGBuffer().FBO);
            if (Config::get()->ENABLE_SHADOW_MAPPING) {
                ShadowMappingPass();
            }
        } else {
            render->getShaders()->shaderOGLRender->renderMesh(this, false, sceneFramebuffer);
        }
    }

    if (Config::get()->TRIANGLE_MODE_WIREFRAME && isRender()) {
        render->getShaders()->shaderOGLWireframe->renderMesh(this, false, Color::gray(), sceneFramebuffer);
    }

    if (Config::get()->TRIANGLE_MODE_PIXELS && isRender()) {
        render->getShaders()->shaderOGLPoints->renderMesh(this, false, sceneFramebuffer);
    }

    if (Config::get()->TRIANGLE_MODE_SHADING && isRender()) {
        render->getShaders()->shaderOGLShading->renderMesh(this, false, sceneFramebuffer);
    }

    if (Config::get()->DRAW_MESH3D_AABB && isRender()) {
        UpdateBoundingBox();
        Drawable::drawAABB(&aabb, Color::white());
    }

    if (Config::get()->DRAW_MESH3D_OCTREE && this->octree != nullptr) {
        Drawable::drawOctree(octree);
    }

    if (Config::get()->DRAW_MESH3D_GRID && this->grid != nullptr) {
        Drawable::drawGrid3D(grid);
    }

    if (Config::get()->MOUSE_CLICK_SELECT_OBJECT3D && isRender()) {
        render->getShaders()->shaderOGLColor->renderMesh(
            this,
            false,
            getPickingColor(),
            false,
            window->getPickingColorFramebuffer().FBO
        );
    }

    for (const auto s: customShaders) {
        s->render(sceneFramebuffer);
    }
}

void Mesh3D::postUpdate()
{
    Object3D::postUpdate();
}

void Mesh3D::BuildOctree(int depth)
{
    Logging::Message("Building Octree for %s", getName().c_str());

    UpdateBoundingBox();

    delete octree;

    octree = new Octree(aabb, depth);
}

void Mesh3D::setRender(bool render)
{
    Mesh3D::render = render;
}

void Mesh3D::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    Mesh3DGUI::DrawPropertiesGUI(this);
}

void Mesh3D::makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    Logging::Message("[Mesh3D] makeRigidBodyFromTriangleMesh for %s", getName().c_str());

    setMass(mass);

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(getPosition().toBullet());

    btVector3 inertia(0, 0, 0);

    btRigidBody::btRigidBodyConstructionInfo info(
        mass,
        new btDefaultMotionState(transformation),
        getTriangleMeshFromMesh3D(inertia),
        inertia
    );

    body = new btRigidBody(info);
    body->activate(true);
    body->setContactProcessingThreshold(BT_LARGE_FLOAT);
    body->setUserPointer(this);
    body->setUserIndex(Config::CollisionSource::OBJECT_COLLIDER);
    body->setAngularFactor(angularFactor.toBullet());

    if (mass <= 0) {
        body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
    }

    world->addRigidBody(body, collisionGroup, collisionMask);
}

void Mesh3D::makeRigidBodyFromTriangleMeshFromConvexHull(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    Logging::Message("[Mesh3D] makeRigidBodyFromTriangleMeshFromConvexHull for %s", getName().c_str());

    setMass(mass);

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(getPosition().toBullet());

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
            convexHullShape->addPoint(a);
            convexHullShape->addPoint(b);
            convexHullShape->addPoint(c);
        }
    }
    ghostObject = new btPairCachingGhostObject();
    ghostObject->setWorldTransform(Tools::GLMMatrixToBulletTransform(getModelMatrix()));
    ghostObject->setCollisionShape(convexHullShape);
    ghostObject->setUserPointer(this);
    ghostObject->setUserIndex(Config::CollisionSource::OBJECT_COLLIDER);

    world->addCollisionObject(ghostObject, collisionGroup, collisionMask);
}

void Mesh3D::SetupGhostCollider(CollisionShape modeShape)
{
    Logging::Message("[Mesh3D] setupGhostCollider for %s", getName().c_str());

    removeCollisionObject();

    setCollisionMode(GHOST);
    setCollisionShape(modeShape);

    if (getCollisionShape() == SIMPLE_SHAPE || getCollisionShape() == CAPSULE) {
        makeSimpleGhostBody(
            getPosition(),
            getModelMatrix(),
            simpleShapeSize,
            Brakeza::get()->getComponentsManager()->Collisions()->getDynamicsWorld(),
            btBroadphaseProxy::DefaultFilter,
            btBroadphaseProxy::DefaultFilter
        );
    }

    if (getCollisionShape() == TRIANGLE_MESH_SHAPE) {
        makeGhostBody(
            Brakeza::get()->getComponentsManager()->Collisions()->getDynamicsWorld(),
            btBroadphaseProxy::DefaultFilter,
            btBroadphaseProxy::DefaultFilter
        );
    }
}

void Mesh3D::SetupRigidBodyCollider(CollisionShape modeShape)
{
    Logging::Message("[Mesh3D] SetupRigidBodyCollider for %s", getName().c_str());
    removeCollisionObject();

    setCollisionShape(modeShape);
    setCollisionMode(BODY);
    if (getCollisionShape() == SIMPLE_SHAPE || getCollisionShape() == CAPSULE) {
        MakeSimpleRigidBody(
            mass,
            Brakeza::get()->getComponentsManager()->Collisions()->getDynamicsWorld(),
            btBroadphaseProxy::DefaultFilter,
            btBroadphaseProxy::DefaultFilter
        );
    }

    if (getCollisionShape() == TRIANGLE_MESH_SHAPE) {
        if (isColliderStatic()) {
            makeRigidBodyFromTriangleMesh(
                mass,
                Brakeza::get()->getComponentsManager()->Collisions()->getDynamicsWorld(),
                Config::collisionGroups::AllFilter,
                Config::collisionGroups::AllFilter
            );
        } else {
            makeRigidBodyFromTriangleMeshFromConvexHull(
                mass,
                Brakeza::get()->getComponentsManager()->Collisions()->getDynamicsWorld(),
                Config::collisionGroups::AllFilter,
                Config::collisionGroups::AllFilter
            );
        }
    }
}

void Mesh3D::DrawImGuiCollisionShapeSelector()
{
    auto flags = ImGuiComboFlags_None;
    const char* items[] = { "SIMPLE", "CAPSULE", "TRIANGLE",  };
    int item_current_idx = collisionShape;
    const char* combo_preview_value = items[item_current_idx];

    auto comboTitle = "Shape##" + getName();
    if (ImGui::BeginCombo(comboTitle.c_str(), combo_preview_value, flags)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (item_current_idx == n);
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
                                SetupGhostCollider(CAPSULE);
                            }

                            if (collisionMode == BODY) {
                                SetupRigidBodyCollider(CAPSULE);
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
                            Logging::Message("[Mesh3D] Error: Cannot process this shader type!");
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

void Mesh3D::AddCustomShader(ShaderOGLCustom *s)
{
    customShaders.emplace_back(s);
}

void Mesh3D::LoadShader(const std::string &folder, const std::string &jsonFilename)
{
    auto name = Tools::getFilenameWithoutExtension(jsonFilename);

    std::string shaderFragmentFile = folder + std::string(name + ".fs");
    std::string shaderVertexFile = folder + std::string(name + ".vs");

    auto type = ShaderOGLCustom::extractTypeFromShaderName(folder, name);

    Logging::Message("LoadShaderInto Scene: Folder: %s, Name: %s, Type: %d", folder.c_str(), name.c_str(), type);

    switch(type) {
        case SHADER_POSTPROCESSING : {
            Logging::Error("[Mesh3D] You can't add a 'Postprocessing shader' type into Mesh3D");
            break;
        }
        case SHADER_OBJECT : {
            AddCustomShader(new ShaderOGLCustomMesh3D(this, name, shaderVertexFile, shaderFragmentFile));
            break;
        }
        default: {
            Logging::Error("[Mesh3D] Cannot process this shader type!");
        }
    }
}

void Mesh3D::RemoveShader(int index)
{
    if (index >= 0 && index < customShaders.size()) {
        customShaders.erase(customShaders.begin() + index);
    }
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
    const auto numSpotLights = static_cast<int>(shadowSpotLights.size());

    for (int i = 0; i < numSpotLights; i++) {
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
}

btBvhTriangleMeshShape *Mesh3D::getTriangleMeshFromMesh3D(btVector3 inertia) const
{
    auto *triangleMesh = new btTriangleMesh();

    for (auto &m: meshes) {
        for (auto & modelTriangle : m.modelTriangles) {
            btVector3 a = modelTriangle->A.toBullet();
            btVector3 b = modelTriangle->B.toBullet();
            btVector3 c = modelTriangle->C.toBullet();
            triangleMesh->addTriangle(a, b, c, false);
        }
    }

    auto s = new btBvhTriangleMeshShape(triangleMesh, true, true);
    if (mass > 0) {
        s->calculateLocalInertia(mass, inertia);
    }

    return s;
}

btConvexHullShape *Mesh3D::getConvexHullShapeFromMesh(btVector3 inertia) const
{
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
    Logging::Message("[Mesh3D] Filling buffers...");
    ComponentRender::FillOGLBuffers(meshes);
}

void Mesh3D::setSourceFile(const std::string &sourceFile)
{
    Mesh3D::sourceFile = sourceFile;
}