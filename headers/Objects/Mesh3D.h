
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
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#define MAX_VERTEX_MODEL 250000
#define MAX_MESH_TEXTURES 512

typedef float vec3_t[3];

class Mesh3D : public Object3D {

public:
    std::string source_file;
    std::string prefix_texture_folder;
    Assimp::Importer importer;

    std::vector<Triangle *> modelTriangles;
    AABB3D aabb;

    Texture *modelTextures;
    Vertex3D *modelVertices;

    int numTextures;

    bool shadowCaster;
    int BSPEntityIndex;

    Mesh3D();

    bool AssimpLoadGeometryFromFile(const std::string &fileName);

    bool AssimpInitMaterials(const aiScene *pScene, const std::string &Filename);

    void AssimpProcessNodes(const aiScene *, aiNode *node);

    void AssimpLoadMesh(aiMesh *mesh);

    void sendTrianglesToFrame(std::vector<Triangle *> *frameTriangles);
    //void shadowMapping(LightPoint3D *);
    //void setLightPoints(std::vector<LightPoint3D*> &);

    bool isShadowCaster() const;

    void setShadowCaster(bool shadow_caster);

    int getBspEntityIndex() const;

    void setBspEntityIndex(int bspEntityIndex);

    void updateBoundingBox();

    void copyFrom(Mesh3D *source);

    void onUpdate();

    const std::string &getSourceFile() const;

    void setSourceFile(const std::string &sourceFile);

    Octree *getOctree() const;

    void buildOctree();

    Grid3D *getGrid3D() const;

    void buildGrid3DForEmptyContainsStrategy(int sizeX, int sizeY, int sizeZ);

    void buildGrid3DForEmptyRayIntersectionStrategy(int sizeX, int sizeY, int sizeZ, Vertex3D direction);

    void buildGrid3DForEmptyDataImageStrategy(int sizeX, int sizeZ, const std::string& filename, int fixedY);

private:
    Octree *octree;
    Grid3D *grid;
};


#endif //SDL2_3D_ENGINE_MESH_H
