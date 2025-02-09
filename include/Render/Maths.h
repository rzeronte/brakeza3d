
#ifndef BRAKEDA3D_MATHS_H
#define BRAKEDA3D_MATHS_H


#include "Plane.h"
#include "../Objects/Vector3D.h"
#include "../Objects/Object3D.h"
#include "../Objects/Point2D.h"
#include "../Components/Camera3D.h"
#include "../Objects/LightPoint3D.h"
#include "../Objects/Triangle3D.h"
#include "../Misc/Image.h"

class Maths {
public:
    static float degreesToRadians(float);

    static float radiansToDegrees(float);

    static int isVector3DClippingPlane(Plane &P, Vector3D &V);

    static float distanceBetweenVertices(Vertex3D v1, Vertex3D v2);

    static float getHorizontalAngleBetweenObject3DAndCamera(Object3D *object, Camera3D *cam);

    static bool sameSide(Vertex3D p1, Vertex3D p2, Vertex3D a, Vertex3D b);

    static bool PointInTriangle(Vertex3D p, Vertex3D a, Vertex3D b, Vertex3D c);

    static float TriangleArea(float x0, float y0, float x1, float y1, float x2, float y2);

    static float normalizeToRange(float value, float min, float max);

    static float sqrt1(const float &n);

    static Vertex3D getHalfwayVector(Vertex3D a, Vertex3D b);

    static glm::mat3 RotateOnAxis(glm::mat3 originalRotation, float angle, glm::vec3 axis);

    static bool isTriangleIntersectingAABB(Triangle &triangle, AABB3D &box);
};


#endif //BRAKEDA3D_MATHS_H
