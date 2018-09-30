
#ifndef SDL2_3D_ENGINE_RENDER_H
#define SDL2_3D_ENGINE_RENDER_H


#include "Vertex.h"
#include "Camera.h"
#include "Point2D.h"
#include "Texture.h"
#include "LightPoint.h"

class Render {
public:

    static Vertex objectToLocal(Vertex V, Object3D *o);

    static bool triangulate(Vertex *vertexes, int num_vertex, Object3D *parent, SDL_Surface *screen, Camera *cam, Vertex A, Vertex B, Vertex C, Texture *texture);

    static Vertex objectSpace(Vertex A, Object3D *o);
    static Vertex homogeneousClipSpace(Vertex v, Camera *cam);
    static Vertex cameraSpace(Vertex V, Camera *cam);

    static Vertex screenSpacePerspective(Vertex v, Camera *cam);
    static Point2D screenSpace(Vertex, Camera *cam);

    static bool isVector3DClippingPlane(Plane P, Vector3D V);

    static float barycentricSide(int x, int y, Point2D pa, Point2D pb);
    static void getBarycentricCoordinates(float &, float &, float &, int, int, Point2D, Point2D, Point2D);

    static Vertex rotateVertex(Vertex V, Vertex rotation);

};


#endif //SDL2_3D_ENGINE_RENDER_H
