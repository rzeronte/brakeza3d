
#ifndef SDL2_3D_ENGINE_VECTOR3D_H
#define SDL2_3D_ENGINE_VECTOR3D_H

#include <SDL.h>
#include "Vertex3D.h"

class Vector3D {
public:
    Vector3D();

    Vertex3D vertex1;
    Vertex3D vertex2;

    Vector3D(Vertex3D, Vertex3D);

    Vertex3D getComponent();
    Vertex3D getUnitVector();
};


#endif //SDL2_3D_ENGINE_VECTOR3D_H
