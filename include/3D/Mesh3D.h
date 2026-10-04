
#ifndef SDL2_3D_ENGINE_MESH_H
#define SDL2_3D_ENGINE_MESH_H

#include <mutex>
#include <atomic>
#include <algorithm>
#include <memory>
#include <string>
#include <assimp/scene.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "Vertex3D.h"
#include "../Render/Triangle3D.h"
#include "../Misc/Tools.h"
#include "../Misc/FilePaths.h"
#include "../Render/Octree.h"
#include "../Render/Grid3D.h"
#include "../Render/Collider.h"
#include "../OpenGL/Code/ShaderBaseCustomOGLCode.h"
#include "../Render/Mesh3DShaderChain.h"
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>

struct MaterialEntryData;
struct ModelData;

struct Mesh3DData {
    std::vector<Triangle *> modelTriangles;
    std::vector<Vertex3D *> modelVertices;

    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;

    GLuint vertexBuffer = 0;
    GLuint feedbackBuffer = 0;
    GLuint feedbackNormalBuffer = 0;
    GLuint uvBuffer = 0;
    GLuint normalBuffer = 0;
    GLuint vertexBoneDataBuffer = 0;

    // Fase 2.2: EBO para el pipeline estatico (Mesh3D sin transform feedback). indexBuffer==0
    // significa "sin EBO" -- se sigue dibujando con glDrawArrays (mallas animadas, o instancias
    // sin ModelData compartido que no hayan pasado por el nuevo camino de FillOGLBuffers).
    GLuint indexBuffer = 0;
    GLsizei indexCount = 0;

    int materialIndex;

    std::string name;
    unsigned int submeshPickingId = 0;
    Color submeshPickingColor;

    AABB3D localAabb;
    bool visibleInFrustum = true;
};

class Mesh3D : public Object3D
{
    std::mutex mtx;

protected:
    FilePath::ModelFile sourceFile;
    Vertex3D drawOffset = Vertex3D::zero();

    std::vector<Image *> modelTextures;
    std::vector<Image *> modelSpecularTextures;
    std::vector<ShaderBaseCustom*> customShaders;
    std::vector<Mesh3DData> meshes;
    Mesh3DShaderChain* shaderChain = nullptr;
    mutable GLuint chainTempTexture = 0;

    // Mantiene vivo el ModelData cacheado (y sus buffers de GPU compartidos, Fase 2.1 del
    // plan de rendimiento) mientras esta instancia exista, independientemente de si
    // ModelDataCache sigue reteniendolo o no (p.ej. tras pulsar "Clear Meshes" con la
    // escena cargada). Vacio si la carga fallo o la instancia no vino de un fichero cacheado.
    std::shared_ptr<ModelData> sharedModel;

    // true cuando vertexBuffer/uvBuffer/normalBuffer (y, en Mesh3DAnimation, tambien
    // vertexBoneDataBuffer) son propiedad de un recurso compartido (ModelData o
    // AnimationData) y NO deben liberarse en ~Mesh3D() -- lo hace el destructor de ese
    // recurso compartido cuando desaparece la ultima instancia que lo usa. Mesh3D la pone
    // a true junto con sharedModel; Mesh3DAnimation la pone a true junto con su propio
    // sharedAnimModel (Mesh3D no necesita conocer el tipo AnimationData).
    bool sharedStaticGeometry = false;

    AABB3D aabb;
    Octree *octree = nullptr;
    Grid3D *grid = nullptr;
    std::atomic<bool> loaded = false;
    std::atomic<bool> loadFailed = false;

    bool sharedTextures = false;
    bool renderDefaultPipeline = true;
    bool frustumCullSubmeshes = false;

    // Emisión sobre el diffuse (desactivada por defecto): el objeto se ve con el color de su
    // textura sin depender de las luces. intensity 0..1 = mezcla entre iluminado y diffuse puro.
    bool emissionEnabled = false;
    float emissionIntensity = 1.0f;
public:


    Mesh3D();
    Mesh3D(const FilePath::ModelFile& modelFile);
    ~Mesh3D() override;

