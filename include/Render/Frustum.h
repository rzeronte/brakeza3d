
#ifndef SDL2_3D_ENGINE_FRUSTUM_H
#define SDL2_3D_ENGINE_FRUSTUM_H

#include "../Objects/Vertex3D.h"
#include "../Objects/Object3D.h"
#include "../Objects/Vector3D.h"
#include "Plane.h"
#include "AABB3D.h"

// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-extracting-the-planes/
class Frustum : public Object3D {
public:
    Vertex3D direction;   // Normalized
    Vertex3D up;
    Vertex3D right;

    Plane planes[6];

    AABB3D bounds;

public:
    Frustum();

    bool isVertexInside(Vertex3D &v);

    bool isAABBInFrustum(AABB3D *aabb);
};


#endif //SDL2_3D_ENGINE_FRUSTUM_H
