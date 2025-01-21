#define GL_GLEXT_PROTOTYPES

#include <vector>
#include <assimp/cimport.h>
#include <glm/ext/matrix_float4x4.hpp>
#include "../../include/Objects/Mesh3D.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Misc/ToolsJSON.h"

Mesh3D::Mesh3D()
:
    octree(nullptr),
    grid(nullptr),
    sharedTextures(false),
    flatTextureColor(false),
    render(true)
{
    decal = false;
    luaEnvironment["this"] = this;
}

void Mesh3D::updateBoundingBox()
{
    // Obtener la matriz de modelo-vista-proyección
    glm::mat4 mvpMatrix = Camera3D::getProjectionMatrix() * ComponentsManager::get()->getComponentCamera()->getCamera()->getViewMatrix() * getModelMatrix();

    // Inicializar los valores extremos
    float maxX = -FLT_MAX, minX = FLT_MAX, maxY = -FLT_MAX, minY = FLT_MAX, maxZ = -FLT_MAX, minZ = FLT_MAX;

    for (auto &vertex : this->vertices) {
        // Transformar el vértice por la matriz de modelo-vista-proyección
        glm::vec4 transformedVertex = mvpMatrix * glm::vec4(vertex, 1.0f);

        // Normalizar las coordenadas w para obtener coordenadas de clip
        transformedVertex /= transformedVertex.w;

        // Actualizar los valores máximos y mínimos
        maxX = std::max(maxX, transformedVertex.x);
        minX = std::min(minX, transformedVertex.x);
        maxY = std::max(maxY, transformedVertex.y);
        minY = std::min(minY, transformedVertex.y);
        maxZ = std::max(maxZ, transformedVertex.z);
        minZ = std::min(minZ, transformedVertex.z);
    }

    // Actualizar el bounding box
    this->aabb.max.x = maxX;
    this->aabb.max.y = maxY;
    this->aabb.max.z = maxZ;

    this->aabb.min.x = minX;
    this->aabb.min.y = minY;
    this->aabb.min.z = minZ;

    // Actualizar los vértices del bounding box (si es necesario)
    this->aabb.updateVertices();
}

void Mesh3D::cloneParts(Mesh3D *source, bool isFlatTextureColor, bool isEnableLights, Color color)
{
    for (auto &triangle : source->modelTriangles) {
        auto *t = new Triangle(triangle->A, triangle->B, triangle->C, this);

        t->setFlatColor(color);
        t->setEnableLights(isEnableLights);

        this->modelTriangles.push_back(t);
    }

    this->modelTextures = source->modelTextures;
    this->scale = source->scale;
    this->sharedTextures = true;
}

void Mesh3D::clone(Mesh3D *source)
{
    this->sharedTextures = true;

    this->modelTextures = source->modelTextures;
    this->modelSpecularTextures = source->modelSpecularTextures;
    this->scale = source->scale;

    this->uvbuffer = source->uvbuffer;
    this->normalbuffer = source->normalbuffer;
    this->vertexbuffer = source->vertexbuffer;

    this->uvs = source->uvs;
    this->normals = source->normals;
    this->vertices = source->vertices;
}

