#include "../../include/Render/Frustum.h"
#include "../../include/EngineSetup.h"
#include "../../include/Render/Maths.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Render/Logging.h"

Frustum::Frustum()
{
}

bool Frustum::isVertexInside(Vertex3D &v) {
    EngineSetup *setup = EngineSetup::get();

    for (int i = setup->FAR_PLANE; i <= setup->BOTTOM_PLANE; i++) {
        if (planes[i].distance(v) >= EngineSetup::get()->FRUSTUM_CLIPPING_DISTANCE) {
            return false;
        }
    }

    return true;
}

bool Frustum::isAABBInFrustum(AABB3D *aabb) {
    for(auto & vertice : aabb->vertices) {
        if (!this->isVertexInside(vertice)) {
            return false;
        }
    }
    return true;
}