    void AssimpLoadGeometryFromFile(const FilePath::ModelFile &fileName);
    void AssimpInitMaterials(const aiScene *pScene, std::vector<MaterialEntryData>* outMaterialEntries = nullptr);
    void ProcessNodes(const aiScene *scene, const aiNode *node);
    void LoadMesh(int meshId, const aiMesh *mesh, const std::string &nodeName = "");
    void RegisterSubmeshPicking();
    void UnregisterSubmeshPicking();
    void onUpdate() override;
    void RunObjectShaders() const;
    void postUpdate() override;
    void BuildOctree(int depth);
    void DrawPropertiesGUI() override;
    void makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
    void makeRigidBodyFromTriangleMeshFromConvexHull(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
    btRigidBody* BuildRigidBodyFromTriangleMeshOnly(float mass);
    btRigidBody* BuildRigidBodyFromConvexHullOnly(float mass);
    void makeGhostBody(btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask) override;
    void SetupGhostCollider(CollisionShape modeShape) override;
    void SetupRigidBodyCollider(CollisionShape modeShape) override;
    void DrawImGuiCollisionShapeSelector() override;
    void BuildGrid3D(int sizeX, int sizeY, int sizeZ);
    void FillGrid3DFromGeometry();
    void AddCustomShader(ShaderBaseCustom *);
    void LoadShader(const FilePath::ShaderConfigFile &jsonFilename);
    void RemoveShader(int i);
    void MoveShaderUp(ShaderBaseCustom* shader);
    void MoveShaderDown(ShaderBaseCustom* shader);
    virtual void FillOGLBuffers();
    virtual void ShadowMappingPass();
    virtual void UpdateBoundingBox();
    void updateSubmeshFrustumVisibility();
    [[nodiscard]] btBvhTriangleMeshShape *getTriangleMeshFromMesh3D(btVector3 inertia) const;
    [[nodiscard]] btConvexHullShape *getConvexHullShapeFromMesh(btVector3 inertia);

    void setSourceFile(const FilePath::ModelFile &sourceFile);

    void setRenderPipelineDefault(bool value);
    
    void InitializeShaderChain(int screenWidth, int screenHeight);
    void ProcessShaderChain(GLuint finalFBO);
    void CleanupShaderChain();

    [[nodiscard]] Mesh3DShaderChain* GetShaderChain() const { return shaderChain; }
    
    void SetChainTempTexture(GLuint texture) const { chainTempTexture = texture; }
    GLuint GetChainTempTexture() const { return chainTempTexture; }

    [[nodiscard]] bool isLoaded() const                                           { return loaded; }
    [[nodiscard]] bool isLoadFailed() const                                       { return loadFailed; }
    [[nodiscard]] ObjectType getTypeObject() const override                       { return ObjectType::Mesh3D; }
    GUIType::Sheet getIcon() override                                             { return IconObject::MESH_3D; }
    std::vector<Mesh3DData> &getMeshData()                                        { return meshes; }
    const std::vector<Mesh3DData> &getMeshData() const                             { return meshes; }
    AABB3D &getAABB()                                                             { return aabb; }
    [[nodiscard]] const std::vector<ShaderBaseCustom *> &getCustomShaders() const { return customShaders; }
    [[nodiscard]] const std::vector<Image *> &getModelSpecularTextures() const    { return modelSpecularTextures; }
    std::vector<Image *> &getModelSpecularTextures()                              { return modelSpecularTextures; }
    void setSharedTextures(bool v)                                                { sharedTextures = v; }
    [[nodiscard]] bool isSharedTextures() const                                   { return sharedTextures; }
    [[nodiscard]] Grid3D *getGrid3D() const                                       { return grid; }
    [[nodiscard]] Octree *getOctree() const                                       { return octree; }
    [[nodiscard]] std::vector<Triangle *> &getModelTriangles(int i)               { return meshes[i].modelTriangles; }
    [[nodiscard]] std::vector<Image *> &getModelTextures()                        { return modelTextures; }
    [[nodiscard]] const std::vector<Image *> &getModelTextures() const             { return modelTextures; }
    [[nodiscard]] std::vector<Vertex3D *> &getModelVertices(int i)                { return meshes[i].modelVertices; }
    [[nodiscard]] bool isRenderPipelineDefault() const                            { return renderDefaultPipeline; }
    [[nodiscard]] bool isFrustumCullSubmeshes() const                             { return frustumCullSubmeshes; }
    void setFrustumCullSubmeshes(bool value)                                      { frustumCullSubmeshes = value; }
    [[nodiscard]] const std::string &getModelFile() const                         { return sourceFile.str(); }
    [[nodiscard]] bool isEmissionEnabled() const                                  { return emissionEnabled; }
    void setEmissionEnabled(bool value)                                           { emissionEnabled = value; }
    [[nodiscard]] float getEmissionIntensity() const                              { return emissionIntensity; }
    void setEmissionIntensity(float value)                                        { emissionIntensity = std::clamp(value, 0.0f, 1.0f); }

    friend class Mesh3DSerializer;
    friend class Mesh3DGUI;
};

#endif //SDL2_3D_ENGINE_MESH_H
