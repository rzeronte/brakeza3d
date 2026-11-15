/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */

#ifndef SDL2_3D_ENGINE_TRIANGLE_H
#define SDL2_3D_ENGINE_TRIANGLE_H

#include "../3D/Vertex3D.h"
#include "Point2D.h"
#include "../3D/Object3D.h"

class Triangle {

public:
    Vertex3D A, B, C;
    Vertex3D Ao, Bo, Co;
    Vertex3D An, Bn, Cn;
    Point2D As, Bs, Cs;
    Object3D *parent = nullptr;
    Vertex3D normal;

    bool enableLights = false;
    bool clipped = false;

    Triangle() = default;
    Triangle(Vertex3D A, Vertex3D B, Vertex3D C, Object3D *parent);

    void updateObjectSpace();
    void updateNormal();
    void setEnableLights(bool enableLights);

    [[nodiscard]] bool isPointInside(Vertex3D) const;
    [[nodiscard]] Vertex3D getNormal() const;
    [[nodiscard]] bool isEnableLights() const;
};

#endif //SDL2_3D_ENGINE_TRIANGLE_H
