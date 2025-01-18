
#include "Collider.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"

Collider::Collider()
:
    collisionsEnabled(false),
    mass(1),
    simpleShapeSize(Vertex3D(1, 1, 1)),
    collisionMode(CollisionMode::GHOST),
    ghostObject( new btPairCachingGhostObject()),
    body(nullptr),
    collisionShape(CollisionShape::SIMPLE_SHAPE)
{
}

void Collider::integrate()
{
}

bool Collider::isCollisionsEnabled() const
{
    return collisionsEnabled;
}

void Collider::setCollisionsEnabled(bool value)
{
    this->collisionsEnabled = value;
}

void Collider::resolveCollision(Collider *o)
{
}

CollisionMode Collider::getCollisionMode() const {
    return collisionMode;
}

void Collider::setCollisionMode(CollisionMode collisionMode) {
    Collider::collisionMode = collisionMode;
}

CollisionShape Collider::getCollisionShape() const {
    return collisionShape;
}

void Collider::setCollisionShape(CollisionShape collisionShape) {
    Collider::collisionShape = collisionShape;
}

void Collider::removeCollisionObject() const
{
    if (collisionMode == CollisionMode::GHOST) {
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeCollisionObject(ghostObject);
    }

    if (collisionMode == CollisionMode::BODY) {
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeCollisionObject(body);
    }
}

void Collider::makeGhostBody(btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{

}

void Collider::makeSimpleGhostBody(Vertex3D dimensions, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(btVector3(0, 0, 0));

    btConvexHullShape *convexHullShape = reinterpret_cast<btConvexHullShape *>(new btBoxShape(
        btVector3(dimensions.x, dimensions.y, dimensions.z)
    ));

    ghostObject->setCollisionShape(convexHullShape);
    ghostObject->setWorldTransform(transformation);
    ghostObject->setUserPointer(this);

    ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

    world->addCollisionObject(ghostObject, collisionGroup, collisionMask);
}

btPairCachingGhostObject *Collider::getGhostObject() const
{
    return ghostObject;
}

btRigidBody *Collider::getRigidBody() const
{
    return body;
}

void Collider::drawImGuiCollisionModeSelector()
{
    auto flags = ImGuiComboFlags_None;
    const char* items[] = { "GHOST", "RIGID BODY" };
    int item_current_idx = (int) collisionMode;
    const char* combo_preview_value = items[item_current_idx];

    if (ImGui::BeginCombo("Collision mode", combo_preview_value, flags)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(items[n], is_selected)) {
                if (!is_selected) {
                    item_current_idx = n;
                    switch (n) {
                        default:
                        case 0: {
                            setupGhostCollider(CollisionShape::SIMPLE_SHAPE);
                            break;
                        }
                        case 1: {
                            setupRigidBodyCollider(CollisionShape::SIMPLE_SHAPE);
                            break;
                        }
                    }
                }
            }
        }
        ImGui::EndCombo();
    }
}

void Collider::drawImGuiCollisionShapeSelector()
{
    if (getCollisionShape() == CollisionShape::SIMPLE_SHAPE) {
        if (ImGui::TreeNode("Simple shape size")) {
            const float range_min = -500000;
            const float range_max = 500000;
            const float range_sensibility = 0.1;

            ImGui::DragScalar("X", ImGuiDataType_Float, &simpleShapeSize.x, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Y", ImGuiDataType_Float, &simpleShapeSize.y, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Z", ImGuiDataType_Float, &simpleShapeSize.z, range_sensibility ,&range_min, &range_max, "%f", 1.0f);

            if (ImGui::Button(std::string("Update collision shape").c_str())) {
                setupGhostCollider(CollisionShape::SIMPLE_SHAPE);
            }
            ImGui::TreePop();
        }
    }
}

void Collider::setupGhostCollider(CollisionShape mode)
{
    Logging::Message("[Collisionable] setupGhostCollider");
    removeCollisionObject();

    setCollisionMode(CollisionMode::GHOST);
    setCollisionShape(mode);


    if (getCollisionShape() == CollisionShape::SIMPLE_SHAPE) {
        makeSimpleGhostBody(
            simpleShapeSize,
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            btBroadphaseProxy::DefaultFilter,
            btBroadphaseProxy::DefaultFilter
        );
    }
}

void Collider::setupRigidBodyCollider(CollisionShape shapeMode)
{
    Logging::Message("[Collisionable] setupRigidBodyCollider");

    removeCollisionObject();

    setCollisionMode(CollisionMode::BODY);
    setCollisionShape(shapeMode);

    if (getCollisionShape() == CollisionShape::SIMPLE_SHAPE) {
        makeSimpleRigidBody(
            mass,
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            btBroadphaseProxy::DefaultFilter,
            btBroadphaseProxy::DefaultFilter
        );
    }
}

void Collider::makeSimpleRigidBody(
        float mass,
        btDiscreteDynamicsWorld *world,
        int collisionGroup,
        int collisionMask
) {
}

float Collider::getMass() const {
    return mass;
}

void Collider::setMass(float mass) {
    Collider::mass = mass;
}
