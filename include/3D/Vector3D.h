/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */

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
