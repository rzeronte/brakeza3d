#include "../../include/Render/Frustum.h"
#include "../../include/Misc/ToolsMaths.h"
#include "../../include/Components/Components.h"

Frustum::Frustum() = default;

bool Frustum::isVertexInside(Vertex3D &v)
{
    auto camera = Components::get()->Camera();
    glm::vec4 clipSpacePos = camera->getGLMMat4ProjectionMatrix() * camera->getGLMMat4ViewMatrix() * glm::vec4(v.toGLM(), 1);

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
        if (!isVertexInside(vertice)) {
            return false;
        }
    }
    return true;
}

bool Frustum::isAABBVisibleInFrustum(AABB3D *aabb)
{
    auto camera = Components::get()->Camera();
    glm::mat4 vp = camera->getGLMMat4ProjectionMatrix() * camera->getGLMMat4ViewMatrix();

    glm::vec4 c[8];
    for (int i = 0; i < 8; i++)
        c[i] = vp * glm::vec4(aabb->vertices[i].toGLM(), 1.0f);

    // Cull if all 8 corners are outside the same clip plane
    auto allOutside = [&](bool (*test)(const glm::vec4&)) {
        for (const auto& v : c) if (!test(v)) return false;
        return true;
    };

    if (allOutside([](const glm::vec4& v){ return v.x < -v.w; })) return false;
    if (allOutside([](const glm::vec4& v){ return v.x >  v.w; })) return false;
    if (allOutside([](const glm::vec4& v){ return v.y < -v.w; })) return false;
    if (allOutside([](const glm::vec4& v){ return v.y >  v.w; })) return false;
    if (allOutside([](const glm::vec4& v){ return v.z < -v.w; })) return false;
    if (allOutside([](const glm::vec4& v){ return v.z >  v.w; })) return false;

    return true;
}
