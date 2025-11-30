
#ifndef BRAKEDA3D_MATHS_H
#define BRAKEDA3D_MATHS_H

#include "../Render/Plane.h"
#include "../3D/Vector3D.h"
#include "../3D/Object3D.h"
#include "../Components/Camera3D.h"
#include "../3D/LightPoint.h"
#include "../Render/Triangle3D.h"

class ToolsMaths {
public:
    static float DegreesToRadians(float);
    static float RadiansToDegrees(float);
    static int isVector3DClippingPlane(const Plane &P, const Vector3D &V);
    static bool isTriangleIntersectingAABB(const Triangle &triangle, const AABB3D &box);
    static bool isSameSide(const Vertex3D &p1, const Vertex3D &p2, const Vertex3D &a, const Vertex3D &b);
    static bool PointInTriangle(const Vertex3D &p, const Vertex3D &a, const Vertex3D &b, const Vertex3D &c);
    static float distanceBetweenVertices(const Vertex3D &v1, const Vertex3D &v2);
    static float getHorizontalAngleBetweenObject3DAndCamera(Object3D *object, Camera3D *cam);
    static float TriangleArea(float x0, float y0, float x1, float y1, float x2, float y2);
    static float NormalizeToRange(float value, float min, float max);
    static float sqrt1(const float &n);
    static float Interpolate(float val, float bound_left, float bound_right);
    static float Clamp(float n, float lower, float upper);
    static float Percentage(int value, int total);

    static Vertex3D getHalfwayVector(const Vertex3D &a, const Vertex3D &b);
    static glm::mat3 RotateOnAxis(const glm::mat3 &originalRotation, float angle, glm::vec3 axis);
};


#endif //BRAKEDA3D_MATHS_H
