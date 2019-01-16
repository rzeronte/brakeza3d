
#ifndef SDL2_3D_ENGINE_PLANE_H
#define SDL2_3D_ENGINE_PLANE_H


#include "../Objects/Vertex3D.h"

class Plane {
public:
    Vertex3D A;
    Vertex3D B;
    Vertex3D C;

    Vertex3D normal;

    Plane(const Vertex3D A, const Vertex3D B, const Vertex3D C);
    Plane(const Vertex3D P, const Vertex3D N);
    Plane();

    float distance(Vertex3D &p);

    Vertex3D getNormalVector();
    void     updateNormalVector();

    Vertex3D getPointIntersection(Vertex3D v1, Vertex3D v2, float &transition);
    bool isFrontFacingTo(Vertex3D direction);

};


#endif //SDL2_3D_ENGINE_PLANE_H
