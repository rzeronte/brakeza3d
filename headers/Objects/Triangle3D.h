
#ifndef SDL2_3D_ENGINE_TRIANGLE_H
#define SDL2_3D_ENGINE_TRIANGLE_H

#include "Vertex3D.h"
#include "Vector3D.h"
#include "Camera3D.h"
#include "Point2D.h"
#include "../Render/Texture.h"
#include "Object3D.h"
#include "LightPoint3D.h"
#include <list>
#include "../Render/EngineBuffers.h"

class Triangle {

public:
    int id;
    bool drawed = false;

    Vertex3D A, B, C;
    Vertex3D Ao, Bo, Co;
    Vertex3D Ac, Bc, Cc;
    Vertex3D An, Bn, Cn;
    Point2D As, Bs, Cs;

    Vertex3D Ae, Be, Ce;

    float fullArea;
    float reciprocalFullArea;

    Texture *texture;
    Texture *lightmap;

    // lightmap coordinates
    float light_u1, light_v1;
    float light_u2, light_v2;
    float light_u3, light_v3;

    // texture vertex coordinates
    float tex_u1, tex_v1;
    float tex_u2, tex_v2;
    float tex_u3, tex_v3;

    // texture & lightmaps pre-cached data
    float light_u1_Ac_z, light_u2_Bc_z, light_u3_Cc_z;
    float light_v1_Ac_z, light_v2_Bc_z, light_v3_Cc_z;
    float tex_u1_Ac_z,   tex_u2_Bc_z,   tex_u3_Cc_z;
    float tex_v1_Ac_z,   tex_v2_Bc_z,   tex_v3_Cc_z;
    float persp_correct_Az, persp_correct_Bz, persp_correct_Cz;

    int maxX, minX, maxY, minY;

    Object3D *parent;

    Vertex3D normal;

    int order = 0;
    bool is_clipped = false;

    bool is_bsp = false;
    int bsp_surface = -1;

    int lod;

    LightPoint3D **lightPoints;
    int numberLightPoints = 0;

    Triangle();
    Triangle(Vertex3D A, Vertex3D B, Vertex3D C, Object3D *parent);

    void updateFullVertexSpaces(Camera3D *cam);

    void updateObjectSpace();
    void updateCameraSpace(Camera3D *cam);
    void updateNDCSpace(Camera3D *cam);
    void updateScreenSpace(Camera3D *cam);

    void updateUVCache();

    bool isBackFaceCulling(Camera3D *cam);

    void softwareRasterizer();
    void softwareRasterizerForTile(int minX, int minY, int maxX, int maxY);

    Vertex3D getNormal();
    void     updateNormal();
    Vertex3D getCenterOfMass();

    void drawWireframe();
    void drawWireframeColor(Uint32 c);

    void draw(Camera3D *);
    void drawForTile(Camera3D *cam, int minX, int minY, int maxX, int maxY);

    void drawNormal(Camera3D *cam, Uint32 color);

    void shadowMapping(LightPoint3D *lp);

    void processPixel(int, int, int, float, float, float, float, float, float, float, float);
    Uint32 processPixelTexture(float, float);
    Uint32 processPixelLightmap(Uint32, float, float);

    void scanVerticesForShadowMapping(LightPoint3D *lp);
    void scanShadowMappingBottomFlatTriangle(Point2D, Point2D, Point2D, Vertex3D, Vertex3D, Vertex3D, LightPoint3D *lp);
    void scanShadowMappingTopFlatTriangle(Point2D, Point2D, Point2D, Vertex3D, Vertex3D, Vertex3D, LightPoint3D *lp);
    void scanShadowMappingLine(float x1 , float x2 , int y, Point2D, Point2D, Point2D, Vertex3D, Vertex3D, Vertex3D, LightPoint3D *lp);

    Texture *getTexture() const;
    void setTexture(Texture *texture);

    Texture *getLightmap() const;
    void setLightmap(Texture *texture);

    bool clipping(Camera3D *cam, Triangle*, int&);
    void setClipped(bool);
    bool isClipped();
    bool testForClipping(Camera3D *cam);

    void setLightPoints(LightPoint3D **lightPoints, int number);

    bool isPointInside(Vertex3D);

    int processLOD();
    int getLOD();

    float updateFullArea();
    void updateBoundingBox();

    OCLTriangle getOpenCL();

    int getId() const;
    void setId(int id);

    Plane plane();

};

#endif //SDL2_3D_ENGINE_TRIANGLE_H
