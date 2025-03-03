#define GL_GLEXT_PROTOTYPES

#include <vector>
#include <assimp/cimport.h>
#include <glm/ext/matrix_float4x4.hpp>
#include "../../include/Objects/Mesh3D.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"
#include "../../include/OpenGL/ShaderOpenGLCustomMesh3D.h"

Mesh3D::Mesh3D()
:
    octree(nullptr),
    grid(nullptr),
    sharedTextures(false),
    render(true)
{
    decal = false;
    luaEnvironment["this"] = this;
}

void Mesh3D::onUpdate()
{
    Object3D::onUpdate();

    if (isRemoved()) return;

    auto render = ComponentsManager::get()->getComponentRender();
    auto window = ComponentsManager::get()->getComponentWindow();

    if (render->getSelectedObject() == this) {
        render->getShaderOGLOutline()->drawOutline(this, Color::green(), 0.1f);
    }

    if (EngineSetup::get()->TRIANGLE_MODE_PIXELS && isRender()) {
        for (auto &m: meshes) {
            render->getShaderOGLPoints()->render(
                this,
                m.vertexbuffer,
                m.vertices.size(),
                Color::green(),
                window->getSceneFramebuffer()
            );
        }
    }

    if (EngineSetup::get()->TRIANGLE_MODE_COLOR_SOLID && isRender()) {
        for (auto &m: meshes) {
            render->getShaderOGLShading()->render(
                getModelMatrix(),
                m.vertexbuffer,
                m.uvbuffer,
                m.normalbuffer,
                (int) m.vertices.size(),
                window->getSceneFramebuffer()
            );
        }
    }

    if (EngineSetup::get()->TRIANGLE_MODE_TEXTURIZED && isRender()) {
        render->getShaderOGLRender()->renderMesh(this,window->getSceneFramebuffer());
    }

    if (EngineSetup::get()->TRIANGLE_MODE_WIREFRAME && isRender()) {
        for (auto &m: meshes) {
            render->getShaderOGLWireframe()->render(
                getModelMatrix(),
                m.vertexbuffer,
                m.uvbuffer,
                m.normalbuffer,
                (int) m.vertices.size(),
                window->getSceneFramebuffer()
            );
        }
    }

    if (EngineSetup::get()->DRAW_MESH3D_AABB && isRender()) {
        this->updateBoundingBox();
        Drawable::drawAABB(&this->aabb, Color::white());
    }

    if (EngineSetup::get()->DRAW_MESH3D_OCTREE && this->octree != nullptr) {
        Drawable::drawOctree(this->octree);
    }

    if (EngineSetup::get()->DRAW_MESH3D_GRID && this->grid != nullptr) {
        Drawable::drawGrid3D(this->grid);
    }

    for (auto &s: customShaders) {
        s->render(window->getSceneFramebuffer());
    }
}

void Mesh3D::postUpdate()
{
    Object3D::postUpdate();
}

void Mesh3D::AssimpLoadGeometryFromFile(const std::string &fileName)
{
    Logging::Message("[Mesh3D] Loading geometry for %s...", fileName.c_str());
    std::cout << "AssimpLoadGeometryFromFile: " << fileName.c_str() << std::endl;

    if (!Tools::fileExists(fileName.c_str())) {
        Logging::Message("[error] Error import 3D file not exist");
        std::cout << "Error import 3D file not exist" << std::endl;
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
        Logging::Message("Error import 3D file for ASSIMP");
        exit(-1);
    }

    Logging::Message("Num meshes: %d", scene->mNumMeshes);
    meshes.resize(scene->mNumMeshes);

    AssimpInitMaterials(scene, fileName);
    ProcessNodes(scene, scene->mRootNode);

    sourceFile = fileName;

    fillBuffers();
}

