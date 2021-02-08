//
// Created by darkhead on 8/2/21.
//

#ifndef BRAKEDA3D_AABB3D_H
#define BRAKEDA3D_AABB3D_H


#include "../Objects/Vertex3D.h"

class AABB3D {
public:

    Vertex3D size() {
        return max - min;
    }

    bool isColliding(AABB3D *other)
    {
        return !(max.x < other->min.x || min.x > other->max.x ||
                 max.y < other->min.y || min.z > other->max.y ||
                 max.z < other->min.z || min.z > other->max.z
         );
    }

    // Maximum values of X,Y,Z
    Vertex3D max;
    // Minimum values of X,Y,Z
    Vertex3D min;
    Vertex3D vertices[8];
};


#endif //BRAKEDA3D_AABB3D_H
