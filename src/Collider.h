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
    Collider();

    // Punto inicial
    Vertex3D basePoint;

    // Velocidad
    Vertex3D frameVelocity;
    Vertex3D normalizedVelocity;

    // Hit information
    bool foundCollision;
    double nearestDistance;
    Vertex3D intersectionPoint;


    bool onGround = false;
    bool veryClosed = false;
    bool maxDeepRecursion = false;

    // forces
    Vertex3D air;
    Vertex3D gravity;
    Vector3D movement;
    Vertex3D friction;
};

#endif //BRAKEDA3D_COLLIDER_H