void Mesh3D::AssimpInitMaterials(const aiScene *pScene, const std::string &Filename)
{
    // Extract the directory part from the file name
    std::string::size_type SlashIndex = Filename.find_last_of('/');
    std::string Dir;

    if (SlashIndex == std::string::npos) {
        Dir = ".";
    } else if (SlashIndex == 0) {
        Dir = "/";
    } else {
        Dir = Filename.substr(0, SlashIndex);
    }

    Logging::Message("[Mesh3D]: mNumMaterials: %d", pScene->mNumMaterials);

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

            std::string FullPath = EngineSetup::get()->TEXTURES_FOLDER + base_filename;

            Logging::Message("[Mesh3D] Loading '%s' as texture for mesh: %s", FullPath.c_str(), Filename.c_str());

            this->modelTextures.push_back(new Image(FullPath));
            this->modelSpecularTextures.push_back(new Image(FullPath));
        } else {
            Logging::Message("[Mesh3D] ERROR: mMaterial[%s]: Not valid color", i);
        }
        //}
    }
}

void Mesh3D::ProcessNodes(const aiScene *scene, aiNode *node)
{
    int numMeshes = (int) node->mNumMeshes;

    for (unsigned int x = 0; x < numMeshes; x++) {
        int idMesh = (int) node->mMeshes[x];
        this->LoadMesh(idMesh, scene->mMeshes[idMesh]);
    }

    for (unsigned int j = 0; j < node->mNumChildren; j++) {
        ProcessNodes(scene, node->mChildren[j]);
    }
}

void Mesh3D::LoadMesh(int meshId, aiMesh *mesh)
{
    Logging::Message("[Mesh3D] Loading mesh: %d | Numº Vertices: %d", meshId, mesh->mNumVertices);

    if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
        Logging::Message("Skip mesh non triangle: %s", mesh->mPrimitiveTypes);
        return;
    }

    meshes[meshId].materialIndex = mesh->mMaterialIndex;

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

Octree *Mesh3D::getOctree() const {
    return octree;
}

Grid3D *Mesh3D::getGrid3D() const {
    return grid;
}

void Mesh3D::buildGrid3D(int sizeX, int sizeY, int sizeZ)
{
    updateBoundingBox();

    if (grid != nullptr) delete grid;

    grid = new Grid3D(aabb, sizeX, sizeY, sizeZ);
}

void Mesh3D::buildOctree(int depth)
{
    Logging::Message("Building Octree for %s", getLabel().c_str());

    updateBoundingBox();

    if (octree != nullptr) delete octree;

    octree = new Octree(aabb, depth);
}

Mesh3D::~Mesh3D()
{
    Logging::Message("Delete Mesh3D: %s", getLabel().c_str());

    for (auto &m : meshes) {
        for (auto triangle : m.modelTriangles) delete triangle;
        for (auto vertex : m.modelVertices) delete vertex;

        if (glIsBuffer(m.vertexbuffer))
            glDeleteBuffers(1, &m.vertexbuffer);

        if (glIsBuffer(m.uvbuffer))
            glDeleteBuffers(1, &m.uvbuffer);

        if (glIsBuffer(m.normalbuffer))
            glDeleteBuffers(1, &m.normalbuffer);
    }

    if (!sharedTextures) {
        for (auto texture : modelTextures) delete texture;
        for (auto texture : modelSpecularTextures) delete texture;
    }
}

bool Mesh3D::isRender() const
{
    return render;
}

void Mesh3D::setRender(bool render)
{
    Mesh3D::render = render;
}

std::vector<Triangle *> &Mesh3D::getModelTriangles(int i)
{
    return meshes[i].modelTriangles;
}

std::vector<Image *> &Mesh3D::getModelTextures()
{
    return modelTextures;
}

const std::vector<Image *> &Mesh3D::getModelSpecularTextures() const
{
    return modelSpecularTextures;
}

std::vector<Vertex3D *> &Mesh3D::getModelVertices(int i)
{
    return meshes[i].modelVertices;
}

AABB3D &Mesh3D::getAabb()
{
    return aabb;
}

const char *Mesh3D::getTypeObject()
{
    return "Mesh3D";
}

const char *Mesh3D::getTypeIcon()
{
    return "meshIcon";
}

Mesh3D *Mesh3D::create(Vertex3D position, const std::string& imageFile)
{
    auto o = new Mesh3D();
    o->setPosition(position);
    o->AssimpLoadGeometryFromFile(imageFile);

    return o;
}

