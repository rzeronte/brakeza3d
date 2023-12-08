#define GL_GLEXT_PROTOTYPES

#include <vector>
#include <assimp/cimport.h>
#include <ext/matrix_float4x4.hpp>
#include "../../include/Objects/Mesh3D.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Render/Transforms.h"

Mesh3D::Mesh3D()
:
    octree(nullptr),
    grid(nullptr),
    sharedTextures(false),
    flatTextureColor(false),
    render(true),
    layer(Mesh3DRenderLayer::ONUPDATE)
{
    decal = false;
    openClRenderer = new MeshOpenCLRenderer(this, this->modelTriangles);
    luaEnvironment.set("this", this);
}

void Mesh3D::sendTrianglesToFrame(std::vector<Triangle *> *frameTriangles)
{
    for (auto & modelTriangle : this->modelTriangles) {
        frameTriangles->push_back(modelTriangle);
    }
}

void Mesh3D::updateBoundingBox()
{
    float maxX = -9999999, minX = 9999999, maxY = -9999999, minY = 9999999, maxZ = -9999999, minZ = 9999999;

    for (auto & modelTriangle : this->modelTriangles) {
        modelTriangle->updateObjectSpace();

        maxX = std::fmax(maxX, modelTriangle->Ao.x);
        minX = std::fmin(minX, modelTriangle->Ao.x);

        maxY = std::fmax(maxY, modelTriangle->Ao.y);
        minY = std::fmin(minY, modelTriangle->Ao.y);

        maxZ = std::fmax(maxZ, modelTriangle->Ao.z);
        minZ = std::fmin(minZ, modelTriangle->Ao.z);

        //
        maxX = std::fmax(maxX, modelTriangle->Bo.x);
        minX = std::fmin(minX, modelTriangle->Bo.x);

        maxY = std::fmax(maxY, modelTriangle->Bo.y);
        minY = std::fmin(minY, modelTriangle->Bo.y);

        maxZ = std::fmax(maxZ, modelTriangle->Bo.z);
        minZ = std::fmin(minZ, modelTriangle->Bo.z);

        //
        maxX = std::fmax(maxX, modelTriangle->Co.x);
        minX = std::fmin(minX, modelTriangle->Co.x);

        maxY = std::fmax(maxY, modelTriangle->Co.y);
        minY = std::fmin(minY, modelTriangle->Co.y);

        maxZ = std::fmax(maxZ, modelTriangle->Co.z);
        minZ = std::fmin(minZ, modelTriangle->Co.z);
    }

    this->aabb.max.x = maxX;
    this->aabb.max.y = maxY;
    this->aabb.max.z = maxZ;

    this->aabb.min.x = minX;
    this->aabb.min.y = minY;
    this->aabb.min.z = minZ;

    this->aabb.updateVertices();
}

