
#ifndef SDL2_3D_ENGINE_MESH_H
#define SDL2_3D_ENGINE_MESH_H

#include <string>
#include "Vertex3D.h"
#include "Triangle3D.h"
#include "../Render/Tools.h"
#include "../Render/EngineSetup.h"
#include "Object3D.h"
#include "LightPoint3D.h"

#define MAX_MODEL_TRIANGLES 100000
#define MAX_VERTEX_MODEL 100000
#define MAX_MESH_TEXTURES 100

class Mesh3D : public Object3D {

public:
    std::string mesh_file;

    Texture *model_textures;
    Triangle *model_triangles;
    Vertex3D *model_vertex;

    vec3_t *obj_uv_list;

    int n_triangles;
    int n_vertex;
    int n_textures;

    LightPoint3D **lightPoints;
    int numberLightPoints;

    bool shadowCaster;

    bool isShadowCaster() const;
    void setShadowCaster(bool shadow_caster);

    Mesh3D();

    // OBJ
    bool loadOBJBlender(const char *name);
    void loadOBJBlenderVertex();
    void loadOBJBlenderTriangles();
    void loadOBJBlenderTextureCoordinates();
    void loadOBJBlenderMaterials();

    void draw(Camera3D *);
    void shadowMapping(LightPoint3D *);
    void setLightPoints(LightPoint3D **lightPoint, int);
};


#endif //SDL2_3D_ENGINE_MESH_H
