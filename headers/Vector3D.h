//
// Created by darkhead on 28/4/18.
//

#ifndef SDL2_3D_ENGINE_VECTOR3D_H
#define SDL2_3D_ENGINE_VECTOR3D_H

#include <SDL.h>
#include "Vertex.h"

class Vector3D {
public:
    Vector3D();

    Vertex vertex1;
    Vertex vertex2;

    Vertex componente;

    Vector3D(Vertex, Vertex);

    Vertex getComponent();
    Vertex getUnitVector();

    Vertex consoleInfo(std::string label);
};


#endif //SDL2_3D_ENGINE_VECTOR3D_H