void Mesh3D::cloneParts(Mesh3D *source, bool isFlatTextureColor, bool isEnableLights, Color color)
{
    for (auto &triangle : source->modelTriangles) {
        auto *t = new Triangle(triangle->A, triangle->B, triangle->C, this);

        t->setTexture(triangle->getTexture());
        t->setFlatTextureColor(isFlatTextureColor);
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
    for (auto &triangle : source->modelTriangles) {
        auto *t = new Triangle(triangle->A, triangle->B, triangle->C,this);

        t->setTexture(triangle->getTexture());
        t->setFlatTextureColor(source->isFlatTextureColor());
        t->setFlatColor(source->getFlatColor());
        t->setEnableLights(triangle->isEnableLights());

        this->modelTriangles.push_back(t);
    }

    this->modelTextures = source->modelTextures;
    this->scale = source->scale;
    this->sharedTextures = true;

    openClRenderer->makeOCLTriangles();
}

void Mesh3D::onUpdate()
{
    Object3D::onUpdate();

    if (isRemoved()) return;

    if (isRender()) {
        //this->sendTrianglesToFrame(&ComponentsManager::get()->getComponentRender()->getFrameTriangles());
    }

    if (layer == Mesh3DRenderLayer::ONUPDATE) {
        //onUpdateOpenCLRender();
        ComponentsManager::get()->getComponentWindow()->getShaderOGLRender()->render(
            getModelMatrix(),
            modelTextures[0]->getOGLTextureID(),
            modelTextures[0]->getOGLTextureID(),
            vertexbuffer,
            uvbuffer,
            normalbuffer,
            (int) vertices.size()
        );
    }

    for (auto s: shaders) {
        s->preUpdate();
    }
}

void Mesh3D::drawOnUpdateSecondPass()
{
    Object3D::drawOnUpdateSecondPass();

    if (layer == Mesh3DRenderLayer::SECONDARY) {
        //onUpdateOpenCLRender();
        //renderOpenGL();
    }
}

void Mesh3D::onUpdateOpenCLRender()
{
    if ((int) modelTriangles.size() > 0) openClRenderer->onUpdate(modelTextures[0]);
}

void Mesh3D::postUpdate()
{
    Object3D::postUpdate();

    for (auto s: shaders) {
        s->postUpdate();
    }
}

void Mesh3D::AssimpLoadGeometryFromFile(const std::string &fileName)
{
    Logging::Log("AssimpLoadGeometryFromFile for %s", fileName.c_str());

    if (!Tools::fileExists(fileName.c_str())) {
        Logging::Log("Error import 3D file not exist");
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

    openClRenderer->makeOCLTriangles();

    sourceFile = fileName;

    //bool res = Tools::loadOBJ("../suzanne.obj", vertices, uvs, normals);
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
        t->setFlatTextureColor(this->isFlatTextureColor());
        t->setEnableLights(this->isEnableLights());
        t->setFlatColor(this->flatColor);
        this->modelTriangles.push_back(t);

        if (!this->modelTriangles.empty()) {
            this->modelTriangles[k]->setTexture(this->modelTextures[mesh->mMaterialIndex]);
        }
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

void Mesh3D::setFlatColor(const Color &flatColor) {
    Mesh3D::flatColor = flatColor;
}

const Color &Mesh3D::getFlatColor() const {
    return flatColor;
}

Mesh3D::~Mesh3D()
{
    Logging::Log("Delete Mesh3D: %s", getLabel().c_str());

    for (auto triangle : modelTriangles) {
        delete triangle;
    }

    for (auto vertex : modelVertices) {
        delete vertex;
    }

    if (!sharedTextures) {
        for (auto texture : modelTextures) {
            delete texture;
        }
    }

    delete openClRenderer;
}

bool Mesh3D::isRender() const {
    return render;
}

void Mesh3D::setRender(bool render) {
    Mesh3D::render = render;
}

std::vector<Triangle *> &Mesh3D::getModelTriangles() {
    return modelTriangles;
}

std::vector<Image *> &Mesh3D::getModelTextures() {
    return modelTextures;
}

std::vector<Vertex3D *> &Mesh3D::getModelVertices(){
    return modelVertices;
}

AABB3D &Mesh3D::getAabb(){
    return aabb;
}


MeshOpenCLRenderer *Mesh3D::getOpenClRenderer() const {
    return openClRenderer;
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

    if (EngineSetup::get()->DRAW_MESH3D_AABB) {
        this->updateBoundingBox();
        Drawable::drawAABB(&this->aabb, Color::white());
    }

}

Mesh3DRenderLayer Mesh3D::getLayer() const {
    return layer;
}

void Mesh3D::setLayer(Mesh3DRenderLayer layer) {
    Mesh3D::layer = layer;
}

const char *Mesh3D::getTypeObject() {
    return "Mesh3D";
}

const char *Mesh3D::getTypeIcon() {
    return "meshIcon";
}

Mesh3D *Mesh3D::create() {
    return new Mesh3D();
}

void Mesh3D::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();
    std::string title = "Mesh3D (Triangles: " + std::to_string(getModelTriangles().size()) + ")";

    if (ImGui::TreeNode(title.c_str())) {
        ImGui::Checkbox(std::string("Enable lights").c_str(), &enableLights);

        ImGui::Separator();

        auto ImGuiTextures = Brakeza3D::get()->getManagerGui()->getImGuiTextures();

        if (ImGui::TreeNode("Shaders")) {
            if ((int) shaders.size() <= 0) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No shaders attached");
            }

            for (int i = 0; i < (int) shaders.size(); i++) {
                auto s = shaders[i];
                ImGui::Image((ImTextureID)ImGuiTextures->getTextureByLabel("shaderIcon")->getTexture(), ImVec2(24, 24));
                ImGui::SameLine(100);

                if (!s->isEnabled()) {
                    if (ImGui::ImageButton((ImTextureID)ImGuiTextures->getTextureByLabel("unlockIcon")->getTexture(), ImVec2(14, 14))) {
                        s->setEnabled(true);
                    }
                } else {
                    if (ImGui::ImageButton((ImTextureID)ImGuiTextures->getTextureByLabel("lockIcon")->getTexture(), ImVec2(14, 14))) {
                        s->setEnabled(false);
                    }
                }
                ImGui::SameLine();
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures->getTextureByLabel("removeIcon")->getTexture(), ImVec2(14, 14))) {
                    removeShader(i);
                }
                ImGui::SameLine();
                if (ImGui::CollapsingHeader(s->getLabel().c_str(), ImGuiTreeNodeFlags_None)) {
                    ImGui::PushID(i);
                    s->drawImGuiProperties();
                    ImGui::PopID();
                }
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

cJSON * Mesh3D::getJSON()
{
    cJSON *root = Object3D::getJSON();

    cJSON_AddStringToObject(root, "model", sourceFile.c_str());
    cJSON_AddBoolToObject(root, "enableLights", isEnableLights());

    cJSON *shadersArrayJSON = cJSON_CreateArray();
    for ( auto s : shaders) {
        cJSON_AddItemToArray(shadersArrayJSON, s->getJSON());
    }
    cJSON_AddItemToObject(root, "shaders", shadersArrayJSON);

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
                case Mesh3DShaderLoaderMapping::ShaderEdgeObject: {
                    auto edgeColor = cJSON_GetObjectItemCaseSensitive(currentShader, "color");
                    auto color = Color(
                        cJSON_GetObjectItemCaseSensitive(edgeColor, "r")->valueint,
                        cJSON_GetObjectItemCaseSensitive(edgeColor, "g")->valueint,
                        cJSON_GetObjectItemCaseSensitive(edgeColor, "b")->valueint
                    );
                    auto shader = new ShaderEdgeObject(
                        true,
                        color,
                        (float)cJSON_GetObjectItemCaseSensitive(currentShader, "size")->valuedouble
                    );
                    o->addMesh3DShader(shader);
                    break;
                }
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

std::vector<ObjectShaderOpenCL *> &Mesh3D::getShaders() {
    return shaders;
}

void Mesh3D::addMesh3DShader(ObjectShaderOpenCL *shader) {
    shader->setObject(this);
    shaders.push_back(shader);
}

void Mesh3D::removeShader(int index) {
    if (index >= 0 && index < shaders.size()) {
        shaders.erase(shaders.begin() + index);
    }
}

void Mesh3D::fillBuffers()
{
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
