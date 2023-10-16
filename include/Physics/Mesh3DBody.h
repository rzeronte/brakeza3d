#ifndef BRAKEDA3D_OBJECTPHYSIC_H
#define BRAKEDA3D_OBJECTPHYSIC_H

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "../Objects/Vertex3D.h"
#include "../Objects/Mesh3D.h"
#include "Body.h"
#include "Mesh3DGhost.h"


class Mesh3DBody : public Mesh3D, public Body {
public:
    Mesh3DBody();

    void integrate() override;

    void makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *, int collisionGroup, int collisionMask);

    void makeRigidBody(float mass, btDiscreteDynamicsWorld *, int collisionGroup, int collisionMask);

    void makeSimpleRigidBody(float mass, Vertex3D pos, M3 rotation, Vertex3D dimensions, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);

    void setGravity(Vertex3D g);

    static void createFromJSON(cJSON *object);

private:
    btConvexHullShape *getConvexHullShapeFromMesh();

    btBvhTriangleMeshShape *getTriangleMeshFromMesh3D();

protected:
    void resolveCollision(Collisionable *collisionable) override;

    void drawImGuiProperties() override;

    const char *getTypeObject() override;

    const char *getTypeIcon() override;

    cJSON *getJSON() override;

    static void setPropertiesFromJSON(cJSON *object, Mesh3DBody *o);

};


#endif //BRAKEDA3D_OBJECTPHYSIC_H
