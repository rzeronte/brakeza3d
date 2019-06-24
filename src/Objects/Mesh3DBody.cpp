//
// Created by darkhead on 2019-05-11.
//

#include <btBulletDynamicsCommon.h>
#include "../../headers/Objects/Mesh3DBody.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Transforms.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../../headers/Render/Engine.h"

extern Engine *brakeza3D;

Mesh3DBody::Mesh3DBody()
{
    m_isActive = false;
    m_disableRotation = false;
    mass = 1.f;

}

btRigidBody* Mesh3DBody::makeRigidBody(float mass, std::vector<Mesh3DBody*> &physicsObjects, Camera3D *cam, btDiscreteDynamicsWorld *world, bool useObjectSpace)
{
    btConvexHullShape* me = new btConvexHullShape();

    for (int i=0; i < this->n_triangles; i++) {
        this->model_triangles[i].updateFullVertexSpaces(cam);
        btVector3 a, b, c;
        // Esto solo lo utilizamos para mayas procedentes de triángulos BSP en crudo.
        if (useObjectSpace) {
            a = btVector3( this->model_triangles[i].Ao.x, this->model_triangles[i].Ao.y, this->model_triangles[i].Ao.z );
            b = btVector3( this->model_triangles[i].Bo.x, this->model_triangles[i].Bo.y, this->model_triangles[i].Bo.z );
            c = btVector3( this->model_triangles[i].Co.x, this->model_triangles[i].Co.y, this->model_triangles[i].Co.z );
        } else {
            a = btVector3( this->model_triangles[i].A.x, this->model_triangles[i].A.y, this->model_triangles[i].A.z );
            b = btVector3( this->model_triangles[i].B.x, this->model_triangles[i].B.y, this->model_triangles[i].B.z );
            c = btVector3( this->model_triangles[i].C.x, this->model_triangles[i].C.y, this->model_triangles[i].C.z );
        }
        me->addPoint(a);
        me->addPoint(b);
        me->addPoint(c);
    }

    this->mass = mass;

    btTransform trans;
    trans.setIdentity();

    Vertex3D pos = *this->getPosition();
    trans.setOrigin(btVector3(pos.x , pos.y, pos.z));

    btVector3 localInertia(0, 0, 0);
    btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
    btCollisionShape* shape = new btConvexHullShape(*me);
    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
    this->m_body = new btRigidBody(cInfo);
    m_body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

    this->m_body->setUserPointer(this);

    world->addRigidBody(this->m_body);
    physicsObjects.push_back(this);


    return this->m_body;
}

void Mesh3DBody::integrate()
{
    if (m_isActive) {
        m_body->activate();
    }

    if (this->mass == 0) {
        return;
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
