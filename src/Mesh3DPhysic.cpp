//
// Created by darkhead on 2019-05-11.
//

#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <LinearMath/btDefaultMotionState.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include "Mesh3DPhysic.h"
#include "../headers/Render/Logging.h"
#include "../headers/Render/Transforms.h"

Mesh3DPhysic::Mesh3DPhysic()
{
    m_isActive = true;
    m_disableRotation = false;
}


btRigidBody* Mesh3DPhysic::makeRigidBody(std::vector<Mesh3DPhysic*> &physicsObjects, Camera3D *cam, btDiscreteDynamicsWorld *world)
{
    btConvexHullShape* me = new btConvexHullShape();

    for (int i=0; i < this->n_triangles; i++) {
        this->model_triangles[i].updateVertexSpaces(cam);
        btVector3 a = btVector3( this->model_triangles[i].A.x, this->model_triangles[i].A.y, this->model_triangles[i].A.z );
        btVector3 b = btVector3( this->model_triangles[i].B.x, this->model_triangles[i].B.y, this->model_triangles[i].B.z );
        btVector3 c = btVector3( this->model_triangles[i].C.x, this->model_triangles[i].C.y, this->model_triangles[i].C.z );

        me->addPoint(a);
        me->addPoint(b);
        me->addPoint(c);
    }

    btTransform trans;
    trans.setIdentity();

    Vertex3D pos = *this->getPosition();
    trans.setOrigin(btVector3(pos.x , pos.y, pos.z));

    btScalar mass(1.);
    btVector3 localInertia(0, 0, 0);
    btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
    btCollisionShape* shape = new btConvexHullShape(*me);
    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
    this->m_body = new btRigidBody(cInfo);

    world->addRigidBody(this->m_body);
    physicsObjects.push_back(this);

    return this->m_body;
}

void Mesh3DPhysic::integrate()
{
    if (m_isActive) {
        m_body->activate();
    }
    // Sync position
    btTransform t ;
    m_body->getMotionState()->getWorldTransform(t);
    btVector3 pos = t.getOrigin();

    Vertex3D worldPosition = Vertex3D(pos.getX() , pos.getY() , pos.getZ());
    this->setPosition(worldPosition);

    // Sync rotation
    btQuaternion  quat = t.getRotation();
    float angle = quat.getAngle();
    btVector3 axis = quat.getAxis();

}