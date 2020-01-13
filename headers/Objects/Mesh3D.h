
#ifndef SDL2_3D_ENGINE_MESH_H
#define SDL2_3D_ENGINE_MESH_H

#include <string>
#include "Vertex3D.h"
#include "Triangle3D.h"
#include "../Misc/Tools.h"
#include "../EngineSetup.h"
#include "Object3D.h"
//#include "LightPoint3D.h"

#define MAX_VERTEX_MODEL 10000
#define MAX_MESH_TEXTURES 100

typedef float vec3_t[3];

class Mesh3D : public Object3D {

public:
    std::string mesh_file;

    std::vector<Triangle*> modelTriangles;

    Texture *modelTextures;
    Vertex3D *modelVertices;

    vec3_t *verticesTextureCoordsList;

    int numVertices;
    int numTextures;

    //std::vector<LightPoint3D *>lightPoints;

    bool shadowCaster;
    int BSPEntityIndex;

    Vertex3D aabbMin;
    Vertex3D aabbMax;

    Mesh3D();

    // OBJ
    bool loadOBJBlender(const char *name);
    void loadOBJBlenderVertex();
    void loadOBJBlenderTriangles();
    void loadOBJBlenderTextureCoordinates();
    void loadOBJBlenderMaterials();

    void draw(std::vector<Triangle*> *frameTriangles);
    //void shadowMapping(LightPoint3D *);
    //void setLightPoints(std::vector<LightPoint3D*> &);

    bool isShadowCaster() const;
    void setShadowCaster(bool shadow_caster);

    int getBspEntityIndex() const;
    void setBspEntityIndex(int bspEntityIndex);
};


#endif //SDL2_3D_ENGINE_MESH_H