void Mesh3D::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();
    std::string title = "Mesh3D (File: " + sourceFile + ")";

    if (ImGui::CollapsingHeader("Mesh3D")) {
        if (ImGui::TreeNode("Mesh information")) {
            auto fileModel = std::string("- File model: ") + sourceFile;
            ImGui::Text(fileModel.c_str());
            ImGui::Text("- Num Meshes: %d", (int) meshes.size());
            int cont = 1;
            for (auto &m: meshes) {
                auto meshTitle = "Mesh " + std::to_string(cont);
                if (ImGui::TreeNode(meshTitle.c_str())) {
                    ImGui::Text("Num Vertices: %d", (int) m.vertices.size());
                    ImGui::Text("Num UVs: %d", (int) m.uvs.size());
                    ImGui::Text("Num Normals: %d", (int) m.normals.size());
                    ImGui::TreePop();
                }
                cont++;
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::TreeNode("Grid3D")) {
            if (grid != nullptr) {
                if (ImGui::Button("Fill from mesh geometry")) {
                    fillGrid3DFromGeometry();
                }
                grid->drawImGuiProperties();
                ImGui::SameLine();
                if (ImGui::Button("Delete Grid3D")) {
                    delete grid;
                    grid = nullptr;
                }
            } else {
                static int sizeX = 1;
                static int sizeY = 1;
                static int sizeZ = 1;
                ImGui::SliderInt("Size X", &sizeX, 1, 10);
                ImGui::SliderInt("Size Y", &sizeY, 1, 10);
                ImGui::SliderInt("Size Z", &sizeZ, 1, 10);

                if (ImGui::Button("Create Grid3D")) {
                    buildGrid3D(sizeX, sizeY, sizeZ);
                }
            }

            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::TreeNode("Octree")) {
            if (octree == nullptr) {
                static int maxDepth = 1;
                ImGui::SliderInt("Depth", &maxDepth, 1, 4);
                if (ImGui::Button("Create octree")) {
                    buildOctree(maxDepth);
                }
            }

            if (octree != nullptr) {
                static int maxDepth = 1;
                ImGui::SliderInt("Depth", &maxDepth, 1, 4);
                if (ImGui::Button("Update octree")) {
                    buildOctree(maxDepth);
                }
                ImGui::SameLine();
                if (ImGui::Button("Delete Octree")) {
                    delete octree;
                    octree = nullptr;
                }
            }

            ImGui::TreePop();
        }
        ImGui::Separator();
        ImGui::Checkbox(std::string("Enable lights").c_str(), &enableLights);
    }
}

cJSON * Mesh3D::getJSON()
{
    cJSON *root = Object3D::getJSON();

    cJSON *effectsArrayJSON = cJSON_CreateArray();
    for ( auto s : customShaders) {
        cJSON_AddItemToArray(effectsArrayJSON, s->getTypesJSON());
    }
    cJSON_AddItemToObject(root, "shaders", effectsArrayJSON);


    cJSON_AddStringToObject(root, "model", sourceFile.c_str());
    cJSON_AddBoolToObject(root, "enableLights", isEnableLights());

    if (grid != nullptr) {
        cJSON_AddItemToObject(root, "grid", grid->getJSON());
    }

    if (octree != nullptr) {
        cJSON_AddItemToObject(root, "octree", octree->getJSON());
    }

    return root;
}

