
#ifndef SDL2_3D_ENGINE_MESH_H
#define SDL2_3D_ENGINE_MESH_H

#include <mutex>
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

struct Mesh3DData {
    std::vector<Triangle *> modelTriangles;
    std::vector<Vertex3D *> modelVertices;

    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;

    GLuint vertexBuffer;
    GLuint feedbackBuffer;
    GLuint uvBuffer;
    GLuint normalBuffer;
    GLuint vertexBoneDataBuffer;

    int materialIndex;
};

class Mesh3D : public Object3D
{
    bool sharedTextures = false;
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

    AABB3D aabb;
    Octree *octree = nullptr;
    Grid3D *grid = nullptr;
    bool loaded = false;

    bool renderDefaultPipeline = true;
public:


    Mesh3D();
    Mesh3D(const FilePath::ModelFile& modelFile);
    ~Mesh3D() override;

    void AssimpLoadGeometryFromFile(const FilePath::ModelFile &fileName);
    void AssimpInitMaterials(const aiScene *pScene);
    void ProcessNodes(const aiScene *scene, const aiNode *node);
    void LoadMesh(int meshId, const aiMesh *mesh);
    void onUpdate() override;
    void RunObjectShaders() const;
    void postUpdate() override;
    void BuildOctree(int depth);
    void DrawPropertiesGUI() override;
    void makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
    void makeRigidBodyFromTriangleMeshFromConvexHull(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
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
    void FillOGLBuffers();
    virtual void ShadowMappingPass();
    virtual void UpdateBoundingBox();
    [[nodiscard]] btBvhTriangleMeshShape *getTriangleMeshFromMesh3D(btVector3 inertia) const;
    [[nodiscard]] btConvexHullShape *getConvexHullShapeFromMesh(btVector3 inertia);

    void setSourceFile(const FilePath::ModelFile &sourceFile);

    void setRenderPipelineDefault(bool value);
    
    void InitializeShaderChain(int screenWidth, int screenHeight);
    void ProcessShaderChain(GLuint finalFBO);
    void CleanupShaderChain();
    
    void SetChainTempTexture(GLuint texture) const { chainTempTexture = texture; }
    GLuint GetChainTempTexture() const { return chainTempTexture; }

    [[nodiscard]] bool isLoaded() const                                           { return loaded; }
    [[nodiscard]] ObjectType getTypeObject() const override                       { return ObjectType::Mesh3D; }
    GUIType::Sheet getIcon() override                                             { return IconObject::MESH_3D; }
    std::vector<Mesh3DData> &getMeshData()                                        { return meshes; }
    const std::vector<Mesh3DData> &getMeshData() const                             { return meshes; }
    AABB3D &getAABB()                                                             { return aabb; }
    [[nodiscard]] const std::vector<ShaderBaseCustom *> &getCustomShaders() const { return customShaders; }
    [[nodiscard]] const std::vector<Image *> &getModelSpecularTextures() const    { return modelSpecularTextures; }
    [[nodiscard]] Grid3D *getGrid3D() const                                       { return grid; }
    [[nodiscard]] Octree *getOctree() const                                       { return octree; }
    [[nodiscard]] std::vector<Triangle *> &getModelTriangles(int i)               { return meshes[i].modelTriangles; }
    [[nodiscard]] std::vector<Image *> &getModelTextures()                        { return modelTextures; }
    [[nodiscard]] const std::vector<Image *> &getModelTextures() const             { return modelTextures; }
    [[nodiscard]] std::vector<Vertex3D *> &getModelVertices(int i)                { return meshes[i].modelVertices; }
    [[nodiscard]] bool isRenderPipelineDefault() const                            { return renderDefaultPipeline; }

    friend class Mesh3DSerializer;
    friend class Mesh3DGUI;
};

#endif //SDL2_3D_ENGINE_MESH_H
