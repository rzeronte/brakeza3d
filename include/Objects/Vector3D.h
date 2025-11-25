
#ifndef SDL2_3D_ENGINE_VECTOR3D_H
#define SDL2_3D_ENGINE_VECTOR3D_H

#include <SDL2/SDL.h>
#include "Vertex3D.h"

class Vector3D {
public:
    Vertex3D vertex1;
    Vertex3D vertex2;

    Vector3D();
    Vector3D(Vertex3D &, Vertex3D &);

    void clear();
    [[nodiscard]] Vertex3D getComponent() const;
    [[nodiscard]] Vertex3D normal() const;
    [[nodiscard]] Vertex3D origin() const;
    [[nodiscard]] Vertex3D end() const;
    [[nodiscard]] Vertex3D end(float) const;
    [[nodiscard]] Vertex3D middlePoint() const;
};


#endif //SDL2_3D_ENGINE_VECTOR3D_H
