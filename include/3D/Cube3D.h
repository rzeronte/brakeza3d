/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEDA3D_CUBE3D_H
#define BRAKEDA3D_CUBE3D_H

#include "Vertex3D.h"
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
    Vertex3D vertices[36];
    Plane planes[6];
    float scaleX, scaleY, scaleZ;
public:

    Cube3D(float scale);
    Cube3D(float scaleX, float scaleY, float scaleZ);
    void updateGeometry();
    void makeVertices();
    void makeTriangles();
    void makePlanes();
    bool isPointInside(const Vertex3D &v) const;
    void onUpdate() override;
    void postUpdate() override;
};


#endif //BRAKEDA3D_CUBE3D_H
