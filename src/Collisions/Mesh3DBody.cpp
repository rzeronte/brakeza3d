
#include <btBulletDynamicsCommon.h>
#include "../../include/Physics/Mesh3DBody.h"
#include "../../include/Render/Logging.h"
#include "../../include/EngineSetup.h"

Mesh3DBody::Mesh3DBody() {
    setMass(1.f);
    BSPEntityIndex = -1;
}

void Mesh3DBody::makeRigidBody(float mass, btDiscreteDynamicsWorld *world) {

    setMass(mass);

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(btVector3(getPosition().x, getPosition().y, getPosition().z));

    btMatrix3x3 matrixRotation = Tools::M3ToBulletM3(getRotation());
    btQuaternion rotation;
    matrixRotation.getRotation(rotation);

    btRigidBody::btRigidBodyConstructionInfo cInfo(
        mass,
        new btDefaultMotionState(transformation),
        new btConvexHullShape(*this->getConvexHullShapeFromMesh()),
        btVector3(0, 0, 0)
    );

    this->body = new btRigidBody(cInfo);
    this->body->setUserPointer(dynamic_cast<Body *> (this));

    world->addRigidBody(this->body);
}

void Mesh3DBody::makeSimpleRigidBody(float mass, Vertex3D pos, Vertex3D dimensions, btDiscreteDynamicsWorld *world) {
    setMass(mass);

    btVector3 position;
    pos.saveToBtVector3(&position);

    btTransform transform;
    transform.setOrigin(position);

    auto *myMotionState = new btDefaultMotionState(transform);
    btCollisionShape *shape = new btBoxShape(btVector3(dimensions.x, dimensions.y, dimensions.z));

    btRigidBody::btRigidBodyConstructionInfo cInfo(
        mass,
        myMotionState,
        shape,
        btVector3(0, 0, 0)
    );

    this->body = new btRigidBody(cInfo);
    this->body->activate(true);
    this->body->setUserPointer(dynamic_cast<Body *> (this));

    world->addRigidBody(this->body);
}

void Mesh3DBody::integrate() {

    if (this->body == nullptr) {
        return;
    }

    if (this->mass == 0) {
        return;
    }

    btTransform t;
    body->getMotionState()->getWorldTransform(t);
    btVector3 pos = t.getOrigin();

    Vertex3D worldPosition = Vertex3D(pos.getX(), pos.getY(), pos.getZ());
    this->setPosition(worldPosition);
}


void Mesh3DBody::makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *world) {
    setMass(mass);
    updateBoundingBox();

    this->triangleMesh = new btTriangleMesh();

    for (auto & modelTriangle : this->modelTriangles) {
        btVector3 a, b, c;
        a = btVector3(modelTriangle->Ao.x, modelTriangle->Ao.y, modelTriangle->Ao.z);
        b = btVector3(modelTriangle->Bo.x, modelTriangle->Bo.y, modelTriangle->Bo.z);
        c = btVector3(modelTriangle->Co.x, modelTriangle->Co.y, modelTriangle->Co.z);

        this->triangleMesh.addTriangle(a, b, c, false);
    }

    btTransform trans;
    trans.setIdentity();
    btVector3 localInertia(0, 0, 0);

    this->shape = new btBvhTriangleMeshShape(&triangleMesh, true, true);
    this->shape->calculateLocalInertia(0, localInertia);

    btVector3 position;
    getPosition().saveToBtVector3(&position);
    trans.setOrigin(position);

    btRigidBody::btRigidBodyConstructionInfo info(
        mass,
        new btDefaultMotionState(trans),
        shape,
        localInertia
    );

    this->body = new btRigidBody(info);
    this->body->activate(true);
    this->body->setContactProcessingThreshold(BT_LARGE_FLOAT);
    this->body->setCcdMotionThreshold(1.f);
    this->body->setCcdSweptSphereRadius(1.f);
    this->body->setUserPointer(dynamic_cast<Body *> (this));

    world->addRigidBody(this->body);
}

void Mesh3DBody::setGravity(Vertex3D g) {
    btVector3 gravity;
    Vertex3D(0, 0, 0).saveToBtVector3(&gravity);
    getRigidBody()->setGravity(gravity);
}

btConvexHullShape *Mesh3DBody::getConvexHullShapeFromMesh() {
    auto *convexHull = new btConvexHullShape();
    for (auto & modelTriangle : this->modelTriangles) {

        btVector3 a = btVector3(modelTriangle->A.x, modelTriangle->A.y, modelTriangle->A.z);
        btVector3 b = btVector3(modelTriangle->B.x, modelTriangle->B.y, modelTriangle->B.z);
        btVector3 c = btVector3(modelTriangle->C.x, modelTriangle->C.y, modelTriangle->C.z);

        convexHull->addPoint(a);
        convexHull->addPoint(b);
        convexHull->addPoint(c);
    }

    return convexHull;
}

void Mesh3DBody::resolveCollision(Collisionable *with) {
    if (EngineSetup::get()->LOG_COLLISION_OBJECTS) {
        auto *object = dynamic_cast<Object3D*> (with);
        Logging::getInstance()->Log("Mesh3DBody: Collision "  + getLabel() + " with " + object->getLabel());
    }
}