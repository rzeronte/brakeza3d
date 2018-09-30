
#ifndef SDL2_3D_ENGINE_TRIANGLE_H
#define SDL2_3D_ENGINE_TRIANGLE_H

#include "Vertex.h"
#include "Vector3D.h"
#include "Camera.h"
#include "Point2D.h"
#include "Texture.h"
#include "Object3D.h"
#include "LightPoint.h"
#include <list>


class TriangleRunVerticesType {
public:
    const static int RUN_FOR_VIDEOBUFFER = 1;
    const static int RUN_FOR_SHADOW_MAPPING = 2;
};

class Triangle {

public:
    Vertex A, B, C;
    Vertex Ao, Bo, Co;
    Vertex Ac, Bc, Cc;

    Texture *texture;
    Object3D *parent;

    int order = 0;

    LightPoint **lightPoints;
    int numberLightPoints = 0;

    Triangle();
    Triangle(Vertex A, Vertex B, Vertex C, Object3D *parent);

    void updateVertexObjectSpace();
    void updateVertexCameraSpace(Camera *cam);

    // Check camera - face culling
    bool faceCulling(Object3D *obj);
    void drawNormal(SDL_Surface *screen, Camera *cam, Uint32 color);
    Vertex getNormal();
    Vertex getCenter();
    Vertex calcNormalSurface(Vector3D, Vector3D);

    // Rasterization
    void drawWireframe(SDL_Surface *screen,Camera *cam);
    bool draw(SDL_Surface *, Camera *);
    void shadowMapping(LightPoint *lp);

    void scanVertices(Camera *, SDL_Surface *);
    void scanBottomFlatTriangle(Point2D, Point2D, Point2D, Vertex, Vertex, Vertex, Vertex, Vertex, Vertex, Camera*, SDL_Surface *);
    void scanTopFlatTriangle(Point2D, Point2D, Point2D, Vertex, Vertex, Vertex, Vertex, Vertex, Vertex, Camera*, SDL_Surface *);
    void scanLine(float x1 , float x2 , int y, Point2D, Point2D, Point2D, Vertex, Vertex, Vertex, Vertex, Vertex, Vertex, Camera*, SDL_Surface *);

    void scanVerticesForShadowMapping(LightPoint *lp);
    void scanShadowMappingBottomFlatTriangle(Point2D, Point2D, Point2D, Vertex, Vertex, Vertex, LightPoint *lp);
    void scanShadowMappingTopFlatTriangle(Point2D, Point2D, Point2D, Vertex, Vertex, Vertex, LightPoint *lp);
    void scanShadowMappingLine(float x1 , float x2 , int y, Point2D, Point2D, Point2D, Vertex, Vertex, Vertex, LightPoint *lp);

    // texture
    Texture *getTexture() const;
    void setTexture(Texture *texture);

    // debug helpers
    void consoleInfo(const char*);

    // clipping triangle
    bool clipping(SDL_Surface *screen,Camera *cam);

    void setLightPoints(LightPoint **lightPoints, int number);
};

#endif //SDL2_3D_ENGINE_TRIANGLE_H