void Mesh3D::setPropertiesFromJSON(cJSON *object, Mesh3D *o, bool loadGeometry)
{
    o->setBelongToScene(true);
    Object3D::setPropertiesFromJSON(object, o);
    o->setEnableLights(cJSON_GetObjectItemCaseSensitive(object, "enableLights")->valueint);
    if (loadGeometry) {
        o->AssimpLoadGeometryFromFile(cJSON_GetObjectItemCaseSensitive(object, "model")->valuestring);
    }

    if (cJSON_GetObjectItemCaseSensitive(object, "shaders") != nullptr) {
        auto shaderTypesMapping = ComponentsManager::get()->getComponentRender()->getShaderTypesMapping();
        cJSON *currentShaderJSON;
        cJSON_ArrayForEach(currentShaderJSON, cJSON_GetObjectItemCaseSensitive(object, "shaders")) {
            auto typeString = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "type")->valuestring;
            auto type = ShaderOpenGLCustom::getShaderTypeFromString(typeString);
            switch (type) {
                case ShaderCustomTypes::SHADER_OBJECT: {
                    auto name = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "name")->valuestring;
                    auto vertex = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "vertexshader")->valuestring;
                    auto fragment = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "fragmentshader")->valuestring;
                    auto types = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "types");
                    o->addCustomShader(new ShaderOpenGLCustomMesh3D(o, name, vertex, fragment, types));
                    break;
                }
            }
        }
    }

    if (cJSON_GetObjectItemCaseSensitive(object, "isCollisionsEnabled") != nullptr) {
        bool collisionsEnabled = cJSON_GetObjectItemCaseSensitive(object, "isCollisionsEnabled")->valueint;
        cJSON *colliderJSON = cJSON_GetObjectItemCaseSensitive(object, "collider");

        if (collisionsEnabled) {
            o->setCollisionsEnabled(true);
            int mode = (int) cJSON_GetObjectItemCaseSensitive(colliderJSON, "mode")->valueint;
            int shape = (int) cJSON_GetObjectItemCaseSensitive(colliderJSON, "shape")->valueint;

            if ((cJSON_GetObjectItemCaseSensitive(colliderJSON, "colliderStatic") != nullptr)) {
                o->setColliderStatic(cJSON_GetObjectItemCaseSensitive(colliderJSON, "colliderStatic")->valueint);
            }

            switch (mode) {
                case CollisionMode::GHOST:
                    if (shape == CollisionShape::SIMPLE_SHAPE) {
                        o->setupGhostCollider(CollisionShape::SIMPLE_SHAPE);
                    }
                    if (shape == CollisionShape::CAPSULE) {
                        o->setupGhostCollider(CollisionShape::CAPSULE);
                    }

                    if (shape == CollisionShape::TRIANGLE_MESH_SHAPE) {
                        o->setupGhostCollider(CollisionShape::TRIANGLE_MESH_SHAPE);
                    }
                    break;
                case CollisionMode::BODY:
                    if (shape == CollisionShape::SIMPLE_SHAPE) {
                        o->setupRigidBodyCollider(CollisionShape::SIMPLE_SHAPE);
                    }
                    if (shape == CollisionShape::CAPSULE) {
                        o->setupRigidBodyCollider(CollisionShape::CAPSULE);
                    }
                    if (shape == CollisionShape::TRIANGLE_MESH_SHAPE) {
                        o->setupRigidBodyCollider(CollisionShape::TRIANGLE_MESH_SHAPE);
                    }
                    break;
            }
        }
    }

    if (cJSON_GetObjectItemCaseSensitive(object, "grid") != nullptr) {
        auto gridJSON = cJSON_GetObjectItemCaseSensitive(object, "grid");
        o->buildGrid3D(
            (int) cJSON_GetObjectItemCaseSensitive(gridJSON, "x")->valueint,
            (int) cJSON_GetObjectItemCaseSensitive(gridJSON, "y")->valueint,
            (int) cJSON_GetObjectItemCaseSensitive(gridJSON, "z")->valueint
        );
    }

    if (cJSON_GetObjectItemCaseSensitive(object, "octree") != nullptr) {
        auto octreeJSON = cJSON_GetObjectItemCaseSensitive(object, "octree");
        auto maxDepth = (int) cJSON_GetObjectItemCaseSensitive(octreeJSON, "maxDepth")->valueint;
        o->buildOctree(maxDepth);
    }
}

void Mesh3D::createFromJSON(cJSON *object)
{
    auto o = new Mesh3D();

    Mesh3D::setPropertiesFromJSON(object, o, true);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

void Mesh3D::fillBuffers()
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

        glGenBuffers(1, &m.feedbackBuffer);  // Creamos el buffer para Transform Feedback
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m.feedbackBuffer);  // Vinculamos el buffer de feedback
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, m.vertices.size() * sizeof(glm::vec4), &m.vertices[0], GL_DYNAMIC_COPY);  // Inicializamos el buffer vacío
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);  // Desvinculamos el buffer
    }

    loaded = true;
}

