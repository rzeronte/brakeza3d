#include "../../include/Misc/Octree.h"
#include "../../include/Render/Logging.h"

#define MAX_RECURSIVE_DEPTH 2

Octree::Octree(AABB3D bounds, int maxDepth)
:
    maxDepth(maxDepth)
{
    this->root = BuildOctree(bounds, 0);
}

OctreeNode *Octree::BuildOctree(AABB3D bounds, int recursiveDepth)
{
    auto *node = new OctreeNode();
    node->triangles.resize(0);
    node->bounds = bounds;

    if (recursiveDepth >= maxDepth) {
        for (auto & i : node->children) {
            i = nullptr;
        }
        return node;
    }

    Vertex3D childSize = (bounds.max - bounds.min).getScaled(0.5);

    Logging::Message("OctreeNode: (%d)", recursiveDepth);

    for (int i = 0; i < 8; i++) {
        AABB3D childBounds;
        Vertex3D childOffset;

        switch (i) {
            case 0:
                childOffset = Vertex3D(0, 0, 0);
                break;
            case 1:
                childOffset = Vertex3D(childSize.x, 0, 0);
                break;
            case 2:
                childOffset = Vertex3D(childSize.x, childSize.y, 0);
                break;
            case 3:
                childOffset = Vertex3D(childSize.x, childSize.y, childSize.z);
                break;
            case 4:
                childOffset = Vertex3D(0, 0, childSize.z);
                break;
            case 5:
                childOffset = Vertex3D(0, childSize.y, childSize.z);
                break;
            case 6:
                childOffset = Vertex3D(0, childSize.y, 0);
                break;
            case 7:
                childOffset = Vertex3D(childSize.x, 0, childSize.z);
                break;
            default:
                break;
        }

        childBounds.min = bounds.min + childOffset;
        childBounds.max = bounds.min + childOffset + childSize;
        childBounds.updateVertices();

        node->children[i] = BuildOctree(
            childBounds,
            recursiveDepth + 1
        );
        Logging::Message("ChildrenNode: (%d) - depth: %d", i, recursiveDepth + 1);

    }

    return node;
}

bool Octree::isTriangleInsideAABB(Triangle *triangle, AABB3D childBounds)
{
    std::vector<Plane> planes = childBounds.getPlanes();

    triangle->updateObjectSpace();

    bool r1 = Plane::isVertex3DClosedByPlanes(triangle->Ao, planes);
    bool r2 = Plane::isVertex3DClosedByPlanes(triangle->Bo, planes);
    bool r3 = Plane::isVertex3DClosedByPlanes(triangle->Co, planes);

    if (r1 || r2 || r3) {
        return true;
    }

    return false;
}

OctreeNode* Octree::FindNodeContainingVertex(OctreeNode* node, const Vertex3D& vertex)
{
    if (!node || !node->bounds.isPointInside(vertex)) {
        return nullptr;
    }

    if (node->isLeaf()) {
        return node;
    }

    for (int i = 0; i < 8; i++) {
        if (node->children[i]) {
            OctreeNode* foundNode = FindNodeContainingVertex(node->children[i], vertex);
            if (foundNode) {
                return foundNode;
            }
        }
    }

    return node;
}

OctreeNode* Octree::FindNode(Vertex3D vertex)
{
    return FindNodeContainingVertex(this->root, vertex);
}