/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */

#ifndef SDL2_3D_ENGINE_FRUSTUM_H
#define SDL2_3D_ENGINE_FRUSTUM_H

#include "../3D/Vertex3D.h"
#include "Plane.h"
#include "AABB3D.h"
#include "../3D/Object3D.h"

// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-extracting-the-planes/
class Frustum : public Object3D {
public:
    Vertex3D direction;   // Normalized
    Vertex3D up;
    Vertex3D right;

    Plane planes[6];

    AABB3D bounds;

    Frustum();
    static bool isVertexInside(Vertex3D &v);
    static bool isAABBInFrustum(AABB3D *aabb);
    static bool isAABBVisibleInFrustum(AABB3D *aabb);
    static bool isAABBVisibleInVP(AABB3D *aabb, const glm::mat4 &vp);
};


#endif //SDL2_3D_ENGINE_FRUSTUM_H