void Mesh3D::makeGhostBody(btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    auto *convexHullShape = new btConvexHullShape();

    updateBoundingBox();
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

    world->addCollisionObject(ghostObject, collisionGroup, collisionMask);
}

void Mesh3D::setupGhostCollider(CollisionShape modeShape) {
    Logging::Message("[Mesh3D] setupGhostCollider for %s", getLabel().c_str());

    removeCollisionObject();

    setCollisionMode(CollisionMode::GHOST);
    setCollisionShape(modeShape);

    if (getCollisionShape() == CollisionShape::SIMPLE_SHAPE || getCollisionShape() == CollisionShape::CAPSULE) {
        makeSimpleGhostBody(
            getPosition(),
            getModelMatrix(),
            simpleShapeSize,
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            btBroadphaseProxy::DefaultFilter,
            btBroadphaseProxy::DefaultFilter
        );
    }

    if (getCollisionShape() == CollisionShape::TRIANGLE_MESH_SHAPE) {
        makeGhostBody(
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            btBroadphaseProxy::DefaultFilter,
            btBroadphaseProxy::DefaultFilter
        );
    }
}

void Mesh3D::setupRigidBodyCollider(CollisionShape modeShape)
{
    Logging::Message("[Mesh3D] setupRigidBodyCollider for %s", getLabel().c_str());
    removeCollisionObject();

    setCollisionShape(modeShape);
    setCollisionMode(CollisionMode::BODY);
    if (getCollisionShape() == CollisionShape::SIMPLE_SHAPE || getCollisionShape() == CollisionShape::CAPSULE) {
        updateBoundingBox();
        makeSimpleRigidBody(
            mass,
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            btBroadphaseProxy::DefaultFilter,
            btBroadphaseProxy::DefaultFilter
        );
    }

    if (getCollisionShape() == CollisionShape::TRIANGLE_MESH_SHAPE) {
        if (isColliderStatic()) {
            makeRigidBodyFromTriangleMesh(
                mass,
                Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                EngineSetup::collisionGroups::AllFilter,
                EngineSetup::collisionGroups::AllFilter
            );
        } else {
            makeRigidBodyFromTriangleMeshFromConvexHull(
                mass,
                Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                EngineSetup::collisionGroups::AllFilter,
                EngineSetup::collisionGroups::AllFilter
            );

        }
    }
}

void Mesh3D::drawImGuiCollisionShapeSelector()
{
    auto flags = ImGuiComboFlags_None;
    const char* items[] = { "SIMPLE", "CAPSULE", "TRIANGLE",  };
    int item_current_idx = (int) collisionShape;
    const char* combo_preview_value = items[item_current_idx];

    auto comboTitle = "Shape##" + getLabel();
    if (ImGui::BeginCombo(comboTitle.c_str(), combo_preview_value, flags)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(items[n], is_selected)) {
                if (!is_selected) {
                    item_current_idx = n;
                    switch (n) {
                        case 0: {
                            if (collisionMode == CollisionMode::GHOST) {
                                setupGhostCollider(CollisionShape::SIMPLE_SHAPE);
                            }

                            if (collisionMode == CollisionMode::BODY) {
                                setupRigidBodyCollider(CollisionShape::SIMPLE_SHAPE);
                            }

                            break;
                        }
                        case 1: {
                            if (collisionMode == CollisionMode::GHOST) {
                                setupGhostCollider(CollisionShape::CAPSULE);
                            }

                            if (collisionMode == CollisionMode::BODY) {
                                setupRigidBodyCollider(CollisionShape::CAPSULE);
                            }
                            break;
                        }
                        case 2: {
                            if (collisionMode == CollisionMode::GHOST) {
                                setupGhostCollider(CollisionShape::TRIANGLE_MESH_SHAPE);
                            }

                            if (collisionMode == CollisionMode::BODY) {
                                setupRigidBodyCollider(CollisionShape::TRIANGLE_MESH_SHAPE);
                            }

                            break;
                        }
                    }
                }
            }
        }
        ImGui::EndCombo();
    }

    if (getCollisionShape() == SIMPLE_SHAPE) {
        if (ImGui::Button("Capture from AABB")) {
            updateBoundingBox();
            simpleShapeSize = aabb.size().getScaled(0.5f);
            UpdateShape();
        }
    }
}

