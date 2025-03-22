
#ifndef SDL2_3D_ENGINE_MESH_H
#define SDL2_3D_ENGINE_MESH_H

#include <string>
#include "Vertex3D.h"
#include "Triangle3D.h"
#include "../Misc/Tools.h"
#include "../EngineSetup.h"
#include "Object3D.h"
#include "../Misc/Octree.h"
#include "../Misc/Grid3D.h"
#include "../OpenGL/FXEffectOpenGLObject.h"
#include "../Collision/Collider.h"
#include "../OpenGL/ShaderOpenGLCustom.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>

struct meshData {
    std::vector<Triangle *> modelTriangles;
    std::vector<Vertex3D *> modelVertices;

    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;

    GLuint vertexbuffer;
    GLuint feedbackBuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;

    GLuint vertexBoneDataBuffer;

    int materialIndex;
};

class Mesh3D : public Object3D {
private:

    bool sharedTextures;
    bool render;
    bool loaded = false;

protected:
    std::string sourceFile;

    std::vector<Image *> modelTextures;
    std::vector<Image *> modelSpecularTextures;
    AABB3D aabb;
    Octree *octree;
    Grid3D *grid;
    std::vector<ShaderOpenGLCustom*> customShaders;
public:
    std::vector<meshData> meshes;

    Mesh3D();

    ~Mesh3D() override;

    void AssimpLoadGeometryFromFile(const std::string &fileName);

    void AssimpInitMaterials(const aiScene *pScene, const std::string &Filename);

    void ProcessNodes(const aiScene *scene, aiNode *node);

    void LoadMesh(int meshId, aiMesh *mesh);

    void updateBoundingBox();

    void onUpdate() override;

    void postUpdate() override;

    void buildOctree(int depth);

    void setRender(bool render);

    [[nodiscard]] bool isRender() const;

    [[nodiscard]] Grid3D *getGrid3D() const;

    [[nodiscard]] Octree *getOctree() const;

    [[nodiscard]] std::vector<Triangle *> &getModelTriangles(int i) ;

    [[nodiscard]] std::vector<Image *> &getModelTextures() ;

    [[nodiscard]] std::vector<Vertex3D *> &getModelVertices(int i) ;

    AABB3D &getAabb();

    cJSON * getJSON() override;

    void drawImGuiProperties() override;

    const char *getTypeObject() override;

    const char *getTypeIcon() override;

    static Mesh3D* create(Vertex3D position, const std::string& imageFile);

    static void createFromJSON(cJSON *object);

    static void setPropertiesFromJSON(cJSON *object, Mesh3D *o, bool loadGeometry);

    void fillBuffers();

    [[nodiscard]] const std::vector<Image *> &getModelSpecularTextures() const;

    void makeGhostBody(btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask) override;
    void makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);
    void makeRigidBodyFromTriangleMeshFromConvexHull(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);

    void setupGhostCollider(CollisionShape modeShape) override;
    void setupRigidBodyCollider(CollisionShape modeShape) override;
    void drawImGuiCollisionShapeSelector() override;

    btBvhTriangleMeshShape *getTriangleMeshFromMesh3D(btVector3 inertia);

    btConvexHullShape *getConvexHullShapeFromMesh(btVector3 inertia);

    void setSourceFile(const std::string &sourceFile);

    void buildGrid3D(int sizeX, int sizeY, int sizeZ);

    void fillGrid3DFromGeometry();

    void addCustomShader(ShaderOpenGLCustom *);

    void loadShader(std::string folder, std::string jsonFilename);

    void removeShader(int i);

    [[nodiscard]] const std::vector<ShaderOpenGLCustom *> &getCustomShaders() const;

    void checkClickObject(Vector3D ray, Object3D *&foundObject, float &lastDepthFound) override;
};

#endif //SDL2_3D_ENGINE_MESH_H
