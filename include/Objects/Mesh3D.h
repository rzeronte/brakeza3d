
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
#include "../Render/MeshOpenCLRenderer.h"
#include "../Render/ObjectShaderOpenCL.h"
#include <assimp/Importer.hpp>      // C++ assimpImporter interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <vec3.hpp>
#include <vec2.hpp>

typedef float vec3_t[3];

typedef enum {
    ONUPDATE = 0,
    SECONDARY = 1,
} Mesh3DRenderLayer;

class Mesh3D : public Object3D {
private:

    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;

    Octree *octree;
    Grid3D *grid;
    AABB3D aabb;

    bool sharedTextures;
    bool flatTextureColor;
    bool render;
    Mesh3DRenderLayer layer;
    std::vector<ObjectShaderOpenCL*> shaders;
protected:
    std::vector<Triangle *> modelTriangles;
    std::vector<Image *> modelTextures;
    std::vector<Vertex3D *> modelVertices;

    Color flatColor;
    MeshOpenCLRenderer *openClRenderer;
    std::string sourceFile;
public:
    Mesh3D();

    ~Mesh3D() override;

    void AssimpLoadGeometryFromFile(const std::string &fileName);

    void AssimpInitMaterials(const aiScene *pScene, const std::string &Filename);

    void AssimpProcessNodes(const aiScene *, aiNode *node);

    void AssimpLoadMesh(aiMesh *mesh);

    void sendTrianglesToFrame(std::vector<Triangle *> *frameTriangles);

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

    [[nodiscard]] MeshOpenCLRenderer *getOpenClRenderer() const;

    void onDrawHostBuffer() override;

    cJSON * getJSON() override;

    void onUpdateOpenCLRender();

    void drawOnUpdateSecondPass() override;

    Mesh3DRenderLayer getLayer() const;

    void setLayer(Mesh3DRenderLayer layer);

    void drawImGuiProperties() override;

    const char *getTypeObject() override;

    const char *getTypeIcon() override;

    static Mesh3D* create();

    static void createFromJSON(cJSON *object);

    static void setPropertiesFromJSON(cJSON *object, Mesh3D *o);

    std::vector<ObjectShaderOpenCL *> &getShaders();

    void addMesh3DShader(ObjectShaderOpenCL *shader);

    void removeShader(int i);

    void fillBuffers();

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
};


#endif //SDL2_3D_ENGINE_MESH_H