void Mesh3D::makeRigidBodyFromTriangleMeshFromConvexHull(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    Logging::Message("[Mesh3D] makeRigidBodyFromTriangleMeshFromConvexHull for %s", getLabel().c_str());

    setMass(mass);

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(getPosition().toBullet());

    btVector3 inertia(0, 0, 0);
    btCollisionShape* shape = getConvexHullShapeFromMesh(inertia);

    shape->calculateLocalInertia(mass, inertia); // Calcula el tensor de inercia

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
    body->setRestitution(restitution);
    body->setActivationState(ACTIVE_TAG);
    body->setLinearFactor(linearFactor.toBullet());
    body->setAngularFactor(angularFactor.toBullet());
    body->setFriction(friction);
    body->setDamping(linearDamping, angularDamping);

    world->addRigidBody(body, collisionGroup, collisionMask);
}

void Mesh3D::makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    Logging::Message("[Mesh3D] makeRigidBodyFromTriangleMesh for %s", getLabel().c_str());

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
    body->setActivationState(DISABLE_DEACTIVATION);
    body->setAngularFactor(angularFactor.toBullet());

    if (mass <= 0) {
        body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
    }

    world->addRigidBody(this->body, collisionGroup, collisionMask);
}

btBvhTriangleMeshShape *Mesh3D::getTriangleMeshFromMesh3D(btVector3 inertia)
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
    s->calculateLocalInertia(mass, inertia);

    return s;
}

btConvexHullShape *Mesh3D::getConvexHullShapeFromMesh(btVector3 inertia)
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

    convexHull->calculateLocalInertia(mass, inertia);

    return convexHull;
}

void Mesh3D::setSourceFile(const std::string &sourceFile)
{
    Mesh3D::sourceFile = sourceFile;
}

void Mesh3D::updateBoundingBox()
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

void Mesh3D::fillGrid3DFromGeometry()
{
    grid->reset(grid->getNumberCubesX(), grid->getNumberCubesY(), grid->getNumberCubesZ());
    for (auto &m: meshes) {
        grid->doTestForNonEmptyGeometry(m.modelTriangles);
    }
}

void Mesh3D::addCustomShader(ShaderOpenGLCustom *s)
{
    customShaders.emplace_back(s);
}

void Mesh3D::loadShader(std::string folder, std::string jsonFilename)
{
    auto name = Tools::getFilenameWithoutExtension(jsonFilename);

    std::string shaderFragmentFile = folder + std::string(name + ".fs");
    std::string shaderVertexFile = folder + std::string(name + ".vs");

    auto type = ShaderOpenGLCustom::extractTypeFromShaderName(folder, name);

    Logging::Message("LoadShaderInto Scene: Folder: %s, Name: %s, Type: %d", folder.c_str(), name.c_str(), type);

    switch(type) {
        case ShaderCustomTypes::SHADER_POSTPROCESSING : {
            Logging::Message("[error] You can't add a 'Postprocessing shader' type into Mesh3D");
            break;
        }
        case ShaderCustomTypes::SHADER_OBJECT : {
            addCustomShader(new ShaderOpenGLCustomMesh3D(this, name, shaderVertexFile, shaderFragmentFile));
            break;
        }
    }
}

void Mesh3D::removeShader(int index)
{
    if (index >= 0 && index < customShaders.size()) {
        customShaders.erase(customShaders.begin() + index);
    }
}

const std::vector<ShaderOpenGLCustom *> &Mesh3D::getCustomShaders() const {
    return customShaders;
}

void Mesh3D::checkClickObject(Vector3D ray, Object3D *&foundObject, float &lastDepthFound)
{
    auto *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    for (auto m: meshes) {
        for (auto &triangle : m.modelTriangles) {
            triangle->updateObjectSpace();
            auto p = Plane(triangle->Ao, triangle->Bo, triangle->Co);
            float t;
            if (Maths::isVector3DClippingPlane(p, ray)) {
                Vertex3D intersectionPoint  = p.getPointIntersection(ray.origin(), ray.end(), t);
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
    }
}