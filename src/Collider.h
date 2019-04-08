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
    Collider() {
        this->movement = Vector3D(Vertex3D(0, 0, 0), Vertex3D(0, 0, 0));
        this->gravity = Vertex3D(0, 0, 0);
        this->eRadius = Vertex3D(30, 30, 30);
        this->intersectionPoint = Vertex3D(0, 0, 0);
    }

    int collisionID = 0;
    Vertex3D eRadius;   // ellipsoid radius

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

    float t = 0;
    int numCollisionInTest;
    bool onGround = false;

    // forces
    Vertex3D air;
    Vertex3D gravity;
    Vector3D movement;
    Vertex3D friction;
    Plane planeCollision;

    bool embeddedInPlane;
    int triangleIdSelected = 0;

    std::vector<int> historicalTriangleIdCollisionInTest;
    std::vector<Vertex3D> historicalIntersectionPointsInTest;
    std::vector<Plane> historicalPlanesIntersectedInTest;

};

#endif //BRAKEDA3D_COLLIDER_H
