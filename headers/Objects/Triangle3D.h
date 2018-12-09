
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

class Triangle {

public:
    Vertex3D A, B, C;
    Vertex3D Ao, Bo, Co;
    Vertex3D Ac, Bc, Cc;
    Vertex3D An, Bn, Cn;
    Point2D As, Bs, Cs;

    Texture *texture;
    Object3D *parent;

    int order = 0;
    bool is_clipped = false;

    LightPoint3D **lightPoints;
    int numberLightPoints = 0;

    Triangle();
    Triangle(Vertex3D A, Vertex3D B, Vertex3D C, Object3D *parent);

    void updateVertexSpaces(Camera3D *cam);

    bool isBackFaceCulling(Camera3D *cam);

    Vertex3D getNormal();
    Vertex3D getCenter();

    void drawWireframe(Camera3D *cam);
    bool draw(Camera3D *);
    void drawNormal(Camera3D *cam, Uint32 color);

    void shadowMapping(LightPoint3D *lp);

    void scanVertices(Camera3D *);
    void scanBottomFlatTriangle(Point2D, Point2D, Point2D);
    void scanTopFlatTriangle(Point2D, Point2D, Point2D);
    void scanLine(float x1 , float x2 , int y);
    void processPixel(Point2D);

    void scanVerticesForShadowMapping(LightPoint3D *lp);
    void scanShadowMappingBottomFlatTriangle(Point2D, Point2D, Point2D, Vertex3D, Vertex3D, Vertex3D, LightPoint3D *lp);
    void scanShadowMappingTopFlatTriangle(Point2D, Point2D, Point2D, Vertex3D, Vertex3D, Vertex3D, LightPoint3D *lp);
    void scanShadowMappingLine(float x1 , float x2 , int y, Point2D, Point2D, Point2D, Vertex3D, Vertex3D, Vertex3D, LightPoint3D *lp);

    Texture *getTexture() const;
    void setTexture(Texture *texture);

    bool clipping(Camera3D *cam);

    void setLightPoints(LightPoint3D **lightPoints, int number);

    void setClipped(bool);
    bool isClipped();


    int triangulate(int num_vertex, Vertex3D *vertices, Vertex3D normal, Triangle *triangle, int &ntriangles, Object3D *parent, Texture *texture, bool clipped);

};

#endif //SDL2_3D_ENGINE_TRIANGLE_H
