#include "../../include/Render/Frustum.h"
#include "../../include/Render/Maths.h"
#include "../../include/ComponentsManager.h"

Frustum::Frustum() = default;

bool Frustum::isVertexInside(Vertex3D &v)
{
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    glm::vec4 clipSpacePos = Camera3D::getProjectionMatrix() * camera->getViewMatrix() * glm::vec4(v.toGLM(), 1);

    if (clipSpacePos.x < -clipSpacePos.w || clipSpacePos.x > clipSpacePos.w ||
        clipSpacePos.y < -clipSpacePos.w || clipSpacePos.y > clipSpacePos.w ||
        clipSpacePos.z < -clipSpacePos.w || clipSpacePos.z > clipSpacePos.w) {
        return false;
    }

    return true;
}

bool Frustum::isAABBInFrustum(AABB3D *aabb)
{
    for(auto & vertice : aabb->vertices) {
        if (!Frustum::isVertexInside(vertice)) {
            return false;
        }
    }
    return true;
}
