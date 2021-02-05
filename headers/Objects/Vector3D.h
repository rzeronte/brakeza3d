
#ifndef SDL2_3D_ENGINE_VECTOR3D_H
#define SDL2_3D_ENGINE_VECTOR3D_H

#include <SDL.h>
#include "Vertex3D.h"

class Vector3D {
public:
    Vector3D();

    Vertex3D vertex1;
    Vertex3D vertex2;

    Vector3D(Vertex3D &, Vertex3D &);

    Vertex3D getComponent() const;
    Vertex3D normal() const;

    Vertex3D origin() const;
    Vertex3D end() const;
    Vertex3D end(float) const;
    Vertex3D middlePoint() const;
    void clear();

};


#endif //SDL2_3D_ENGINE_VECTOR3D_H
