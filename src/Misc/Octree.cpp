#include "../../headers/Misc/Octree.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/EngineSetup.h"

#define MAX_RECURSIVE_DEPTH 2

Octree::Octree(std::vector<Triangle*> &triangles, AABB3D bounds)
{
    this->root = this->BuildOctree(triangles, bounds, 0);
}

OctreeNode* Octree::BuildOctree(std::vector<Triangle*> &triangles, AABB3D bounds, int recursiveDepth)
{
    auto* node = new OctreeNode();
    node->triangles.resize(0);
    node->bounds = bounds;

    for (int j = 0; j < triangles.size(); j++) {
        if (isTriangleInsideAABB(triangles[j], bounds)) {
            node->triangles.push_back(triangles[j]);
        }
    }

    if (recursiveDepth >= MAX_RECURSIVE_DEPTH) {
        for (int i = 0; i < 8; i++) {
            node->children[i] = NULL;
        }
        return node;
    }

    Vertex3D childDimensions = (bounds.max - bounds.min).getScaled(0.5);

    Logging::getInstance()->Log("OctreeNode: (" + std::to_string(recursiveDepth) + ") = " + std::to_string(node->triangles.size()));

    for (int i = 0; i < 8; i++) {
        AABB3D childBounds;
        Vertex3D childOffset;

        switch (i) {
            case 0:
                childOffset = Vertex3D(0, 0, 0);
                break;
            case 1:
                childOffset = Vertex3D(childDimensions.x, 0, 0);
                break;
            case 2:
                childOffset = Vertex3D(childDimensions.x, childDimensions.y, 0);
                break;
            case 3:
                childOffset = Vertex3D(childDimensions.x, childDimensions.y, childDimensions.z);
                break;
            case 4:
                childOffset = Vertex3D(0, 0, childDimensions.z);
                break;
            case 5:
                childOffset = Vertex3D(0, childDimensions.y, childDimensions.z);
                break;
            case 6:
                childOffset = Vertex3D(0, childDimensions.y, 0);
                break;
            case 7:
                childOffset = Vertex3D(childDimensions.x, 0, childDimensions.z);
                break;
        }

        childBounds.min = bounds.min + childOffset;
        childBounds.max = bounds.min + childOffset + childDimensions;
        childBounds.updateVertices();

        node->children[i] = BuildOctree(
            node->triangles,
            childBounds,
            recursiveDepth + 1
        );
    }

    return node;
}

bool Octree::isTriangleInsideAABB(Triangle *triangle, AABB3D childBounds)
{
    std::vector<Plane> planes = childBounds.getPlanes();

    triangle->updateObjectSpace();

    bool r1 = this->isVertexInsideAABB(triangle->Ao, planes);
    bool r2 = this->isVertexInsideAABB(triangle->Bo, planes);
    bool r3 = this->isVertexInsideAABB(triangle->Co, planes);

    if (r1 || r2 || r3 ) {
        return true;
    }

    return false;
}

bool Octree::isVertexInsideAABB(Vertex3D v, std::vector<Plane> planes)
{
    int plane_init = 0;
    int plane_end = 6;

    for (int i = plane_init; i < plane_end; i++) {
        Plane p = planes[i];
        float d = p.distance(v);
        if (d >= EngineSetup::getInstance()->FRUSTUM_CLIPPING_DISTANCE) {
            return false;
        }
    }

    return true;
}