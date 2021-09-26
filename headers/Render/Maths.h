
#ifndef BRAKEDA3D_MATHS_H
#define BRAKEDA3D_MATHS_H


#include "Plane.h"
#include "../Objects/Vector3D.h"
#include "../Objects/Object3D.h"
#include "../Objects/Point2D.h"
#include "../Components/Camera3D.h"
#include "Texture.h"
#include "../Objects/LightPoint3D.h"
#include "../Objects/Triangle3D.h"

class Maths {
public:
    static float degreesToRadians(float);

    static float radiansToDegrees(float);

    static int isVector3DClippingPlane(Plane &P, Vector3D &V);

    static float barycentricSide(int x, int y, Point2D pa, Point2D pb);

    static void getBarycentricCoordinates(float &, float &, float &, int, int, Point2D, Point2D, Point2D);

    static void
    getBarycentricCoordinatesPrecalc(float &, float &, float &, int, int, Point2D, Point2D, Point2D, float, float,
                                     float);

    static int orient2d(const Point2D &a, const Point2D &b, const Point2D &c);

    static Vertex3D getCenterVertices(Vertex3D vertices[], int num_vertices);

    static void sortVerticesByX(Vertex3D *vertexes, int N);

    static void sortVerticesByY(Vertex3D *vertexes, int N);

    static void sortVerticesByY(Vertex3D &A, Vertex3D &B, Vertex3D &C);

    static void sortVerticesByX(Vertex3D &A, Vertex3D &B, Vertex3D &C);


    static void sortPointsByY(Point2D &A, Point2D &B, Point2D &C);

    static void sortPointsByX(Point2D &A, Point2D &B, Point2D &C);

    static void VertexSwap(Vertex3D *vertexes, int i, int j);

    static float distanceBetweenVertices(Vertex3D v1, Vertex3D v2);

    static float getHorizontalAngleBetweenObject3DAndCamera(Object3D *object, Camera3D *cam);

    static long GetNextActive(long x, long vertexCount, const bool *active);

    static long GetPrevActive(long x, long vertexCount, const bool *active);

    static int
    TriangulatePolygon(long vertexCount, Vertex3D *vertices, Vertex3D normal, std::vector<Triangle *> &triangles,
                       Object3D *parent, Texture *texture, Texture *lightmap, bool isClipped, bool isBsp,
                       unsigned char[4]);

    static bool
    ClippingPolygon(Vertex3D *input, int numInput, Vertex3D *output, int &numOutput, int plane_id, Plane *planes);

    static float distancePointVector(Vertex3D, Vector3D);

    static bool sameSide(Vertex3D p1, Vertex3D p2, Vertex3D a, Vertex3D b);

    static bool PointInTriangle(Vertex3D p, Vertex3D a, Vertex3D b, Vertex3D c);

    static float TriangleArea(float x0, float y0, float x1, float y1, float x2, float y2);

    static float normalizeToRange(float value, float min, float max);

    static float sqrt1(const float &n);
};


#endif //BRAKEDA3D_MATHS_H
