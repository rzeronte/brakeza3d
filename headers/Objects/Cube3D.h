#ifndef BRAKEDA3D_CUBE3D_H
#define BRAKEDA3D_CUBE3D_H

#include "Vertex3D.h"
#include "Triangle3D.h"
#include "Mesh3D.h"

static const float cubeVertexBuffer[] = {
        -1.0f, -1.0f, -1.0f, // triángulo 1 : comienza
        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triángulo 1 : termina
        1.0f, 1.0f, -1.0f, // triángulo 2 : comienza
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f, // triángulo 2 : termina
        1.0f, -1.0f, 1.0f,  // triángulo 3 : comienza
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,  // triángulo 3 : termina
        1.0f, 1.0f, -1.0f,  // ...
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
};

class Cube3D : public Mesh3D {
public:
    Vertex3D vertices[36];
    Plane planes[6];
    float scaleX, scaleY, scaleZ;

    Cube3D(float scale);

    Cube3D(float scaleX, float scaleY, float scaleZ);

    void updateGeometry();

    void makeVertices();

    void makeTriangles();

    void makePlanes();

    bool isPointInside(Vertex3D v);
};


#endif //BRAKEDA3D_CUBE3D_H
