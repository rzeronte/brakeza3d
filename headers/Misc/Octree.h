//
// Created by darkhead on 24/2/21.
//

#ifndef BRAKEDA3D_OCTREE_H
#define BRAKEDA3D_OCTREE_H


#include "../Render/AABB3D.h"
#include "../Objects/Triangle3D.h"

struct OctreeNode
{
    AABB3D bounds;
    OctreeNode* children[8];
    std::vector<Triangle*> triangles;

    bool isLeaf() {
        for (int i = 0; i < 8 ; i++) {
            if (this->children[i] != NULL) {
                return false;
            }
        }

        return true;
    }
};

class Octree {
public:
    OctreeNode *root;

    Octree(std::vector<Triangle*> &triangles, AABB3D bounds);

    OctreeNode* BuildOctree(std::vector<Triangle*> &triangles, AABB3D bounds, int recursiveDepth);
    bool isTriangleInsideAABB(Triangle *triangle, AABB3D childBounds);

};


#endif //BRAKEDA3D_OCTREE_H
