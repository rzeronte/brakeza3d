
#include "../../include/Render/Collider.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Brakeza.h"

Collider::Collider()
:
    simpleShapeSize(Vertex3D(1, 1, 1)),
    angularFactor(Vertex3D(1, 1, 1)),
    linearFactor(Vertex3D(1, 1, 1)),
    collisionsEnabled(false),
    collisionMode(CollisionMode::NONE),
    ghostObject(nullptr),
    body(nullptr),
    colliderStatic(false),
    mass(0),
    friction(1),
    linearDamping(1),
    angularDamping(1),
    restitution(0),
    shapeMargin(0.004f),
    ccdMotionThreshold(0.0),
    ccdSweptSphereRadius(0.0),
    kinematicBody(nullptr),
    kinematicCapsuleSize(
      Config::get()->PLAYER_CAPSULE_RADIUS,
      Config::get()->PLAYER_CAPSULE_HEIGHT
    ), characterController(nullptr),
collisionShape(SIMPLE_SHAPE) {
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

void Collider::RemoveCollisionObject()
{
    if (collisionMode == GHOST && ghostObject != nullptr) {
        LOG_MESSAGE("[Collider] Removing collider GHOST");
        setCollisionMode(NONE);
        Components::get()->Collisions()->getDynamicsWorld()->removeCollisionObject(ghostObject);
        ghostObject = nullptr;
        return;
    }

    if (collisionMode == BODY && body != nullptr) {
        LOG_MESSAGE("[Collider] Removing collider RIGIDBODY");
        setCollisionMode(NONE);
        Components::get()->Collisions()->getDynamicsWorld()->removeCollisionObject(body);
        body = nullptr;
        return;
    }

    if (collisionMode == KINEMATIC && kinematicBody != nullptr) {
        LOG_MESSAGE("[Collider] Removing collider KINEMATICBODY");
        setCollisionMode(NONE);
        Components::get()->Collisions()->getDynamicsWorld()->removeAction(characterController);
        Components::get()->Collisions()->getDynamicsWorld()->removeCollisionObject(kinematicBody);
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
    LOG_MESSAGE("[Collider] makeSimpleGhostBody");

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(position.toBullet());

    btConvexHullShape *convexHullShape = nullptr;

    if (getCollisionShape() == SIMPLE_SHAPE) {
        convexHullShape = reinterpret_cast<btConvexHullShape *>(new btBoxShape(dimensions.toBullet()));;
    }

    if (getCollisionShape() == CAPSULE_SHAPE) {
        convexHullShape = reinterpret_cast<btConvexHullShape *>(new btCapsuleShape(kinematicCapsuleSize.x, kinematicCapsuleSize.y));;
    }

    if (convexHullShape == nullptr) {
        LOG_ERROR("[Collider] Collider Shape not valid!! Exiting...");
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
    int item_current_idx = collisionMode;
    const char* combo_preview_value = items[item_current_idx];

    if (ImGui::BeginCombo("Mode", combo_preview_value, flags)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = item_current_idx == n;
            if (ImGui::Selectable(items[n], is_selected)) {
                if (!is_selected) {
                    item_current_idx = n;
                    switch (n) {
                        default:
                        case 0: {
                            SetupGhostCollider(SIMPLE_SHAPE);
                            break;
                        }
                        case 1: {
                            SetupRigidBodyCollider(SIMPLE_SHAPE);
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

void Collider::DrawImGuiCollisionShapeSelector()
{
    auto flags = ImGuiComboFlags_None;
    const char* items[] = { "SIMPLE", "CAPSULE_SHAPE" };
    int item_current_idx = collisionShape;
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
                            if (collisionMode == GHOST) {
                                SetupGhostCollider(SIMPLE_SHAPE);
                            }
                            if (collisionMode == BODY) {
                                SetupRigidBodyCollider(SIMPLE_SHAPE);
                            }
                            break;
                        }
                        case 1: {
                            if (collisionMode == GHOST) {
                                SetupGhostCollider(CAPSULE_SHAPE);
                            }
                            if (collisionMode == BODY) {
                                SetupRigidBodyCollider(CAPSULE_SHAPE);
                            }
                            break;
                        }
                        default:
                            LOG_ERROR("[Collider ] Error selection shape collision");
                            break;
                    }
                }
            }
        }
        ImGui::EndCombo();
    }
}

void Collider::setupKinematicCollider()
{
    LOG_MESSAGE("[Collider] setupKinematicCollider");

    RemoveCollisionObject();

    setCollisionMode(KINEMATIC);
    setCollisionShape(CAPSULE_SHAPE);

    MakeKineticBody(
        kinematicCapsuleSize.x,
        kinematicCapsuleSize.y,
        Brakeza::get()->getComponentsManager()->Collisions()->getDynamicsWorld(),
        Config::collisionGroups::AllFilter,
        Config::collisionGroups::AllFilter
    );
}

void Collider::SetupRigidBodyCollider(CollisionShape shapeMode)
{
    LOG_MESSAGE("[Collider] SetupRigidBodyCollider");

    RemoveCollisionObject();

    setCollisionMode(BODY);
    setCollisionShape(shapeMode);

    MakeSimpleRigidBody(
        mass,
        Brakeza::get()->getComponentsManager()->Collisions()->getDynamicsWorld(),
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

void Collider::ApplyCentralForce(Vertex3D f)
{
    if (getCollisionMode() != CollisionMode::BODY) return;
    body->applyCentralForce(f.toBullet());
}

void Collider::ApplyCentralImpulse(Vertex3D f)
{
    if (getCollisionMode() != CollisionMode::BODY) return;
    body->applyCentralImpulse(f.toBullet());
}

void Collider::ApplyImpulse(Vertex3D f, Vertex3D rel)
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
        SetupRigidBodyCollider(getCollisionShape());
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

void Collider::setWalkingDirection(Vertex3D d) const
{
    if (getCollisionMode() == KINEMATIC) {
        characterController->setWalkDirection(d.toBullet());
    }
}

void Collider::Jump(Vertex3D d) const
{
    if (getCollisionMode() == KINEMATIC) {
        characterController->jump(d.toBullet());
    }
}

bool Collider::onGround() const
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
    if (body != nullptr && body->getCollisionShape() != nullptr) {
        body->getCollisionShape()->setMargin(shapeMargin);
    }
    if (ghostObject != nullptr && ghostObject->getCollisionShape() != nullptr) {
        ghostObject->getCollisionShape()->setMargin(shapeMargin);
    }
}

void Collider::setCcdMotionThreshold(float ccdMotionThreshold) {
    Collider::ccdMotionThreshold = ccdMotionThreshold;
    if (body != nullptr) {
        body->setCcdMotionThreshold(ccdMotionThreshold);
    }
}

void Collider::setCcdSweptSphereRadius(float ccdSweptSphereRadius) {
    Collider::ccdSweptSphereRadius = ccdSweptSphereRadius;
    if (body != nullptr) {
        body->setCcdSweptSphereRadius(ccdSweptSphereRadius);
    }
}

void Collider::drawWorldPhysicVariables()
{
    if (ImGui::TreeNodeEx("World variables", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {

        static ImGuiTableFlags flags = ImGuiTableFlags_BordersInnerV |
                                       ImGuiTableFlags_BordersOuterV |
                                       ImGuiTableFlags_RowBg |
                                       ImGuiTableFlags_SizingStretchProp;

        if (getCollisionMode() == BODY) {

            // ========================================
            // GENERAL INFO
            // ========================================
            ImGui::SeparatorText("General Info");
            if (ImGui::BeginTable("GeneralInfo", 2, flags)) {
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Mass");
                ImGui::TableSetColumnIndex(1);
                float mass = body->getMass();
                ImVec4 massColor = (mass <= 0) ? ImVec4(1.0f, 0.5f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                ImGui::TextColored(massColor, "%.3f %s", mass, (mass <= 0 ? "(Static)" : ""));

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Activation State");
                ImGui::TableSetColumnIndex(1);
                int state = body->getActivationState();
                const char* stateNames[] = {"ACTIVE", "ISLAND_SLEEPING", "WANTS_DEACTIVATION", "DISABLE_DEACTIVATION", "DISABLE_SIMULATION"};
                ImGui::Text("%d - %s", state, (state >= 1 && state <= 5) ? stateNames[state-1] : "UNKNOWN");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Collision Flags");
                ImGui::TableSetColumnIndex(1);
                int colFlags = body->getCollisionFlags();
                std::string flagStr = "";
                if (colFlags & btCollisionObject::CF_STATIC_OBJECT) flagStr += "STATIC | ";
                if (colFlags & btCollisionObject::CF_KINEMATIC_OBJECT) flagStr += "KINEMATIC | ";
                if (colFlags & btCollisionObject::CF_NO_CONTACT_RESPONSE) flagStr += "NO_CONTACT | ";
                if (colFlags & btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK) flagStr += "CUSTOM_CALLBACK | ";
                if (colFlags & btCollisionObject::CF_CHARACTER_OBJECT) flagStr += "CHARACTER | ";
                if (flagStr.empty()) flagStr = "NONE";
                else flagStr = flagStr.substr(0, flagStr.length() - 3); // Remove last " | "
                ImGui::TextWrapped("%s", flagStr.c_str());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Contact Processing Threshold");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", body->getContactProcessingThreshold());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Deactivation Time");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", body->getDeactivationTime());

                ImGui::EndTable();
            }

            // ========================================
            // MATERIAL PROPERTIES
            // ========================================
            ImGui::SeparatorText("Material Properties");
            if (ImGui::BeginTable("MaterialProps", 2, flags)) {
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Friction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", body->getFriction());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Restitution (Bounciness)");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", body->getRestitution());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Rolling Friction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", body->getRollingFriction());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Spinning Friction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", body->getSpinningFriction());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Hit Fraction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", body->getHitFraction());

                ImGui::EndTable();
            }

            // ========================================
            // SHAPE INFO
            // ========================================
            ImGui::SeparatorText("Collision Shape");
            if (ImGui::BeginTable("ShapeInfo", 2, flags)) {
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Shape Type");
                ImGui::TableSetColumnIndex(1);
                int shapeType = body->getCollisionShape()->getShapeType();
                const char* shapeNames[] = {"Box", "Sphere", "Capsule", "Cone", "Cylinder", "Compound", "Mesh", "Convex", "Plane"};
                ImGui::Text("%d (%s)", shapeType, shapeType < 9 ? shapeNames[shapeType] : "Other");

                // Solo mostrar margin para shapes que realmente lo usan (no cápsulas ni esferas)
                if (shapeType != CAPSULE_SHAPE_PROXYTYPE && shapeType != SPHERE_SHAPE_PROXYTYPE) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Collision Margin");
                    ImGui::TableSetColumnIndex(1);
                    float margin = body->getCollisionShape()->getMargin();
                    ImVec4 marginColor = (margin > 0.05f) ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    ImGui::TextColored(marginColor, "%.4f %s", margin, (margin > 0.05f ? "(High!)" : ""));
                }

                ImGui::EndTable();
            }

            // ========================================
            // VELOCITY & MOTION
            // ========================================
            ImGui::SeparatorText("Velocity & Motion");
            if (ImGui::BeginTable("Velocity", 2, flags)) {
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                auto lv = body->getLinearVelocity();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Linear Velocity");
                ImGui::TableSetColumnIndex(1);
                float speed = lv.length();
                ImGui::Text("(%.3f, %.3f, %.3f) | Speed: %.3f", lv.x(), lv.y(), lv.z(), speed);

                auto av = body->getAngularVelocity();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Angular Velocity");
                ImGui::TableSetColumnIndex(1); ImGui::Text("(%.3f, %.3f, %.3f)", av.x(), av.y(), av.z());

                auto pv = body->getPushVelocity();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Push Velocity");
                ImGui::TableSetColumnIndex(1); ImGui::Text("(%.3f, %.3f, %.3f)", pv.x(), pv.y(), pv.z());

                ImGui::EndTable();
            }

            // ========================================
            // DAMPING & FACTORS
            // ========================================
            ImGui::SeparatorText("Damping & Factors");
            if (ImGui::BeginTable("Damping", 2, flags)) {
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Linear Damping");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", body->getLinearDamping());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Angular Damping");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", body->getAngularDamping());

                auto lf = body->getLinearFactor();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Linear Factor");
                ImGui::TableSetColumnIndex(1); ImGui::Text("(%.3f, %.3f, %.3f)", lf.x(), lf.y(), lf.z());

                auto af = body->getAngularFactor();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Angular Factor");
                ImGui::TableSetColumnIndex(1); ImGui::Text("(%.3f, %.3f, %.3f)", af.x(), af.y(), af.z());

                ImGui::EndTable();
            }

            // ========================================
            // CONTINUOUS COLLISION DETECTION (CCD)
            // ========================================
            ImGui::SeparatorText("Continuous Collision Detection");
            if (ImGui::BeginTable("CCD", 2, flags)) {
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("CCD Motion Threshold");
                ImGui::TableSetColumnIndex(1);
                float ccdThreshold = body->getCcdMotionThreshold();
                ImVec4 ccdColor = (ccdThreshold > 0) ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
                ImGui::TextColored(ccdColor, "%.3f %s", ccdThreshold, (ccdThreshold > 0 ? "(Enabled)" : "(Disabled)"));

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("CCD Swept Sphere Radius");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", body->getCcdSweptSphereRadius());

                ImGui::EndTable();
            }

            // ========================================
            // PHYSICS PROPERTIES
            // ========================================
            ImGui::SeparatorText("Physics Properties");
            if (ImGui::BeginTable("Physics", 2, flags)) {
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                auto g = body->getGravity();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Gravity");
                ImGui::TableSetColumnIndex(1); ImGui::Text("(%.3f, %.3f, %.3f)", g.x(), g.y(), g.z());

                auto li = body->getLocalInertia();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Local Inertia");
                ImGui::TableSetColumnIndex(1); ImGui::Text("(%.3f, %.3f, %.3f)", li.x(), li.y(), li.z());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Linear Sleeping Threshold");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", body->getLinearSleepingThreshold());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Angular Sleeping Threshold");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", body->getAngularSleepingThreshold());

                ImGui::EndTable();
            }

            // ========================================
            // TRANSFORM INFO
            // ========================================
            ImGui::SeparatorText("Transform");
            if (ImGui::BeginTable("Transform", 2, flags)) {
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                btTransform trans = body->getWorldTransform();
                btVector3 pos = trans.getOrigin();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("World Position");
                ImGui::TableSetColumnIndex(1); ImGui::Text("(%.3f, %.3f, %.3f)", pos.x(), pos.y(), pos.z());

                btQuaternion rot = trans.getRotation();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("World Rotation (Quat)");
                ImGui::TableSetColumnIndex(1); ImGui::Text("(%.3f, %.3f, %.3f, %.3f)", rot.x(), rot.y(), rot.z(), rot.w());

                ImGui::EndTable();
            }

        } else if (getCollisionMode() == GHOST) {

            // ========================================
            // GHOST OBJECT INFO
            // ========================================
            ImGui::SeparatorText("Ghost Object Info");
            if (ImGui::BeginTable("GhostInfo", 2, flags)) {
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Activation State");
                ImGui::TableSetColumnIndex(1);
                int state = ghostObject->getActivationState();
                const char* stateNames[] = {"ACTIVE", "ISLAND_SLEEPING", "WANTS_DEACTIVATION", "DISABLE_DEACTIVATION", "DISABLE_SIMULATION"};
                ImGui::Text("%d - %s", state, (state >= 1 && state <= 5) ? stateNames[state-1] : "UNKNOWN");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Friction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", ghostObject->getFriction());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Restitution");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", ghostObject->getRestitution());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Rolling Friction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", ghostObject->getRollingFriction());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Spinning Friction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", ghostObject->getSpinningFriction());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Hit Fraction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", ghostObject->getHitFraction());

                // Solo mostrar margin para shapes que realmente lo usan (no cápsulas ni esferas)
                int ghostShapeType = ghostObject->getCollisionShape()->getShapeType();
                if (ghostShapeType != CAPSULE_SHAPE_PROXYTYPE && ghostShapeType != SPHERE_SHAPE_PROXYTYPE) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Collision Shape Margin");
                    ImGui::TableSetColumnIndex(1);
                    float margin = ghostObject->getCollisionShape()->getMargin();
                    ImVec4 marginColor = (margin > 0.05f) ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    ImGui::TextColored(marginColor, "%.4f %s", margin, (margin > 0.05f ? "(High!)" : ""));
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("CCD Motion Threshold");
                ImGui::TableSetColumnIndex(1);
                float ccdThreshold = ghostObject->getCcdMotionThreshold();
                ImVec4 ccdColor = (ccdThreshold > 0) ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
                ImGui::TextColored(ccdColor, "%.3f %s", ccdThreshold, (ccdThreshold > 0 ? "(Enabled)" : "(Disabled)"));

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("CCD Swept Sphere Radius");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", ghostObject->getCcdSweptSphereRadius());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Deactivation Time");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", ghostObject->getDeactivationTime());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Num Overlapping Objects");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%d", ghostObject->getNumOverlappingObjects());

                ImGui::EndTable();
            }
        }

        ImGui::TreePop();
    }
}

void Collider::SleepCollider() const
{
    if (getCollisionMode() == GHOST) {
        ghostObject->setActivationState(ISLAND_SLEEPING);
    }

    if (getCollisionMode() == BODY) {
        body->clearForces(); // Elimina cualquier fuerza aplicada
        body->setLinearVelocity(btVector3(0, 0, 0)); // Detiene el movimiento
        body->setAngularVelocity(btVector3(0, 0, 0)); // Detiene la rotación
        body->setActivationState(ISLAND_SLEEPING);
    }

    if (getCollisionMode() == KINEMATIC) {
        kinematicBody->setActivationState(ISLAND_SLEEPING);
    }
}

void Collider::DisableSimulationCollider() const
{
    if (getCollisionMode() == GHOST) {
        ghostObject->forceActivationState(DISABLE_SIMULATION);
    }

    if (getCollisionMode() == CollisionMode::BODY) {
        body->forceActivationState(DISABLE_SIMULATION);
    }

    if (getCollisionMode() == CollisionMode::KINEMATIC) {
        kinematicBody->forceActivationState(DISABLE_SIMULATION);
    }
}

void Collider::EnableSimulationCollider()
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

void Collider::DisableDeactivationCollider()
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
