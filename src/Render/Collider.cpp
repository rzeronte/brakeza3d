
#include "../../include/Render/Collider.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Brakeza.h"

Collider::Collider()
:
    collisionsEnabled(false),
    collisionMode(CollisionMode::NONE),
    simpleShapeSize(Vertex3D(1, 1, 1)),
    angularFactor(Vertex3D(1, 1, 1)),
    linearFactor(Vertex3D(1, 1, 1)),
    ghostObject(nullptr),
    body(nullptr),
    colliderStatic(false),
    mass(0),
    kinematicBody(nullptr),
    friction(1),
    linearDamping(1),
    angularDamping(1),
    restitution(0),
    shapeMargin(0.004f),
    ccdMotionThreshold(0.0),
    ccdSweptSphereRadius(0.0),
    kinematicCapsuleSize(
        Config::get()->PLAYER_CAPSULE_RADIUS,
        Config::get()->PLAYER_CAPSULE_HEIGHT
    ),
    collisionShape(CollisionShape::SIMPLE_SHAPE)
{
}

void Collider::Integrate()
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

void Collider::ResolveCollision(CollisionInfo o)
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

void Collider::makeSimpleGhostBody(
    Vertex3D position,
    glm::mat4 modelMatrix,
    Vertex3D dimensions,
    btDiscreteDynamicsWorld *world,
    int collisionGroup,
    int collisionMask
) {
    Logging::Message("[Collider] makeSimpleGhostBody");

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(position.toBullet());

    btConvexHullShape *convexHullShape = nullptr;

    if (getCollisionShape() == SIMPLE_SHAPE) {
        convexHullShape = reinterpret_cast<btConvexHullShape *>(new btBoxShape(dimensions.toBullet()));;
    }

    if (getCollisionShape() == CAPSULE) {
        convexHullShape = reinterpret_cast<btConvexHullShape *>(new btCapsuleShape(kinematicCapsuleSize.x, kinematicCapsuleSize.y));;
    }

    if (convexHullShape == nullptr) {
        printf("Collider Shape not valid!!. Exiting...");
        exit(-1);
    }

    convexHullShape->setMargin(shapeMargin);
    ghostObject = new btPairCachingGhostObject();
    ghostObject->setCollisionShape(convexHullShape);
    ghostObject->setWorldTransform(transformation);
    ghostObject->setUserPointer(this);
    ghostObject->setUserIndex(Config::CollisionSource::OBJECT_COLLIDER);
    ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
    world->addCollisionObject(ghostObject, collisionGroup, collisionMask);

    ghostObject->setWorldTransform(Tools::GLMMatrixToBulletTransform(modelMatrix));
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

    if (ImGui::BeginCombo("Mode", combo_preview_value, flags)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(items[n], is_selected)) {
                if (!is_selected) {
                    item_current_idx = n;
                    switch (n) {
                        default:
                        case 0: {
                            SetupGhostCollider(CollisionShape::SIMPLE_SHAPE);
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
    auto flags = ImGuiComboFlags_None;
    const char* items[] = { "SIMPLE", "CAPSULE" };
    int item_current_idx = (int) collisionShape;
    const char* combo_preview_value = items[item_current_idx];

    auto comboTitle = "Shape###";
    if (ImGui::BeginCombo(comboTitle, combo_preview_value, flags)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(items[n], is_selected)) {
                if (!is_selected) {
                    item_current_idx = n;
                    switch (n) {
                        case 0: {
                            if (collisionMode == CollisionMode::GHOST) {
                                SetupGhostCollider(CollisionShape::SIMPLE_SHAPE);
                            }

                            if (collisionMode == CollisionMode::BODY) {
                                setupRigidBodyCollider(CollisionShape::SIMPLE_SHAPE);
                            }

                            break;
                        }
                        case 1: {
                            if (collisionMode == CollisionMode::GHOST) {
                                SetupGhostCollider(CollisionShape::CAPSULE);
                            }

                            if (collisionMode == CollisionMode::BODY) {
                                setupRigidBodyCollider(CollisionShape::CAPSULE);
                            }

                            break;
                        }
                    }
                }
            }
        }
        ImGui::EndCombo();
    }
}

void Collider::setupKinematicCollider()
{
    Logging::Message("[Collider] setupKinematicCollider");

    removeCollisionObject();

    setCollisionMode(CollisionMode::KINEMATIC);
    setCollisionShape(CollisionShape::SIMPLE_SHAPE);

    MakeKineticBody(
        kinematicCapsuleSize.x,
        kinematicCapsuleSize.y,
        Brakeza::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
        Config::collisionGroups::AllFilter,
        Config::collisionGroups::AllFilter
    );
}

void Collider::setupRigidBodyCollider(CollisionShape shapeMode)
{
    Logging::Message("[Collider] setupRigidBodyCollider");

    removeCollisionObject();

    setCollisionMode(CollisionMode::BODY);
    setCollisionShape(shapeMode);

    MakeSimpleRigidBody(
        mass,
        Brakeza::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
        Config::collisionGroups::AllFilter,
        Config::collisionGroups::AllFilter
    );
}

void Collider::MakeSimpleRigidBody(
    float mass,
    btDiscreteDynamicsWorld *world,
    int collisionGroup,
    int collisionMask
) {
}

void Collider::MakeKineticBody(
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

void Collider::setAngularVelocity(Vertex3D f)
{
    body->setAngularVelocity(f.toBullet());
}

bool Collider::isColliderStatic() const {
    return colliderStatic;
}

void Collider::setColliderStatic(bool colliderStatic) {
    Collider::colliderStatic = colliderStatic;
}

void Collider::UpdateShapeCollider()
{
    if (getCollisionMode() == GHOST) {
        SetupGhostCollider(getCollisionShape());
    }
    if (getCollisionMode() == BODY) {
        setupRigidBodyCollider(getCollisionShape());
    }
    if (getCollisionMode() == KINEMATIC) {
        setupKinematicCollider();
    }
}

void Collider::setCapsuleColliderSize(float x, float y)
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

void Collider::setAngularFactor(const Vertex3D &angularFactor)
{
    Collider::angularFactor = angularFactor;
}

void Collider::setFriction(float friction)
{
    Collider::friction = friction;
}

Vertex3D Collider::getLinearVelocity() const
{
    if (getCollisionMode() == BODY) {
        return Vertex3D::fromBullet(body->getLinearVelocity());
    }

    return Vertex3D::zero();
}

void Collider::setLinearDamping(float linearDamping) {
    Collider::linearDamping = linearDamping;
}

void Collider::setAngularDamping(float angularDamping) {
    Collider::angularDamping = angularDamping;
}

void Collider::setRestitution(float restitution) {
    Collider::restitution = restitution;
}

void Collider::setShapeMargin(float shapeMargin) {
    Collider::shapeMargin = shapeMargin;
}

void Collider::setCcdMotionThreshold(float ccdMotionThreshold) {
    Collider::ccdMotionThreshold = ccdMotionThreshold;
}

void Collider::setCcdSweptSphereRadius(float ccdSweptSphereRadius) {
    Collider::ccdSweptSphereRadius = ccdSweptSphereRadius;
}

void Collider::drawImGuiVariables()
{
    if (ImGui::TreeNode("World variables")) {
        if (getCollisionMode() == CollisionMode::BODY) {
            ImGui::Text("Activation State: %d", body->getActivationState());
            ImGui::Text("Mass: %f", body->getMass());
            ImGui::Text("Friction: %f", body->getFriction());
            ImGui::Text("Hit Fraction: %f", body->getHitFraction());
            ImGui::Text("Restitution: %f", body->getRestitution());
            ImGui::Text("Spinning Friction: %f", body->getSpinningFriction());
            ImGui::Text("Rolling Friction: %f", body->getRollingFriction());

            ImGui::Separator();

            ImGui::Text("Collision Shape Margin: %f", body->getCollisionShape()->getMargin());

            ImGui::Separator();

            auto g = body->getGravity();
            ImGui::Text("Gravity: %f, %f, %f", g.x(), g.y(), g.z());

            ImGui::Separator();
            auto lv = body->getLinearVelocity();
            ImGui::Text("Linear Velocity: %f, %f, %f", lv.x(), lv.y(), lv.z());

            auto av = body->getAngularVelocity();
            ImGui::Text("Angular Velocity: %f, %f, %f", av.x(), av.y(), av.z());

            ImGui::Separator();

            ImGui::Text("Linear Damping: %f", body->getLinearDamping());
            ImGui::Text("Angular Damping: %f", body->getAngularDamping());

            ImGui::Separator();

            auto af = body->getAngularFactor();
            ImGui::Text("Angular Factor: %f, %f, %f", af.x(), af.y(), af.z());

            auto lf = body->getLinearFactor();
            ImGui::Text("Linear Factor: %f, %f, %f", lf.x(), lf.y(), lf.z());

            ImGui::Separator();

            ImGui::Text("Linear Sleeping Threshold: %f", body->getLinearSleepingThreshold());
            ImGui::Text("Angular Sleeping Threshold: %f", body->getAngularSleepingThreshold());

            ImGui::Separator();

            ImGui::Text("Ccd Motion Threshold: %f", body->getCcdMotionThreshold());
            ImGui::Text("Ccd Swept Sphere Radius: %f", body->getCcdSweptSphereRadius());

            ImGui::Separator();
            auto li = body->getLocalInertia();
            ImGui::Text("Local Inertia: %f, %f, %f", li.x(), li.y(), li.z());

            auto pv = body->getPushVelocity();
            ImGui::Text("Push Velocity: %f, %f, %f", pv.x(), pv.y(), pv.z());
            ImGui::Separator();
            ImGui::Text("Deactivation time: %f", body->getDeactivationTime());
        }
        if (getCollisionMode() == CollisionMode::GHOST) {
            ImGui::Text("Activation State: %d", ghostObject->getActivationState());
            ImGui::Text("Friction: %f", ghostObject->getFriction());
            ImGui::Text("Hit Fraction: %f", ghostObject->getHitFraction());
            ImGui::Text("Restitution: %f", ghostObject->getRestitution());
            ImGui::Text("Spinning Friction: %f", ghostObject->getSpinningFriction());
            ImGui::Text("Rolling Friction: %f", ghostObject->getRollingFriction());

            ImGui::Separator();

            ImGui::Text("Collision Shape Margin: %f", ghostObject->getCollisionShape()->getMargin());
            ImGui::Separator();

            ImGui::Text("Ccd Motion Threshold: %f", ghostObject->getCcdMotionThreshold());
            ImGui::Text("Ccd Swept Sphere Radius: %f", ghostObject->getCcdSweptSphereRadius());

            ImGui::Separator();
            ImGui::Text("Deactivation time: %f", ghostObject->getDeactivationTime());
        }
    }
}

void Collider::sleepCollider()
{
    if (getCollisionMode() == CollisionMode::GHOST) {
        ghostObject->setActivationState(ISLAND_SLEEPING);
    }

    if (getCollisionMode() == CollisionMode::BODY) {
        body->clearForces(); // Elimina cualquier fuerza aplicada
        body->setLinearVelocity(btVector3(0, 0, 0)); // Detiene el movimiento
        body->setAngularVelocity(btVector3(0, 0, 0)); // Detiene la rotación
        body->setActivationState(ISLAND_SLEEPING);
    }

    if (getCollisionMode() == CollisionMode::KINEMATIC) {
        kinematicBody->setActivationState(ISLAND_SLEEPING);
    }
}

void Collider::disableSimulationCollider()
{
    if (getCollisionMode() == CollisionMode::GHOST) {
        ghostObject->forceActivationState(DISABLE_SIMULATION);
    }

    if (getCollisionMode() == CollisionMode::BODY) {
        body->forceActivationState(DISABLE_SIMULATION);
    }

    if (getCollisionMode() == CollisionMode::KINEMATIC) {
        kinematicBody->forceActivationState(DISABLE_SIMULATION);
    }
}

void Collider::enableSimulationCollider()
{
    if (getCollisionMode() == CollisionMode::GHOST) {
        ghostObject->forceActivationState(ACTIVE_TAG);
        ghostObject->activate(true);
    }

    if (getCollisionMode() == CollisionMode::BODY) {
        body->forceActivationState(ACTIVE_TAG);
        body->activate(true);
    }

    if (getCollisionMode() == CollisionMode::KINEMATIC) {
        kinematicBody->forceActivationState(ACTIVE_TAG);
        kinematicBody->activate(true);
    }
}

void Collider::disableDeactivationCollider()
{
    if (getCollisionMode() == CollisionMode::GHOST) {
        ghostObject->setActivationState(DISABLE_DEACTIVATION);
    }

    if (getCollisionMode() == CollisionMode::BODY) {
        body->setActivationState(DISABLE_DEACTIVATION);
    }

    if (getCollisionMode() == CollisionMode::KINEMATIC) {
        kinematicBody->setActivationState(DISABLE_DEACTIVATION);
    }
}

void Collider::setGravityCollider(Vertex3D g)
{
    if (getCollisionMode() == CollisionMode::BODY) {
        body->setGravity(g.toBullet());
    }
}

void Collider::setLinearFactor(Vertex3D linearFactor)
{
    Collider::linearFactor = linearFactor;
}

void Collider::setScalingCollider(Vertex3D v)
{
    if (getCollisionMode() == CollisionMode::BODY) {
        body->getCollisionShape()->setLocalScaling(v.toBullet());
    }
}

void Collider::moveCollider(Vertex3D v)
{
    if (getCollisionMode() == CollisionMode::BODY) {
        btTransform transform;
        body->getMotionState()->getWorldTransform(transform);  // Obtener la transformación actual

        btVector3 origin = transform.getOrigin() + v.toBullet();  // Obtener la posición actual

        transform.setOrigin(origin);  // Aplicar la nueva posición
        body->setWorldTransform(transform);  // Actualizar el cuerpo en Bullet
        body->getMotionState()->setWorldTransform(transform);  // Asegurar que el motor lo reconozca
        body->activate(true);  // Mantener el cuerpo activo
        btVector3 localInertia(0, 0, 0);
        body->getCollisionShape()->calculateLocalInertia(body->getMass(), localInertia); // Recalcular inercia con nueva masa
        body->setMassProps(body->getMass(), localInertia);
        body->updateInertiaTensor();
    }
}

void Collider::setSimpleShapeSize(const Vertex3D &simple_shape_size)
{
    simpleShapeSize = simple_shape_size;
}
