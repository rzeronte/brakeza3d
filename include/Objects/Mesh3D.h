
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
#include <assimp/Importer.hpp>      // C++ assimpImporter interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

typedef float vec3_t[3];

class Mesh3D : public Object3D {
private:
    Octree *octree;
    Grid3D *grid;
    std::string sourceFile;

    AABB3D aabb;
    bool sharedTextures;

    bool flatTextureColor;
    bool render;

protected:
    std::vector<Triangle *> modelTriangles;
    std::vector<Texture *> modelTextures;
    std::vector<Vertex3D *> modelVertices;

    Color flatColor;
    MeshOpenCLRenderer *openClRenderer;
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

    void setEnableLights(bool enableLights);

    void setFlatColor(const Color &flatColor);

    void setRender(bool render);

    [[nodiscard]] const Color &getFlatColor() const;

    [[nodiscard]] bool isRender() const;

    [[nodiscard]] Grid3D *getGrid3D() const;

    [[nodiscard]] Octree *getOctree() const;

    [[nodiscard]] bool isFlatTextureColor() const;

    [[nodiscard]] bool isEnableLights() const;

    [[nodiscard]] std::vector<Triangle *> &getModelTriangles() ;

    [[nodiscard]] std::vector<Texture *> &getModelTextures() ;

    [[nodiscard]] std::vector<Vertex3D *> &getModelVertices() ;

    AABB3D &getAabb();

    OCLMeshContext openCLContext();

    bool enableLights;

    MeshOpenCLRenderer *getOpenClRenderer() const;

    void onDraw();
};


#endif //SDL2_3D_ENGINE_MESH_H
