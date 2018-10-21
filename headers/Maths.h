
#ifndef BRAKEDA3D_MATHS_H
#define BRAKEDA3D_MATHS_H


#include "Plane.h"
#include "Vector3D.h"
#include "Object3D.h"
#include "Point2D.h"
#include "Camera.h"
#include "Texture.h"
#include "LightPoint.h"

class Maths
{
public:
    static float degreesToRadians(float);
    static float radiansToDegrees(float);

    static bool isVector3DClippingPlane(Plane P, Vector3D V);

    static bool triangulate(Vertex *vertexes, int num_vertex, Object3D *parent, Camera *cam, Vertex A, Vertex B, Vertex C, Texture *texture);

    static float barycentricSide(int x, int y, Point2D pa, Point2D pb);
    static void getBarycentricCoordinates(float &, float &, float &, int, int, Point2D, Point2D, Point2D);

    static Vertex rotateVertex(Vertex V, Vertex rotation);

    static Vertex getCenterVertices(Vertex vertices[], int num_vertices);

    static void sortVertexesByX(Vertex vertexes[], int N);
    static void sortVertexesByY(Vertex vertexes[], int N);

    static void sortVertexByY(Vertex &A, Vertex &B, Vertex &C);
    static void sortVertexByX(Vertex &A, Vertex &B, Vertex &C);

    static void sortVertexesByAngles(Vertex *vertexes, int N);

    static void sortPointsByY(Point2D &A, Point2D &B, Point2D &C);
    static void sortPointsByX(Point2D &A, Point2D &B, Point2D &C);

    static void VertexSwap(Vertex *vertexes, int i, int j);

    static float distanteBetweenpoints(Vertex v1, Vertex v2);

    static Uint32 mixColor(Uint32 color, float distance, LightPoint *lp, Vertex Q);

    static Vertex crossProduct(Vertex, Vertex);

    static float floatRound(double dval, int n);
    static float getHorizontalAngleBetweenObject3DAndCamera(Object3D *o1, Camera *cam);
};


#endif //BRAKEDA3D_MATHS_H
