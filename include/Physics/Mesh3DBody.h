#ifndef BRAKEDA3D_OBJECTPHYSIC_H
#define BRAKEDA3D_OBJECTPHYSIC_H

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "../Objects/Vertex3D.h"
#include "../Objects/Mesh3D.h"
#include "Body.h"


class Mesh3DBody : public Mesh3D, public Body {
public:
    Mesh3DBody();


    void integrate();

    void makeRigidBodyFromTriangleMesh(float mass, Camera3D *cam, btDiscreteDynamicsWorld *, bool useObjectSpace);

    btRigidBody *makeRigidBody(float mass, Camera3D *cam, btDiscreteDynamicsWorld *, bool useObjectSpace);

    btRigidBody *
    makeProjectileRigidBodyToPlayer(float mass, Vertex3D size, Vertex3D dir, btDiscreteDynamicsWorld *world,
                                    float forceImpulse);

    btRigidBody *makeSimpleRigidBody(float mass, Vertex3D pos, Vertex3D dimensions, btDiscreteDynamicsWorld *world);

};


#endif //BRAKEDA3D_OBJECTPHYSIC_H
