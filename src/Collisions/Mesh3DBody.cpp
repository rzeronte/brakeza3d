
#include <btBulletDynamicsCommon.h>
#include "../../include/Physics/Mesh3DBody.h"
#include "../../include/Render/Logging.h"

Mesh3DBody::Mesh3DBody()
{
    setMass(0.f);
    BSPEntityIndex = -1;
}

void Mesh3DBody::integrate()
{

    if (this->body == nullptr) {
        return;
    }

    if (this->mass == 0) {
        return;
    }

    btTransform t;
    body->getMotionState()->getWorldTransform(t);
    btVector3 pos = t.getOrigin();

    this->setPosition(Vertex3D(pos.getX(), pos.getY(), pos.getZ()));

    auto rotation = t.getRotation();
    btMatrix3x3 matrixRotation;
    matrixRotation.setRotation(rotation);

    M3 brakezaRotation = Tools::BulletM3ToM3(matrixRotation);
    setRotation(brakezaRotation);
}

void Mesh3DBody::makeRigidBody(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    setMass(mass);

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(btVector3(getPosition().x, getPosition().y, getPosition().z));

    btMatrix3x3 matrixRotation = Tools::M3ToBulletM3(getRotation());
    btQuaternion rotation;
    matrixRotation.getRotation(rotation);

    auto *collisionShape = this->getConvexHullShapeFromMesh();
    btVector3 inertia(0, 0, 0);
    collisionShape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo cInfo(
        mass,
        new btDefaultMotionState(transformation),
        collisionShape,
        inertia
    );

    this->body = new btRigidBody(cInfo);
    this->body->setUserPointer(dynamic_cast<Body *> (this));
    this->body->setRestitution(0.5);

    world->addRigidBody(this->body, collisionGroup, collisionMask );
}

void Mesh3DBody::makeSimpleRigidBody(
        float mass,
        Vertex3D pos,
        Vertex3D dimensions,
        btDiscreteDynamicsWorld *world,
        int collisionGroup,
        int collisionMask
) {
    setMass(mass);

    btVector3 position;
    pos.saveToBtVector3(&position);

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(btVector3(getPosition().x, getPosition().y, getPosition().z));

    btCollisionShape *collisionShape = new btBoxShape(btVector3(dimensions.x, dimensions.y, dimensions.z));
    btVector3 inertia(0, 0, 0);
    collisionShape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo cInfo(
        mass,
        new btDefaultMotionState(transformation),
        collisionShape,
        inertia
    );

    this->body = new btRigidBody(cInfo);
    this->body->activate(true);
    this->body->setUserPointer(dynamic_cast<Body *> (this));
    this->body->setRestitution(0.5);

    world->addRigidBody(this->body, collisionGroup, collisionMask);
}

void Mesh3DBody::makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask) {
    setMass(mass);

    btTransform transformation;
    transformation.setIdentity();

    btBvhTriangleMeshShape *triangleMeshShape = this->getTriangleMeshFromMesh3D();

    btVector3 position;
    getPosition().saveToBtVector3(&position);
    transformation.setOrigin(position);

    btRigidBody::btRigidBodyConstructionInfo info(
        mass,
        new btDefaultMotionState(transformation),
        triangleMeshShape,
        btVector3(0, 0, 0)
    );

    this->body = new btRigidBody(info);
    this->body->activate(true);
    this->body->setContactProcessingThreshold(BT_LARGE_FLOAT);
    this->body->setUserPointer(dynamic_cast<Body *> (this));

    world->addRigidBody(this->body, collisionGroup, collisionMask);
}

void Mesh3DBody::setGravity(Vertex3D g) {
    btVector3 gravity;
    Vertex3D(g.x, g.y, g.z).saveToBtVector3(&gravity);
    getRigidBody()->setGravity(gravity);
}

btBvhTriangleMeshShape *Mesh3DBody::getTriangleMeshFromMesh3D() {
    auto *triangleMesh = new btTriangleMesh();
    updateBoundingBox();

    for (auto & modelTriangle : this->modelTriangles) {
        btVector3 a, b, c;
        modelTriangle->A.saveToBtVector3(&a);
        modelTriangle->B.saveToBtVector3(&b);
        modelTriangle->C.saveToBtVector3(&c);

        triangleMesh->addTriangle(a, b, c, false);
    }

    return new btBvhTriangleMeshShape(triangleMesh, true, true);
}

btConvexHullShape *Mesh3DBody::getConvexHullShapeFromMesh() {
    auto *convexHull = new btConvexHullShape();
    for (auto & modelTriangle : this->modelTriangles) {
        btVector3 a, b, c;
        modelTriangle->A.saveToBtVector3(&a);
        modelTriangle->B.saveToBtVector3(&b);
        modelTriangle->C.saveToBtVector3(&c);

        convexHull->addPoint(a);
        convexHull->addPoint(b);
        convexHull->addPoint(c);
    }

    return convexHull;
}

void Mesh3DBody::resolveCollision(Collisionable *with)
{
    if (EngineSetup::get()->LOG_COLLISION_OBJECTS) {
        auto *object = dynamic_cast<Object3D*> (with);
        Logging::getInstance()->Log("Mesh3DBody: Collision "  + getLabel() + " with " + object->getLabel());
    }
}

void Mesh3DBody::remove()
{
    this->setRemoved(true);
}