void Mesh3D::onUpdate()
{
    Object3D::onUpdate();

    if (isRemoved()) return;

    for (auto s: shaders) {
        s->preUpdate();
    }

    if (EngineSetup::get()->TRIANGLE_MODE_COLOR_SOLID && isRender()) {
        ComponentsManager::get()->getComponentWindow()->getShaderOglShading()->render(
            getModelMatrix(),
            vertexbuffer,
            uvbuffer,
            normalbuffer,
            (int) vertices.size(),
            ComponentsManager::get()->getComponentWindow()->getSceneFramebuffer()
        );
    }

    if (EngineSetup::get()->TRIANGLE_MODE_TEXTURIZED && isRender()) {

        auto window = ComponentsManager::get()->getComponentWindow();

        if (ComponentsManager::get()->getComponentRender()->getSelectedObject() == this) {
            window->getShaderOGLOutline()->drawOutline(this, Color::red(), 0.5f);
        }

        window->getShaderOGLRender()->renderMesh(this,window->getSceneFramebuffer());
    }

    if (EngineSetup::get()->TRIANGLE_MODE_WIREFRAME && isRender()){
        ComponentsManager::get()->getComponentWindow()->getShaderOglWireframe()->render(
            getModelMatrix(),
            vertexbuffer,
            uvbuffer,
            normalbuffer,
            (int) vertices.size(),
            ComponentsManager::get()->getComponentWindow()->getSceneFramebuffer()
        );
    }

    if (EngineSetup::get()->DRAW_MESH3D_AABB && isRender()) {
        this->updateBoundingBox();
        Drawable::drawAABB(&this->aabb, Color::white());
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
        Logging::Message("Error import 3D file not exist");
        std::cout << "Error import 3D file not exist" << std::endl;
        exit(-1);
    }

    Assimp::Importer assimpImporter;
    const aiScene *scene = assimpImporter.ReadFile(
            fileName,
            aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_FlipUVs | aiProcess_OptimizeMeshes
    );

    if (!scene) {
        Logging::Log("Error import 3D file for ASSIMP");
        exit(-1);
    }

    AssimpInitMaterials(scene, fileName);
    AssimpProcessNodes(scene, scene->mRootNode);

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

    Logging::Log("ASSIMP: mNumMaterials: %d", pScene->mNumMaterials);

    for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {

        aiMaterial *pMaterial = pScene->mMaterials[i];
        Logging::Message("[ASSIMP] Loading material: %s", pMaterial->GetName().C_Str());

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

            Logging::Message("[ASSIMP] Loading '%s' as texture for mesh: %s", FullPath.c_str(), Filename.c_str());

            this->modelTextures.push_back(new Image(FullPath));
            this->modelSpecularTextures.push_back(new Image(FullPath));
        } else {
            Logging::Log("ERROR: mMaterial[%s]: Not valid color", i);
        }
        //}
    }
}

void Mesh3D::AssimpProcessNodes(const aiScene *scene, aiNode *node)
{
    for (unsigned int x = 0; x < node->mNumMeshes; x++) {
        int idMesh = (int) node->mMeshes[x];
        this->AssimpLoadMesh(scene->mMeshes[idMesh]);
    }

    for (unsigned int j = 0; j < node->mNumChildren; j++) {
        AssimpProcessNodes(scene, node->mChildren[j]);
    }
}

void Mesh3D::AssimpLoadMesh(aiMesh *mesh)
{
    if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
        Logging::Log("Skip mesh non triangle: %s", mesh->mPrimitiveTypes);
        return;
    }

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    std::vector<Vertex3D> localMeshVertices(mesh->mNumVertices);
    for (unsigned int j = 0; j < mesh->mNumVertices; j++) {

        aiVector3t vf = mesh->mVertices[j];

        Vertex3D v(vf.x, -vf.y, vf.z);

        const aiVector3D *pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][j]) : &Zero3D;
        v.u = pTexCoord->x;
        v.v = pTexCoord->y;

        localMeshVertices[j] = v;
        vertices.emplace_back(vf.x, vf.y, vf.z);
        uvs.emplace_back(v.u, v.v);
    }

    for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
        aiVector3t vf = mesh->mNormals[j];
        normals.emplace_back(vf.x, vf.y, vf.z);
    }

    for (unsigned int k = 0; k < mesh->mNumFaces; k++) {
        const aiFace &Face = mesh->mFaces[k];

        if (Face.mNumIndices < 3) continue;

        Vertex3D V1 = localMeshVertices.at(Face.mIndices[0]);
        Vertex3D V2 = localMeshVertices.at(Face.mIndices[1]);
        Vertex3D V3 = localMeshVertices.at(Face.mIndices[2]);

        auto t = new Triangle(V3, V2, V1, this);
        t->setEnableLights(this->isEnableLights());
        t->setFlatColor(this->flatColor);
        this->modelTriangles.push_back(t);
    }
}

Octree *Mesh3D::getOctree() const {
    return octree;
}

Grid3D *Mesh3D::getGrid3D() const {
    return grid;
}

