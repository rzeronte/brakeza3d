/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by darkhead on 24/2/21.
//

#ifndef BRAKEDA3D_OCTREE_H
#define BRAKEDA3D_OCTREE_H


#include "AABB3D.h"
#include "Triangle3D.h"

struct OctreeNode {
    bool leaf = false;
    AABB3D bounds;
    std::vector<OctreeNode> children;

    [[nodiscard]] bool isLeaf() const
    {
        if (children.empty()) {
            return true;
        }

        return false;
    }
};

class Octree {

private:
    OctreeNode* FindNodeContainingVertex(OctreeNode* node, const Vertex3D& vertex);
    int maxDepth;
    AABB3D bounds;
public:

    Octree(AABB3D bounds, int maxDepth);

    OctreeNode BuildOctree(AABB3D bounds, int recursiveDepth);

    static bool isTriangleInsideAABB(Triangle *triangle, AABB3D childBounds);

    OctreeNode* FindNode(Vertex3D vertex) ;

    OctreeNode root;

    cJSON *getJSON();
};


#endif //BRAKEDA3D_OCTREE_H
