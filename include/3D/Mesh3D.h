
#ifndef SDL2_3D_ENGINE_MESH_H
#define SDL2_3D_ENGINE_MESH_H

#include <string>
#include "Vertex3D.h"
#include "../Render/Triangle3D.h"
#include "../Misc/Tools.h"
#include "../Misc/Octree.h"
#include "../Misc/Grid3D.h"
#include "../Collision/Collider.h"
#include "../OpenGL/ShaderOGLCustom.h"
#include <assimp/scene.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
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
    ~Mesh3D() override;
    // TODO: Borrar o mover esto de quí (LUA)
    static Mesh3D* create(Vertex3D position, const std::string& imageFile);

    const char *getTypeObject() override;
    const char *getTypeIcon() override;
    void AssimpLoadGeometryFromFile(const std::string &fileName);
    void AssimpInitMaterials(const aiScene *pScene);
    void ProcessNodes(const aiScene *scene, aiNode *node);
    void LoadMesh(int meshId, aiMesh *mesh);
    void onUpdate() override;
    void postUpdate() override;
    void buildOctree(int depth);
    void setRender(bool render);
    void drawImGuiProperties() override;
    void makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
    void makeRigidBodyFromTriangleMeshFromConvexHull(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
    void makeGhostBody(btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask) override;
    void setupGhostCollider(CollisionShape modeShape) override;
    void setupRigidBodyCollider(CollisionShape modeShape) override;
    void drawImGuiCollisionShapeSelector() override;
    void setSourceFile(const std::string &sourceFile);
    void buildGrid3D(int sizeX, int sizeY, int sizeZ);
    void fillGrid3DFromGeometry();
    void addCustomShader(ShaderOGLCustom *);
    void loadShader(std::string folder, std::string jsonFilename);
    void removeShader(int i);
    void checkClickObject(Vector3D ray, Object3D *&foundObject, float &lastDepthFound) override;
    virtual void shadowMappingPass();
    virtual void updateBoundingBox();
    std::vector<Mesh3DData> &getMeshData();
    btBvhTriangleMeshShape *getTriangleMeshFromMesh3D(btVector3 inertia);
    btConvexHullShape *getConvexHullShapeFromMesh(btVector3 inertia);
    AABB3D &getAABB();
    [[nodiscard]] const std::vector<ShaderOGLCustom *> &getCustomShaders() const;
    [[nodiscard]] const std::vector<Image *> &getModelSpecularTextures() const;
    [[nodiscard]] bool isRender() const;
    [[nodiscard]] Grid3D *getGrid3D() const;
    [[nodiscard]] Octree *getOctree() const;
    [[nodiscard]] std::vector<Triangle *> &getModelTriangles(int i);
    [[nodiscard]] std::vector<Image *> &getModelTextures();
    [[nodiscard]] std::vector<Vertex3D *> &getModelVertices(int i);

    friend class Mesh3DSerializer;
    friend class Mesh3DGUI;
};

#endif //SDL2_3D_ENGINE_MESH_H
