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

    return isAABBVisibleInVP(aabb, vp);
}

bool Frustum::isAABBVisibleInVP(AABB3D *aabb, const glm::mat4 &vp)
{
    // Extraccion de los 6 planos del frustum en espacio de mundo directamente de la matriz VP
    // (metodo Gribb-Hartmann), en vez de comparar clip-space contra +-w: esa comparacion directa
    // es incorrecta en perspectiva cuando w se vuelve negativo (geometria cerca de, o detras de,
    // el origen de la camara/luz) -- ver .claude/memory/lessons.md.
    glm::vec4 row0(vp[0][0], vp[1][0], vp[2][0], vp[3][0]);
    glm::vec4 row1(vp[0][1], vp[1][1], vp[2][1], vp[3][1]);
    glm::vec4 row2(vp[0][2], vp[1][2], vp[2][2], vp[3][2]);
    glm::vec4 row3(vp[0][3], vp[1][3], vp[2][3], vp[3][3]);

    glm::vec4 planes[6] = {
        row3 + row0, // left
        row3 - row0, // right
        row3 + row1, // bottom
        row3 - row1, // top
        row3 + row2, // near
        row3 - row2, // far
    };

    for (const auto& p : planes) {
        int outsideCount = 0;
        for (const auto& v : aabb->vertices) {
            float d = p.x * v.x + p.y * v.y + p.z * v.z + p.w;
            if (d < 0.0f) outsideCount++;
        }
        if (outsideCount == 8) return false;
    }

    return true;
}
