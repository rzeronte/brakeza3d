//
// Created by darkhead on 2019-04-02.
//

#include "Collision.h"
#include "Render/Maths.h"


Vertex3D Collision::collideAndStop(const Vertex3D &radiusVector, std::vector<Triangle> &polygonList, const Vector3D &r)
{

}

Vertex3D Collision::collideAndSlide(const Vertex3D &radiusVector, std::vector<Triangle> &polygonList, const Vector3D &r, Vertex3D &lastDir, const bool filterPuseJumps)
{

}

void Collision::traceCollision(CollisionList &cl)
{

}

bool Collision::unitSphereIntersection(const Vertex3D &center, const Vector3D &r, float &time)
{

}

bool Collision::isEmbedded(const Triangle &p, const Vertex3D &sphereCenter, Vertex3D &innerMostPoint)
{

}

CollisionList Collision::calcColliders(std::vector<Triangle> &polygonList, const Vector3D &r)
{
    CollisionList	result;

    // Go through our list of potential colliders

    for (int i = 0; i < polygonList.size(); i++)
    {
        // The goal through this loop is to calculate the collision plane

        float		collisionDistance;
        Plane		collisionPlane;
        CollisionType	collisionType = CT_NONE;

        // Our current polygon

        Triangle	&p = polygonList[i];

        // Ignore polygons in the skip list

        bool	skipMe = false;
        for (int j = 0; j < skipList.size(); j++)
        {
            if (skipList[j] == &p) skipMe = true;
        }
        if (skipMe) continue;

        // Ignore back-facing polygons

        if (p.plane().distance(r.origin()) <= 0) continue;

        // Find the point on the sphere, that will eventually collide with the polygon's plane
        Vertex3D	sphereIntersectionPoint = r.origin() - p.plane().getNormalVector();

        // At this point, we know we're in front of the current plane, and we're heading toward it
        //
        // Trace a ray to the plane

        Vector3D	sphereIntersectionRay(sphereIntersectionPoint, r.getComponent());
        if (!p.plane().intersect(sphereIntersectionRay, collisionDistance)) continue;

        // If the polygon is embedded, set the collision type, and define a collision plane that is perpendicular
        // to the direction that the sphere must travel to be un-embedded

        Vertex3D	innerMostPoint;
        if (isEmbedded(p, r.origin(), innerMostPoint))
        {
            collisionDistance = innerMostPoint.distance(r.origin()) - 1;
            collisionPlane.setOrigin( innerMostPoint );
            collisionPlane.setNormal( r.origin() - innerMostPoint );
            collisionType = CT_EMBEDDED;
        }

            // Surface/Edge/Point collision

        else

        {
            // The plane intersection point

            Vertex3D	planeIntersectionPoint = sphereIntersectionPoint + r.normal().getScaled( collisionDistance );

            // Surface collision?

            if (p.isPointInside(planeIntersectionPoint, EPSILON))
            {
                // Is this a valid collision?

                if (collisionDistance < -EPSILON * (float) 2) continue;

                // Our collision plane is our polygon's plane

                collisionPlane = p.plane();
                collisionType = CT_SURFACE;
            }

                // Edge/Point collision

            else
            {
                // New collision plane origin

                Vertex3D	e0, e1;
                bool	edgeFlag;

                collisionPlane.origin() = Maths::closestPointOnPerimeter(p, planeIntersectionPoint, e0, e1, edgeFlag);

                // Point collision?

                if (!edgeFlag)
                {
                    Vector3D	toSphere(collisionPlane.origin(), r.normal().getInverse()); // --> OJO AQUÍ
                    if (!unitSphereIntersection(r.origin(), toSphere, collisionDistance)) continue;

                    // The collision plane

                    sphereIntersectionPoint = collisionPlane.origin() - r.normal().getScaled( collisionDistance );
                    collisionPlane.setNormal( r.origin() - sphereIntersectionPoint );

                    // Generate a vector that points from the vertex at the sphere

                    Vertex3D	atSphere = r.origin() - collisionPlane.origin();

                    // We must be headed at the collision plane

                    if ((r.getComponent() * atSphere) > 0) continue;

                    // The plane is not allowed to face away from the normal of the polygon

                    if ((collisionPlane.getNormalVector() * p.plane().getNormalVector()) < 0) continue;

                    // The plane is not allowed to face the interior of the polygon

                    if (collisionPlane.distance(e0) > 0) continue;
                    if (collisionPlane.distance(e1) > 0) continue;

                    collisionType = CT_POINT;
                }

                    // Edge collision

                else
                {
                    // Find the plane defined by the edge and the velocity vector (cross product)
                    //
                    // This plane will be used to bisect the sphere

                    Vertex3D	edgeNormal = (e1 - e0).getNormalize();

                    float	edgeDot = fabs(r.normal() * edgeNormal);
                    if (edgeDot > 1-EPSILON) continue;
                    Plane	bisectionPlane(e1, r.normal() % edgeNormal);

                    // The intersection of a plane and a sphere is a disc. We want to find the center
                    // of that disc. The center of that disc is the closest point on the bisection plane
                    // to the center of the sphere.

                    Vertex3D	discCenter = bisectionPlane.closest(r.origin());

                    // If the center of the disc is outside the sphere, then the sphere does not intersect
                    // the bisection plane and therefore, will never collide with the edge

                    if (discCenter.distance(r.origin()) > 1-EPSILON) continue;

                    // Find the closest point on the edge to the center of the disc

                    Vertex3D	edgePoint = Maths::closestPointOnLine(e0, e1, discCenter);

                    // Generate a ray that traces back toward the sphere

                    Vector3D	toSphere(edgePoint, discCenter - edgePoint);

                    // Trace from the edgePoint back to the sphere.
                    //
                    // This will be the sphereIntersectionPoint

                    float	t;
                    if (!unitSphereIntersection(r.origin(), toSphere, t)) continue;
                    sphereIntersectionPoint = toSphere.end(t);

                    // Will the sphere interesction point, when travelling along 'r' ever intersect the actual
                    // edge?
                    //
                    // In order to determine this, we'll start by defining a plane that lies on the edge, but is
                    // perpendicular to 'r'.

                    Vertex3D	edgeVect =  ( (e1 - e0) % r.normal() % (e1 - e0) ).getNormalize();
                    Plane	edgePlane(e0, edgeVect);

                    // Next, we'll intersect the edgeVect ray with the plane

                    Vector3D toLine(sphereIntersectionPoint, r.normal());
                    edgePlane.intersect(toLine, t);

                    // Finally, does the result end up with a point inside the line segment defined by the
                    // two edge points? If not, then the sphere won't intersect the edge.

                    if (!Maths::isPointInsideEdge(e0, e1, toLine.end(t))) continue;

                    // Our collision plane is the tangent plane at the sphereIntersectionPoint

                    collisionPlane.setNormal( r.origin() - sphereIntersectionPoint );

                    // The plane is not allowed to face the interior of the polygon...

                    if (collisionPlane.distance(p.getCenterOfMass()) > -0) continue;

                    // The plane is not allowed to face away from the normal of the polygon

                    if ((collisionPlane.getNormalVector() * p.plane().getNormalVector()) < 0) continue;

                    // Trace from the sphereIntersectionPoint to the plane to find the collisionDistance

                    Vector3D	fromSphere(sphereIntersectionPoint, r.normal());

                    collisionPlane.intersect(fromSphere, collisionDistance);

                    // Don't allow collisions behind us

                    if (collisionDistance < -1) continue;

                    collisionType = CT_EDGE;
                }
            }
        }

        // Was it an actual collision?

        if (collisionType != CT_EMBEDDED && collisionDistance > r.getComponent().getModule()) continue;

        // Ignore collision planes that we're traveling away from

        float tp1 = collisionPlane.getNormalVector() * r.normal();
        if (collisionType != CT_EMBEDDED && (tp1) >= 0) continue;

        // Is it closer, farther away, or the same distance as what we've found so far?

        if (result.size())
        {
            float	d = fabs(collisionDistance - result[0].collisionDistance);
            if (d > EPSILON)
            {
                if (collisionDistance < result[0].collisionDistance) result.clear();
                else if (collisionDistance > result[0].collisionDistance) continue;
            }

                // Make sure it's not already in the list (this plane's normal)

            else
            {
                bool	skipMe = false;
                for (int i = 0; i < result.size(); i++)
                {
                    if ((result[i].collisionPlane.getNormalVector() * collisionPlane.getNormalVector()) > 1 - EPSILON)
                    {
                        skipMe = true;
                        break;
                    }
                }
                if (skipMe) continue;
            }
        }

        // Add this collider to the list

        sCollision	nearest;
        nearest.collider = &p;
        nearest.collisionDistance = collisionDistance;
        nearest.collisionPlane = collisionPlane;
        nearest.collisionType = collisionType;
        result.push_back(nearest);
    }

    return result;
}