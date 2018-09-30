
#ifndef SDL2_3D_ENGINE_PLANE_H
#define SDL2_3D_ENGINE_PLANE_H


#include "Vertex.h"

class Plane {
public:
    Vertex A;
    Vertex B;
    Vertex C;

    Plane(const Vertex A, const Vertex B, const Vertex C);
    Plane();

    float distance(Vertex p);
    Vertex getNormalVector();

    Vertex getPointIntersection(Vertex v1, Vertex v2);
};


#endif //SDL2_3D_ENGINE_PLANE_H