void Mesh3D::buildGrid3DForEmptyContainsStrategy(int sizeX, int sizeY, int sizeZ)
{
    Logging::Log("Building Grid3D for %s (TriangleContains)", getLabel().c_str());

    this->updateBoundingBox();
    this->grid = new Grid3D(
            &this->modelTriangles,
            this->aabb,
            sizeX,
            sizeY,
            sizeZ,
            Grid3D::EmptyStrategies::CONTAIN_TRIANGLES
    );
    this->grid->applyCheckCellEmptyStrategy();
}

void Mesh3D::buildGrid3DForEmptyRayIntersectionStrategy(int sizeX, int sizeY, int sizeZ, Vertex3D direction)
{
    Logging::Log("Building Grid3D for %s (RayIntersection)", getLabel().c_str());

    this->updateBoundingBox();
    this->grid = new Grid3D(
            &this->modelTriangles,
            this->aabb,
            sizeX,
            sizeY,
            sizeZ,
            Grid3D::EmptyStrategies::RAY_INTERSECTION
    );
    this->grid->setRayIntersectionDirection(direction);
    this->grid->applyCheckCellEmptyStrategy();
}

void Mesh3D::buildGrid3DForEmptyDataImageStrategy(int sizeX, int sizeZ, const std::string& filename, int fixedY)
{
    Logging::Log("Building Grid3D for %s (DataImage)", getLabel().c_str());

    this->updateBoundingBox();

    this->grid = new Grid3D(
            &this->modelTriangles,
            this->aabb,
            sizeX,
            1,
            sizeZ,
            Grid3D::EmptyStrategies::IMAGE_FILE
    );

    this->grid->setImageFilename(filename);
    this->grid->setFixedYImageData(fixedY);
    this->grid->applyCheckCellEmptyStrategy();
}

void Mesh3D::buildOctree()
{
    Logging::Log("Building Octree for %s", getLabel().c_str());

    this->updateBoundingBox();

    this->octree = new Octree(this->modelTriangles, aabb);
}

bool Mesh3D::isFlatTextureColor() const
{
    return flatTextureColor;
}

void Mesh3D::setFlatTextureColor(bool isFlatTextureColor)
{
    this->flatTextureColor = isFlatTextureColor;
}

void Mesh3D::setFlatColor(const Color &flatColor)
{
    Mesh3D::flatColor = flatColor;
}

const Color &Mesh3D::getFlatColor() const {
    return flatColor;
}

