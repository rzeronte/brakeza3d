//
// Created by darkhead on 2019-01-06.
//

#ifndef BRAKEDA3D_COLLIDER_H
#define BRAKEDA3D_COLLIDER_H


#include <vector>
#include "../headers/Objects/Vertex3D.h"
#include "../headers/Objects/Vector3D.h"
#include "../headers/Render/Plane.h"

class Collider {

public:
    //Triangle object;
    Vector3D movement;

    // data about player movement
    Vertex3D velocity;
    Vertex3D sourcePoint;

    // radius of ellipsoid.
    Vertex3D eRadius;

    // for error handling
    Vertex3D lastSafePosition;
    bool stuck;

    // data for collision response
    bool foundCollision;
    double    nearestDistance; // nearest distance to hit
    Vertex3D nearestIntersectionPoint; // on sphere
    Vertex3D nearestPolygonIntersectionPoint; // on polygon
    //	DoubleVector3d nearestPolygonNormal;

};

#endif //BRAKEDA3D_COLLIDER_H
