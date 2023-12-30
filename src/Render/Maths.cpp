
#include "../../include/Render/Maths.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Render/Transforms.h"

float Maths::degreesToRadians(float angleDegrees) {
    return angleDegrees * (float) M_PI / (float) 180.0;
}

float Maths::radiansToDegrees(float angleRadians) {
    return angleRadians * (float) 180.0 / (float) M_PI;
}

/**
 * 0 = dos vértices dentro
 * 1 = ningún vértice dentro
 * 2 = vértice A dentro
 * 3 = vértice B dentro
 */
int Maths::isVector3DClippingPlane(Plane &P, Vector3D &V) {
    EngineSetup *SETUP = EngineSetup::get();

    float distanceV1 = P.distance(V.vertex1);
    float distanceV2 = P.distance(V.vertex2);

    if (distanceV1 > SETUP->FRUSTUM_CLIPPING_DISTANCE && distanceV2 > SETUP->FRUSTUM_CLIPPING_DISTANCE) {
        return 1;
    }

    if (distanceV2 > SETUP->FRUSTUM_CLIPPING_DISTANCE && distanceV1 < SETUP->FRUSTUM_CLIPPING_DISTANCE) {
        return 2;
    }

    if (distanceV1 > SETUP->FRUSTUM_CLIPPING_DISTANCE && distanceV2 < SETUP->FRUSTUM_CLIPPING_DISTANCE) {
        return 3;
    }

    return 0;
}

float Maths::distanceBetweenVertices(Vertex3D v1, Vertex3D v2) {
    return sqrtf((v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y) + (v2.z - v1.z) * (v2.z - v1.z));
}

float Maths::getHorizontalAngleBetweenObject3DAndCamera(Object3D *object, Camera3D *cam) {
    Vertex3D a = cam->getPosition() - object->getPosition();
    Vertex3D b = object->getRotation() * EngineSetup::get()->forward;

    a = a.getNormalize();

    float theta = acos(a * b);
    theta = Maths::radiansToDegrees(theta);

    if (a.x * a.z < 0) {
        theta = 360.0f - theta;
    }

    return theta;
}

bool Maths::sameSide(Vertex3D p1, Vertex3D p2, Vertex3D a, Vertex3D b) {
    Vertex3D cp1 = (b - a) % (p1 - a);
    Vertex3D cp2 = (b - a) % (p2 - a);

    if ((cp1 * cp2) >= 0) {
        return true;
    }

    return false;
}

bool Maths::PointInTriangle(Vertex3D p, Vertex3D a, Vertex3D b, Vertex3D c) {
    return sameSide(p, a, b, c) && sameSide(p, b, a, c) && sameSide(p, c, a, b);
}

float Maths::TriangleArea(float dX0, float dY0, float dX1, float dY1, float dX2, float dY2) {
    float area = ((dX1 - dX0) * (dY2 - dY0) - (dX2 - dX0) * (dY1 - dY0)) / 2;

    return abs(area);
}

float Maths::normalizeToRange(float value, float min, float max) {
    if ((max - min) == 0) {
        return 0;
    }

    return (value - min) / (max - min);
}

float Maths::sqrt1(const float &n) {
    static union {
        int i;
        float f;
    } u;
    u.i = 0x5F375A86 - (*(int *) &n >> 1);
    return (int(3) - n * u.f * u.f) * n * u.f * 0.5f;
}

Vertex3D Maths::getHalfwayVector(Vertex3D a, Vertex3D b)
{
    return (a.getNormalize() + b.getNormalize()).getNormalize();
}