Mesh3D::~Mesh3D()
{
    Logging::Log("Delete Mesh3D: %s", getLabel().c_str());

    for (auto triangle : modelTriangles) delete triangle;
    for (auto vertex : modelVertices) delete vertex;

    if (!sharedTextures) {
        for (auto texture : modelTextures) delete texture;
        for (auto texture : modelSpecularTextures) delete texture;
    }

    if (loaded) {
        glDeleteBuffers(1, &vertexbuffer);
        glDeleteBuffers(1, &uvbuffer);
        glDeleteBuffers(1, &normalbuffer);
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

std::vector<Triangle *> &Mesh3D::getModelTriangles()
{
    return modelTriangles;
}

std::vector<Image *> &Mesh3D::getModelTextures()
{
    return modelTextures;
}

const std::vector<Image *> &Mesh3D::getModelSpecularTextures() const
{
    return modelSpecularTextures;
}

std::vector<Vertex3D *> &Mesh3D::getModelVertices()
{
    return modelVertices;
}

AABB3D &Mesh3D::getAabb(){
    return aabb;
}

void Mesh3D::onDrawHostBuffer()
{
    Object3D::onDrawHostBuffer();

    if (EngineSetup::get()->DRAW_MESH3D_OCTREE) {
        if (this->octree != nullptr) {
            Drawable::drawOctree(this->octree, true);
        }
    }

    if (EngineSetup::get()->DRAW_MESH3D_GRID) {
        if (this->grid != nullptr) {
            Drawable::drawGrid3D(this->grid);
        }
    }
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
    std::string title = "Mesh3D (File: " + sourceFile + " | Triangles: " + std::to_string(getModelTriangles().size()) + ")";

    if (ImGui::TreeNode("Mesh3D")) {
        if (ImGui::TreeNode("Mesh information")) {
            auto fileModel = std::string("- File model: ") + sourceFile;
            ImGui::Text(fileModel.c_str());

            auto numTriangles = std::string("- Nº Triangles: ") + std::to_string((int) modelTriangles.size());
            ImGui::Text(numTriangles.c_str());
            ImGui::TreePop();
        }
        ImGui::Checkbox(std::string("Enable lights").c_str(), &enableLights);
        ImGui::TreePop();
    }
}

cJSON * Mesh3D::getJSON()
{
    cJSON *root = Object3D::getJSON();

    cJSON_AddStringToObject(root, "model", sourceFile.c_str());
    cJSON_AddBoolToObject(root, "enableLights", isEnableLights());

    return root;
}

void Mesh3D::setPropertiesFromJSON(cJSON *object, Mesh3D *o)
{
    o->setBelongToScene(true);
    Object3D::setPropertiesFromJSON(object, o);
    o->setEnableLights(cJSON_GetObjectItemCaseSensitive(object, "enableLights")->valueint);
    o->AssimpLoadGeometryFromFile(cJSON_GetObjectItemCaseSensitive(object, "model")->valuestring);

    if (cJSON_GetObjectItemCaseSensitive(object, "shaders") != nullptr) {
        auto mesh3DShaderTypes = ComponentsManager::get()->getComponentRender()->getSceneLoader().getMesh3DShaderTypes();
        cJSON *currentShader;
        cJSON_ArrayForEach(currentShader, cJSON_GetObjectItemCaseSensitive(object, "shaders")) {
            auto type = cJSON_GetObjectItemCaseSensitive(currentShader, "type")->valuestring;
            switch(mesh3DShaderTypes[type]) {
                case Mesh3DShaderLoaderMapping::FXOutliner: {
                    auto edgeColor = cJSON_GetObjectItemCaseSensitive(currentShader, "color");
                    auto shader = new FXOutliner(
                        true,
                        o,
                        Color(
                            cJSON_GetObjectItemCaseSensitive(edgeColor, "r")->valueint,
                            cJSON_GetObjectItemCaseSensitive(edgeColor, "g")->valueint,
                            cJSON_GetObjectItemCaseSensitive(edgeColor, "b")->valueint
                        ),
                        (float)cJSON_GetObjectItemCaseSensitive(currentShader, "size")->valuedouble
                    );
                    o->addMesh3DShader(shader);
                    break;
                }
                case Mesh3DShaderLoaderMapping::FXBlink: {
                    auto edgeColor = cJSON_GetObjectItemCaseSensitive(currentShader, "color");
                    auto blinkStep = (float) cJSON_GetObjectItemCaseSensitive(currentShader, "step")->valuedouble;
                    auto shader = new FXBlink( true,o,blinkStep, ToolsJSON::parseColorJSON(edgeColor));
                    o->addMesh3DShader(shader);
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

            switch(mode) {
                default:
                case CollisionMode::GHOST:
                    if (shape == CollisionShape::SIMPLE_SHAPE) {
                        o->setupGhostCollider(CollisionShape::SIMPLE_SHAPE);
                    }
                    if (shape == CollisionShape::TRIANGLE_MESH_SHAPE) {
                        o->setupGhostCollider(CollisionShape::TRIANGLE_MESH_SHAPE);
                    }
                    break;
                case CollisionMode::BODY:
                    if (shape == CollisionShape::SIMPLE_SHAPE) {
                        o->setupRigidBodyCollider(CollisionShape::SIMPLE_SHAPE);
                    }
                    if (shape == CollisionShape::TRIANGLE_MESH_SHAPE) {
                        o->setupRigidBodyCollider(CollisionShape::TRIANGLE_MESH_SHAPE);
                    }
                    break;
            }
        }
    }
}

void Mesh3D::createFromJSON(cJSON *object)
{
    auto o = new Mesh3D();

    Mesh3D::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

void Mesh3D::fillBuffers()
{
    loaded = true;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
}

void Mesh3D::makeGhostBody(btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    auto *convexHullShape = new btConvexHullShape();

    updateBoundingBox();
    for (auto & modelTriangle : getModelTriangles()) {
        btVector3 a, b, c;
        a = btVector3(modelTriangle->A.x, -modelTriangle->A.y, modelTriangle->A.z);
        b = btVector3(modelTriangle->B.x, -modelTriangle->B.y, modelTriangle->B.z);
        c = btVector3(modelTriangle->C.x, -modelTriangle->C.y, modelTriangle->C.z);
        convexHullShape->addPoint(a);
        convexHullShape->addPoint(b);
        convexHullShape->addPoint(c);
    }

    getGhostObject()->setWorldTransform(Tools::GLMMatrixToBulletTransform(getModelMatrix()));
    getGhostObject()->setCollisionShape(convexHullShape);
    getGhostObject()->setUserPointer(this);

    world->addCollisionObject(getGhostObject(), collisionGroup, collisionMask);
}

void Mesh3D::setupGhostCollider(CollisionShape modeShape)
{
    Logging::Message("setupGhostCollider for %s", getLabel().c_str());

    removeCollisionObject();

    setCollisionMode(CollisionMode::GHOST);
    setCollisionShape(modeShape);

    if (getCollisionShape() == CollisionShape::SIMPLE_SHAPE) {
        makeSimpleGhostBody(
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
    Logging::Message("setupRigidBodyCollider for %s", getLabel().c_str());
    removeCollisionObject();

    setCollisionShape(modeShape);
    setCollisionMode(CollisionMode::BODY);

    if (getCollisionShape() == CollisionShape::SIMPLE_SHAPE) {
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
    const char* items[] = { "SIMPLE", "TRIANGLE" };
    int item_current_idx = (int) collisionShape;
    const char* combo_preview_value = items[item_current_idx];

    auto comboTitle = "Collision shape##" + getLabel();
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
}

void Mesh3D::makeRigidBodyFromTriangleMeshFromConvexHull(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    Logging::Message("makeRigidBodyFromTriangleMeshFromConvexHull for %s", getLabel().c_str());

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
    body->setRestitution(0);
    body->setActivationState(ACTIVE_TAG);

    body->setLinearFactor(btVector3(1, 1, 1));
    body->setAngularFactor(btVector3(1, 1, 1));

    world->addRigidBody(this->body, collisionGroup, collisionMask);
}

void Mesh3D::makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    Logging::Message("makeRigidBodyFromTriangleMesh for %s", getLabel().c_str());

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

    if (mass <= 0) {
        body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
    }

    world->addRigidBody(this->body, collisionGroup, collisionMask);
}

btBvhTriangleMeshShape *Mesh3D::getTriangleMeshFromMesh3D(btVector3 inertia)
{
    auto *triangleMesh = new btTriangleMesh();

    for (auto & modelTriangle : this->modelTriangles) {
        btVector3 a = modelTriangle->A.toBullet();
        btVector3 b = modelTriangle->B.toBullet();
        btVector3 c = modelTriangle->C.toBullet();

        a.setY(-a.getY());
        b.setY(-b.getY());
        c.setY(-c.getY());

        triangleMesh->addTriangle(a, b, c, false);
    }

    auto s = new btBvhTriangleMeshShape(triangleMesh, true, true);
    s->calculateLocalInertia(mass, inertia);

    return s;
}

btConvexHullShape *Mesh3D::getConvexHullShapeFromMesh(btVector3 inertia)
{
    auto *convexHull = new btConvexHullShape();
    for (auto &modelTriangle: this->modelTriangles) {
        btVector3 a = modelTriangle->A.toBullet();
        btVector3 b = modelTriangle->B.toBullet();
        btVector3 c = modelTriangle->C.toBullet();

        a.setY(-a.getY());
        b.setY(-b.getY());
        c.setY(-c.getY());

        convexHull->addPoint(a);
        convexHull->addPoint(b);
        convexHull->addPoint(c);
    }
    convexHull->calculateLocalInertia(mass, inertia);

    return convexHull;
}