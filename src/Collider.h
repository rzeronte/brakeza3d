//
// Created by darkhead on 2019-01-06.
//

#ifndef BRAKEDA3D_COLLIDER_H
#define BRAKEDA3D_COLLIDER_H


#include "../headers/Objects/Vertex3D.h"
#include "../headers/Objects/Vector3D.h"
#include "../headers/Render/Plane.h"

class Collider {

public:
    Vertex3D eRadius;  // ellipsoid radius
    // Information about the move being requested: (in R3)

    Vertex3D R3Velocity;
    Vertex3D R3Position;

    // Information about the move being requested: (in eSpace)
    Vertex3D velocity;
    Vertex3D normalizedVelocity;
    Vertex3D basePoint;

    // Hit information
    bool foundCollision;
    double nearestDistance;
    Vertex3D intersectionPoint;

    Vertex3D normalPlane;
    int collisionType;
};

#endif //BRAKEDA3D_COLLIDER_H
