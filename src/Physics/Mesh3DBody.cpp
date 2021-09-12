
#include <btBulletDynamicsCommon.h>
#include "../../headers/Physics/Mesh3DBody.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../../headers/Brakeza3D.h"

Mesh3DBody::Mesh3DBody()
{
    mass = 1.f;
    BSPEntityIndex = -1;
}

btRigidBody* Mesh3DBody::makeRigidBody(float mass, Camera3D *cam, btDiscreteDynamicsWorld *world, bool useObjectSpace)
{
    btConvexHullShape* me = new btConvexHullShape();

    for (int i=0; i < this->modelTriangles.size(); i++) {
        this->modelTriangles[i]->updateFullVertexSpaces(cam);
        btVector3 a, b, c;
        // Esto solo lo utilizamos para mayas procedentes de triángulos BSP en crudo.
        if (useObjectSpace) {
            a = btVector3(this->modelTriangles[i]->Ao.x, this->modelTriangles[i]->Ao.y, this->modelTriangles[i]->Ao.z );
            b = btVector3(this->modelTriangles[i]->Bo.x, this->modelTriangles[i]->Bo.y, this->modelTriangles[i]->Bo.z );
            c = btVector3(this->modelTriangles[i]->Co.x, this->modelTriangles[i]->Co.y, this->modelTriangles[i]->Co.z );
        } else {
            a = btVector3(this->modelTriangles[i]->A.x, this->modelTriangles[i]->A.y, this->modelTriangles[i]->A.z );
            b = btVector3(this->modelTriangles[i]->B.x, this->modelTriangles[i]->B.y, this->modelTriangles[i]->B.z );
            c = btVector3(this->modelTriangles[i]->C.x, this->modelTriangles[i]->C.y, this->modelTriangles[i]->C.z );
        }
        me->addPoint(a);
        me->addPoint(b);
        me->addPoint(c);
    }

    this->mass = mass;

    btTransform trans;
    trans.setIdentity();

    Vertex3D pos = this->getPosition();
    trans.setOrigin(btVector3(pos.x , pos.y, pos.z));

    btVector3 localInertia(0, 0, 0);
    btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
    btCollisionShape* shape = new btConvexHullShape(*me);
    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
    this->m_body = new btRigidBody(cInfo);
    this->m_body->setFriction(1);
    this->m_body->setUserPointer(this);

    world->addRigidBody(this->m_body);

    return this->m_body;
}


btRigidBody* Mesh3DBody::makeProjectileRigidBodyToPlayer(float mass, Vertex3D size, Vertex3D dir, btDiscreteDynamicsWorld* world, float forceImpulse)
{
    this->mass = mass;

    btTransform trans;
    trans.setIdentity();

    Vertex3D pos = this->getPosition();

    pos = pos + dir.getScaled(10); // un poquito delante del NPC

    trans.setOrigin(btVector3(pos.x , pos.y, pos.z));

    btVector3 localInertia(0, 0, 0);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);

    btVector3 btSize; size.saveToBtVector3(&btSize);
    btCollisionShape* shape = new btBoxShape(btSize);

    btRigidBody::btRigidBodyConstructionInfo cInfo(this->mass, myMotionState, shape, localInertia);
    this->m_body = new btRigidBody(cInfo);
    this->m_body->setUserPointer(this);
    this->m_body->setCcdMotionThreshold(0.01f);
    this->m_body->setCcdSweptSphereRadius(0.02f);

    dir = dir.getScaled(forceImpulse);
    btVector3 impulse(dir.x, dir.y, dir.z);
    this->m_body->applyCentralImpulse(impulse);

    world->addRigidBody(this->m_body);

    return this->m_body;
}


btRigidBody* Mesh3DBody::makeSimpleRigidBody(float mass, Vertex3D pos, Vertex3D dimensions, btDiscreteDynamicsWorld* world)
{
    this->mass = mass;

    btTransform trans;
    trans.setIdentity();

    trans.setOrigin(btVector3(pos.x , pos.y, pos.z));
    btVector3 localInertia(0, 0, 0);
    btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
    btCollisionShape* shape = new btBoxShape(btVector3( dimensions.x, dimensions.y, dimensions.z) );

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
    this->m_body = new btRigidBody(cInfo);
    this->m_body->activate(true);
    //this->m_body->setFriction(1);
    this->m_body->setUserPointer(this);
    this->m_body->setCcdMotionThreshold(1.f);
    this->m_body->setCcdSweptSphereRadius(1.f);

    world->addRigidBody(this->m_body);

    return this->m_body;
}

void Mesh3DBody::integrate()
{
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


void Mesh3DBody::makeRigidBodyFromTriangleMesh(float mass, Camera3D *cam, btDiscreteDynamicsWorld *world, bool useObjectSpace)
{
    this->triangleMesh = new btTriangleMesh();

    for (int i=0; i < this->modelTriangles.size(); i++) {
        this->modelTriangles[i]->updateFullVertexSpaces(cam);
        btVector3 a, b, c;
        // Esto solo lo utilizamos para mayas procedentes de triángulos BSP en crudo.
        if (useObjectSpace) {
            a = btVector3(this->modelTriangles[i]->Ao.x, this->modelTriangles[i]->Ao.y, this->modelTriangles[i]->Ao.z );
            b = btVector3(this->modelTriangles[i]->Bo.x, this->modelTriangles[i]->Bo.y, this->modelTriangles[i]->Bo.z );
            c = btVector3(this->modelTriangles[i]->Co.x, this->modelTriangles[i]->Co.y, this->modelTriangles[i]->Co.z );
        } else {
            a = btVector3(this->modelTriangles[i]->A.x, this->modelTriangles[i]->A.y, this->modelTriangles[i]->A.z );
            b = btVector3(this->modelTriangles[i]->B.x, this->modelTriangles[i]->B.y, this->modelTriangles[i]->B.z );
            c = btVector3(this->modelTriangles[i]->C.x, this->modelTriangles[i]->C.y, this->modelTriangles[i]->C.z );
        }

        this->triangleMesh.addTriangle(a, b, c, false);
    }

    btTransform trans;
    trans.setIdentity();
    btVector3 localInertia(0, 0, 0);

    this->shape = new btBvhTriangleMeshShape(&triangleMesh, true, true);
    this->shape->calculateLocalInertia(0, localInertia);

    Vertex3D pos = this->getPosition();
    trans.setOrigin(btVector3(pos.x, pos.y, pos.z));

    this->motionState = new btDefaultMotionState(trans);
    btRigidBody::btRigidBodyConstructionInfo info(0.0f, motionState, shape, localInertia) ;
    this->m_body = new btRigidBody(info);
    this->m_body->activate(true);
    this->m_body->setContactProcessingThreshold(BT_LARGE_FLOAT);
    this->m_body->setCcdMotionThreshold(1.f);
    this->m_body->setCcdSweptSphereRadius(1.f);
    this->m_body->setUserPointer(this);

    world->addRigidBody(this->m_body);
}