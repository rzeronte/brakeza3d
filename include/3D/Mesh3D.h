
#ifndef SDL2_3D_ENGINE_MESH_H
#define SDL2_3D_ENGINE_MESH_H

#include <string>
#include <assimp/scene.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "Vertex3D.h"
#include "../Render/Triangle3D.h"
#include "../Misc/Tools.h"
#include "../Render/Octree.h"
#include "../Render/Grid3D.h"
#include "../Render/Collider.h"
#include "../OpenGL/ShaderOGLCustom.h"
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
    bool render = true;
protected:
    std::string sourceFile;
    Vertex3D drawOffset = Vertex3D::zero();

    std::vector<Image *> modelTextures;
    std::vector<Image *> modelSpecularTextures;
    std::vector<ShaderOGLCustom*> customShaders;
    std::vector<Mesh3DData> meshes;

    AABB3D aabb;
    Octree *octree = nullptr;
    Grid3D *grid = nullptr;
public:
    Mesh3D();
    Mesh3D(std::string modelFile);
    ~Mesh3D() override;

    void AssimpLoadGeometryFromFile(const std::string &fileName);
    void AssimpInitMaterials(const aiScene *pScene);
    void ProcessNodes(const aiScene *scene, const aiNode *node);
    void LoadMesh(int meshId, const aiMesh *mesh);
    void onUpdate() override;
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
    void AddCustomShader(ShaderOGLCustom *);
    void LoadShader(const std::string &jsonFilename);
    void RemoveShader(int i);
    void FillOGLBuffers();
    virtual void ShadowMappingPass();
    virtual void UpdateBoundingBox();
    btBvhTriangleMeshShape *getTriangleMeshFromMesh3D(btVector3 inertia) const;
    btConvexHullShape *getConvexHullShapeFromMesh(btVector3 inertia) const;

    void setRender(bool render);
    void setSourceFile(const std::string &sourceFile);

    ObjectType getTypeObject() const override                                    { return ObjectType::Mesh3D; }
    GUIType::Sheet getIcon() override                                            { return IconObject::MESH_3D; }
    std::vector<Mesh3DData> &getMeshData()                                       { return meshes; }
    AABB3D &getAABB()                                                            { return aabb; }
    [[nodiscard]] const std::vector<ShaderOGLCustom *> &getCustomShaders() const { return customShaders; }
    [[nodiscard]] const std::vector<Image *> &getModelSpecularTextures() const   { return modelSpecularTextures; }
    [[nodiscard]] bool isRender() const                                          { return render; }
    [[nodiscard]] Grid3D *getGrid3D() const                                      { return grid; }
    [[nodiscard]] Octree *getOctree() const                                      { return octree; }
    [[nodiscard]] std::vector<Triangle *> &getModelTriangles(int i)              { return meshes[i].modelTriangles; }
    [[nodiscard]] std::vector<Image *> &getModelTextures()                       { return modelTextures; }
    [[nodiscard]] std::vector<Vertex3D *> &getModelVertices(int i)               { return meshes[i].modelVertices; }

    friend class Mesh3DSerializer;
    friend class Mesh3DGUI;
};

#endif //SDL2_3D_ENGINE_MESH_H
