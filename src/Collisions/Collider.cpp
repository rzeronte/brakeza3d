
#include "../../include/Collision/Collider.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"

Collider::Collider()
:
    collisionsEnabled(false),
    collisionMode(CollisionMode::NONE),
    simpleShapeSize(Vertex3D(1, 1, 1)),
    ghostObject(nullptr),
    body(nullptr),
    colliderStatic(false),
    mass(0),
    kinematicBody(nullptr),
    kinematicCapsuleSize(
        EngineSetup::get()->PLAYER_CAPSULE_RADIUS,
        EngineSetup::get()->PLAYER_CAPSULE_HEIGHT
    ),
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

void Collider::removeCollisionObject()
{
    if (collisionMode == CollisionMode::GHOST && ghostObject != nullptr) {
        Logging::Message("[Collider] Removing collider GHOST");
        setCollisionMode(CollisionMode::NONE);
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeCollisionObject(ghostObject);
        ghostObject = nullptr;
        return;
    }

    if (collisionMode == CollisionMode::BODY && body != nullptr) {
        Logging::Message("[Collider] Removing collider RIGIDBODY");
        setCollisionMode(CollisionMode::NONE);
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeCollisionObject(body);
        body = nullptr;
        return;
    }

    if (collisionMode == CollisionMode::KINEMATIC && kinematicBody != nullptr) {
        Logging::Message("[Collider] Removing collider KINEMATICBODY");
        setCollisionMode(CollisionMode::NONE);
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeAction(characterController);
        ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeCollisionObject(kinematicBody);
        kinematicBody = nullptr;
        delete characterController;
        return;
    }
}

void Collider::makeGhostBody(btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
}

void Collider::makeSimpleGhostBody(Vertex3D dimensions, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    Logging::Message("[Collider] makeSimpleGhostBody");

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(btVector3(0, 0, 0));

    btConvexHullShape *convexHullShape = reinterpret_cast<btConvexHullShape *>(new btBoxShape(dimensions.toBullet()));

    ghostObject = new btPairCachingGhostObject();
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
    const char* items[] = { "GHOST", "RIGID BODY", "KINEMATIC" };
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
                        case 2: {
                            setupKinematicCollider();
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

}

void Collider::setupGhostCollider(CollisionShape mode)
{
    Logging::Message("[Collider] setupGhostCollider");
    removeCollisionObject();

    setCollisionMode(CollisionMode::GHOST);
    setCollisionShape(mode);

    if (getCollisionShape() == CollisionShape::SIMPLE_SHAPE) {
        makeSimpleGhostBody(
            simpleShapeSize,
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::AllFilter,
            EngineSetup::collisionGroups::AllFilter
        );
    }
}

void Collider::setupKinematicCollider()
{
    Logging::Message("[Collider] setupKinematicCollider");

    removeCollisionObject();

    setCollisionMode(CollisionMode::KINEMATIC);
    setCollisionShape(CollisionShape::SIMPLE_SHAPE);

    makeKineticBody(
        kinematicCapsuleSize.x,
        kinematicCapsuleSize.y,
        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
        EngineSetup::collisionGroups::AllFilter,
        EngineSetup::collisionGroups::AllFilter
    );
}

void Collider::setupRigidBodyCollider(CollisionShape shapeMode)
{
    Logging::Message("[Collider] setupRigidBodyCollider");

    removeCollisionObject();

    setCollisionMode(CollisionMode::BODY);
    setCollisionShape(shapeMode);

    if (getCollisionShape() == CollisionShape::SIMPLE_SHAPE) {
        makeSimpleRigidBody(
            mass,
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::AllFilter,
            EngineSetup::collisionGroups::AllFilter
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

void Collider::makeKineticBody(
    float x,
    float y,
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

void Collider::applyCentralForce(Vertex3D f)
{
    if (getCollisionMode() != CollisionMode::BODY) return;
    body->applyCentralForce(f.toBullet());
}

void Collider::applyCentralImpulse(Vertex3D f)
{
    if (getCollisionMode() != CollisionMode::BODY) return;
    body->applyCentralImpulse(f.toBullet());
}

void Collider::applyImpulse(Vertex3D f, Vertex3D rel)
{
    btVector3 impulse = f.toBullet();
    btVector3 rel_pos = rel.toBullet();
    body->applyImpulse(impulse, rel_pos);
}

void Collider::setLinearVelocity(Vertex3D f)
{
    body->setLinearVelocity(f.toBullet());
}

bool Collider::isColliderStatic() const {
    return colliderStatic;
}

void Collider::setColliderStatic(bool colliderStatic) {
    Collider::colliderStatic = colliderStatic;
}

void Collider::UpdateShape()
{
    if (getCollisionMode() == GHOST) {
        setupGhostCollider(getCollisionShape());
    }
    if (getCollisionMode() == BODY) {
        setupRigidBodyCollider(getCollisionShape());
    }
    if (getCollisionMode() == KINEMATIC) {
        setupKinematicCollider();
    }
}

void Collider::setKinematicSize(float x, float y)
{
    kinematicCapsuleSize.x = x;
    kinematicCapsuleSize.y = y;
}

void Collider::setWalkingDirection(Vertex3D d)
{
    if (getCollisionMode() == KINEMATIC) {
        characterController->setWalkDirection(d.toBullet());
    }
}

void Collider::jump(Vertex3D d)
{
    if (getCollisionMode() == KINEMATIC) {
        characterController->jump(d.toBullet());
    }
}

bool Collider::onGround()
{
    if (getCollisionMode() == KINEMATIC) {
        return characterController->onGround();
    }

    return false;
}
