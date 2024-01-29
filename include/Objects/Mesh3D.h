
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
#include <assimp/Importer.hpp>      // C++ assimpImporter interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <vec3.hpp>
#include <vec2.hpp>

typedef float vec3_t[3];

class Mesh3D : public Object3D {
private:

    Octree *octree;
    Grid3D *grid;
    AABB3D aabb;

    bool sharedTextures;
    bool flatTextureColor;
    bool render;
protected:
    std::vector<Triangle *> modelTriangles;
    std::vector<Image *> modelTextures;
    std::vector<Image *> modelSpecularTextures;
    std::vector<Vertex3D *> modelVertices;

    Color flatColor;
    std::string sourceFile;
public:
    Mesh3D();

    ~Mesh3D() override;

    void AssimpLoadGeometryFromFile(const std::string &fileName);

    void AssimpInitMaterials(const aiScene *pScene, const std::string &Filename);

    void AssimpProcessNodes(const aiScene *, aiNode *node);

    void AssimpLoadMesh(aiMesh *mesh);

    void updateBoundingBox();

    void clone(Mesh3D *source);

    void cloneParts(Mesh3D *source, bool isFlatTextureColor, bool isEnableLights, Color color);

    void onUpdate() override;

    void postUpdate() override;

    void buildOctree();

    void buildGrid3DForEmptyContainsStrategy(int sizeX, int sizeY, int sizeZ);

    void buildGrid3DForEmptyRayIntersectionStrategy(int sizeX, int sizeY, int sizeZ, Vertex3D direction);

    void buildGrid3DForEmptyDataImageStrategy(int sizeX, int sizeZ, const std::string& filename, int fixedY);

    void setFlatTextureColor(bool isFlatTextureColor);

    void setFlatColor(const Color &flatColor);

    void setRender(bool render);

    [[nodiscard]] const Color &getFlatColor() const;

    [[nodiscard]] bool isRender() const;

    [[nodiscard]] Grid3D *getGrid3D() const;

    [[nodiscard]] Octree *getOctree() const;

    [[nodiscard]] bool isFlatTextureColor() const;

    [[nodiscard]] std::vector<Triangle *> &getModelTriangles() ;

    [[nodiscard]] std::vector<Image *> &getModelTextures() ;

    [[nodiscard]] std::vector<Vertex3D *> &getModelVertices() ;

    AABB3D &getAabb();

    void onDrawHostBuffer() override;

    cJSON * getJSON() override;

    void onUpdateOpenCLRender();

    void drawImGuiProperties() override;

    const char *getTypeObject() override;

    const char *getTypeIcon() override;

    static Mesh3D* create();

    static void createFromJSON(cJSON *object);

    static void setPropertiesFromJSON(cJSON *object, Mesh3D *o);

    void fillBuffers();

    const std::vector<Image *> &getModelSpecularTextures() const;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
};


#endif //SDL2_3D_ENGINE_MESH_H
