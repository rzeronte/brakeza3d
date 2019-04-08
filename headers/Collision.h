//
// Created by darkhead on 2019-04-02.
//

#ifndef BRAKEDA3D_COLLISION_H
#define BRAKEDA3D_COLLISION_H

// ---------------------------------------------------------------------------------------------------------------------------------

#include "Render/Plane.h"
#include "Objects/Triangle3D.h"

enum	CollisionType	{CT_NONE = 0, CT_SURFACE, CT_EDGE, CT_POINT, CT_EMBEDDED};

// ---------------------------------------------------------------------------------------------------------------------------------

typedef	struct
{
    Triangle	*collider;
    Plane		collisionPlane;
    float		collisionDistance;
    CollisionType	collisionType;
} sCollision;

typedef	std::vector<sCollision> CollisionList;

const	float	EPSILON = 1.0e-5;
static	std::vector<Triangle*>	skipList;

class Collision
{
public:

    static Vertex3D collideAndStop(const Vertex3D &radiusVector, std::vector<Triangle> &polygonList, const Vector3D &r);
    static Vertex3D collideAndSlide(const Vertex3D &radiusVector, std::vector<Triangle> &polygonList, const Vector3D &r, Vertex3D &lastDir, const bool filterPuseJumps = false);

private:

    static void traceCollision(CollisionList &cl);
    static bool unitSphereIntersection(const Vertex3D &center, const Vector3D &r, float &time);
    static bool isEmbedded(const Triangle &p, const Vertex3D &sphereCenter, Vertex3D &innerMostPoint);
    static CollisionList calcColliders(std::vector<Triangle> &polygonList, const Vector3D &r);
};


#endif //BRAKEDA3D_COLLISION_H
