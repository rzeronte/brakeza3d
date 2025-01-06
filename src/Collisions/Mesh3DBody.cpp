
#include <btBulletDynamicsCommon.h>
#include "../../include/Physics/Mesh3DBody.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"

Mesh3DBody::Mesh3DBody()
{
    setMass(0.f);
}

void Mesh3DBody::integrate()
{
    if (this->body == nullptr || this->mass == 0) {
        return;
    }

    btTransform t;
    body->getMotionState()->getWorldTransform(t);
    const btVector3 pos = t.getOrigin();
    setPosition(Vertex3D(pos.getX(), pos.getY(), pos.getZ()));

    auto rotation = t.getRotation();
    btMatrix3x3 matrixRotation;
    matrixRotation.setRotation(rotation);

    setRotation(Tools::BulletM3ToM3(matrixRotation));
}

void Mesh3DBody::makeRigidBody(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    setMass(mass);
    setTypeShape(BodyTypeShape::BODY_TRIANGLE3D_MESH_SHAPE);

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(btVector3(getPosition().x, getPosition().y, getPosition().z));

    btMatrix3x3 matrixRotation = Tools::M3ToBulletM3(getRotation());
    btQuaternion rotation;
    matrixRotation.getRotation(rotation);

    auto *collisionShape = this->getTriangleMeshFromMesh3D();
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
        M3 rotation,
        Vertex3D dimensions,
        btDiscreteDynamicsWorld *world,
        int collisionGroup,
        int collisionMask
) {
    setTypeShape(BodyTypeShape::BODY_SIMPLE_SHAPE);

    setMass(mass);

    btVector3 position;
    pos.saveToBtVector3(&position);

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(btVector3(getPosition().x, getPosition().y, getPosition().z));

    btMatrix3x3 brakezaRotation = Tools::M3ToBulletM3(rotation);
    btQuaternion qRotation;
    brakezaRotation.getRotation(qRotation);

    transformation.setRotation(qRotation);

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

void Mesh3DBody::makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    setTypeShape(BodyTypeShape::BODY_TRIANGLE3D_MESH_SHAPE);

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
        Logging::Log("Mesh3DBody: Collision %s with %s", getLabel().c_str(), object->getLabel().c_str());
    }
}

void Mesh3DBody::drawImGuiProperties()
{
    Mesh3D::drawImGuiProperties();
    ImGui::Separator();
    if (ImGui::TreeNode("Rigid Body")) {
        const float range_sensibility = 1;
        const float range_min = 10000;
        const float range_max = -10000;

        ImGui::DragScalar("Mass", ImGuiDataType_Float, &mass, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
        if (ImGui::IsItemEdited()) {
            setMass(mass);
        }
        ImGui::Separator();

        auto flags = ImGuiComboFlags_None;
        const char* items[] = { "Simple body", "Triangle Mesh body" };
        int item_current_idx = getTypeShape();

        const char* combo_preview_value = items[item_current_idx];
        auto titleCombo = "Collision shape###" + getLabel();

        if (ImGui::BeginCombo(titleCombo.c_str(), combo_preview_value, flags)) {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(items[n], is_selected)) {
                    item_current_idx = n;
                    if (!is_selected) {
                        switch (n) {
                            case 0: {
                                removeCollisionObject();
                                updateBoundingBox();
                                this->makeSimpleRigidBody(
                                        mass,
                                        getPosition(),
                                        getRotation(),
                                        getAabb().size(),
                                        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                                        btBroadphaseProxy::DefaultFilter,
                                        btBroadphaseProxy::DefaultFilter
                                );
                                Logging::Message("Making object %s physics with: Box shape %d", getLabel().c_str(), getTypeShape());
                                break;
                            }
                            case 1: {
                                removeCollisionObject();
                                this->makeRigidBody(
                                        mass,
                                        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                                        btBroadphaseProxy::DefaultFilter,
                                        btBroadphaseProxy::DefaultFilter
                                );
                                Logging::Message("Making object %s physics with: Mesh3D shape %d", getLabel().c_str(), getTypeShape());
                                break;
                            }
                        }
                    }
                }
            }
            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }
}

const char *Mesh3DBody::getTypeObject() {
    return "Mesh3DBody";
}

const char *Mesh3DBody::getTypeIcon() {
    return "gearIcon";
}


cJSON * Mesh3DBody::getJSON()
{
    cJSON *root = Mesh3D::getJSON();

    cJSON_AddNumberToObject(root, "typeShape", typeShape);

    return root;
}

void Mesh3DBody::setPropertiesFromJSON(cJSON *object, Mesh3DBody *o)
{
    o->setBelongToScene(true);
    Mesh3D::setPropertiesFromJSON(object, o);
    auto type = cJSON_GetObjectItemCaseSensitive(object, "typeShape")->valueint;

    switch (type) {
        case BodyTypeShape::BODY_SIMPLE_SHAPE: {
            o->updateBoundingBox();
            o->makeSimpleRigidBody(
                o->mass,
                o->getPosition(),
                o->getRotation(),
                o->getAabb().size(),
                Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                btBroadphaseProxy::DefaultFilter,
                btBroadphaseProxy::DefaultFilter
            );
            break;
        }
        case BodyTypeShape::BODY_TRIANGLE3D_MESH_SHAPE: {
            o->makeRigidBody(
                o->mass,
                Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                btBroadphaseProxy::DefaultFilter,
                btBroadphaseProxy::DefaultFilter
            );
            break;
        }
    }
}

void Mesh3DBody::createFromJSON(cJSON *object)
{
    auto o = new Mesh3DBody();

    Mesh3DBody::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}