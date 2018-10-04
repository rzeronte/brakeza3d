
#ifndef SDL2_3D_ENGINE_MESH_H
#define SDL2_3D_ENGINE_MESH_H

#include <string>
#include "Vertex.h"
#include "Triangle.h"
#include "Tools.h"
#include "Q3Loader.h"
#include "EngineSetup.h"
#include "Object3D.h"
#include "LightPoint.h"

#define MAX_MODEL_TRIANGLES 10000
#define MAX_VERTEX_MODEL 100000
#define MAX_MESH_TEXTURES 100

class Mesh : public Object3D {

public:
    TMapQ3 lMap;
    Billboard *billboard;

    std::string mesh_file;

    Texture *model_textures;
    Triangle *model_triangles;
    Vertex *model_vertex;

    int n_triangles;
    int n_vertex;
    int n_textures;

    LightPoint **lightPoints;
    int numberLightPoints;

    bool shadowCaster;

    bool isShadowCaster() const;
    void setShadowCaster(bool shadow_caster);

    Mesh();

    // OBJ
    bool loadOBJBlender(const char *name);
    void loadOBJBlenderVertex();
    void loadOBJBlenderTriangles();
    void loadOBJBlenderTextureCoordinates();
    void loadOBJBlenderMaterials();

    // BSP
    void loadQ3Map(const std::string &pFilename);
    void loadTrianglesFromQ3Map();
    void loadTexturesFromQ3Map();

    void draw(Camera *);
    void shadowMapping(LightPoint *);
    void setLightPoints(LightPoint **lightPoint, int);
};


#endif //SDL2_3D_ENGINE_MESH_H
