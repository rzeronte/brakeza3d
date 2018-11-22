
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


class TriangleRunVerticesType {
public:
    const static int RUN_FOR_VIDEOBUFFER = 1;
    const static int RUN_FOR_SHADOW_MAPPING = 2;
};

class Triangle {

public:
    Vertex3D A, B, C;
    Vertex3D Ao, Bo, Co;
    Vertex3D Ac, Bc, Cc;

    Texture *texture;
    Object3D *parent;

    int order = 0;
    bool is_clipped = false;

    LightPoint3D **lightPoints;
    int numberLightPoints = 0;

    Triangle();
    Triangle(Vertex3D A, Vertex3D B, Vertex3D C, Object3D *parent);

    void updateVertexObjectSpace();
    void updateVertexCameraSpace(Camera3D *cam);

    // Check camera - face culling
    bool faceCulling(Object3D *obj);

    Vertex3D getNormal();
    Vertex3D getCenter();

    // Rasterization
    void drawWireframe(Camera3D *cam);
    bool draw(Camera3D *);
    void drawNormal(Camera3D *cam, Uint32 color);

    void shadowMapping(LightPoint3D *lp);

    void scanVertices(Camera3D *);
    void scanBottomFlatTriangle(Point2D, Point2D, Point2D, Vertex3D, Vertex3D, Vertex3D, Vertex3D, Vertex3D, Vertex3D);
    void scanTopFlatTriangle(Point2D, Point2D, Point2D, Vertex3D, Vertex3D, Vertex3D, Vertex3D, Vertex3D, Vertex3D);
    void scanLine(float x1 , float x2 , int y, Point2D, Point2D, Point2D, Vertex3D, Vertex3D, Vertex3D, Vertex3D, Vertex3D, Vertex3D);

    void scanVerticesForShadowMapping(LightPoint3D *lp);
    void scanShadowMappingBottomFlatTriangle(Point2D, Point2D, Point2D, Vertex3D, Vertex3D, Vertex3D, LightPoint3D *lp);
    void scanShadowMappingTopFlatTriangle(Point2D, Point2D, Point2D, Vertex3D, Vertex3D, Vertex3D, LightPoint3D *lp);
    void scanShadowMappingLine(float x1 , float x2 , int y, Point2D, Point2D, Point2D, Vertex3D, Vertex3D, Vertex3D, LightPoint3D *lp);

    // texture
    Texture *getTexture() const;
    void setTexture(Texture *texture);

    // clipping triangle
    bool clipping(Camera3D *cam);
    void clippingPlane(Camera3D *cam, int id_plane, Vertex3D *, int &);

    void sortVertexClockWise(Vertex3D vertexes[], int num_vertex);

    void setLightPoints(LightPoint3D **lightPoints, int number);

    void setClipped(bool);
    bool isClipped();

    bool triangulate(Vertex3D [], int num_vertex, Object3D *parent, Camera3D *cam, Vertex3D A, Vertex3D B, Vertex3D C, Texture *texture, Triangle *triangles, int &);

};

#endif //SDL2_3D_ENGINE_TRIANGLE_H
