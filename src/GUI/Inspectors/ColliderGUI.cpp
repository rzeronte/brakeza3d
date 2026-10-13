//
// Created by darkh on 05/02/2026.
//

#include "../../../include/GUI/Inspectors/ColliderGUI.h"
#include "../../../include/3D/Object3D.h"
#include "../../../include/Config.h"
#include "../../../include/GUI/Widgets/GUIFields.h"
#include <imgui.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>

void ColliderGUI::DrawColliderWorldVariables(Object3D *o)
{
        bool dbg = o->isDebugDraw();
        if (ImGui::Checkbox("Debug Draw", &dbg))
            o->setDebugDraw(dbg);
        ImGui::Separator();

        if (o->getCollisionMode() == BODY) {

            // ========================================
            // GENERAL INFO
            // ========================================
            if (GUIFields::BeginSection("General Info", 200.0f)) {

                GUIFields::Row("Mass");
                float mass = o->body->getMass();
                ImVec4 massColor = (mass <= 0) ? ImVec4(1.0f, 0.5f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                ImGui::TextColored(massColor, "%.3f %s", mass, (mass <= 0 ? "(Static)" : ""));

                GUIFields::Row("Activation State");
                int state = o->body->getActivationState();
                const char* stateNames[] = {"ACTIVE", "ISLAND_SLEEPING", "WANTS_DEACTIVATION", "DISABLE_DEACTIVATION", "DISABLE_SIMULATION"};
                ImGui::Text("%d - %s", state, (state >= 1 && state <= 5) ? stateNames[state-1] : "UNKNOWN");

                GUIFields::Row("Collision Flags");
                int colFlags = o->body->getCollisionFlags();
                std::string flagStr = "";
                if (colFlags & btCollisionObject::CF_STATIC_OBJECT) flagStr += "STATIC | ";
                if (colFlags & btCollisionObject::CF_KINEMATIC_OBJECT) flagStr += "KINEMATIC | ";
                if (colFlags & btCollisionObject::CF_NO_CONTACT_RESPONSE) flagStr += "NO_CONTACT | ";
                if (colFlags & btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK) flagStr += "CUSTOM_CALLBACK | ";
                if (colFlags & btCollisionObject::CF_CHARACTER_OBJECT) flagStr += "CHARACTER | ";
                if (flagStr.empty()) flagStr = "NONE";
                else flagStr = flagStr.substr(0, flagStr.length() - 3); // Remove last " | "
                ImGui::TextWrapped("%s", flagStr.c_str());

                GUIFields::Text("Contact Processing Threshold", "%.3f", o->body->getContactProcessingThreshold());

                GUIFields::Text("Deactivation Time", "%.3f", o->body->getDeactivationTime());

                GUIFields::EndSection();
            }

            // ========================================
            // MATERIAL PROPERTIES
            // ========================================
            if (GUIFields::BeginSection("Material Properties", 200.0f)) {

                GUIFields::Text("Friction", "%.3f", o->body->getFriction());

                GUIFields::Text("Restitution (Bounciness)", "%.3f", o->body->getRestitution());

                GUIFields::Text("Rolling Friction", "%.3f", o->body->getRollingFriction());

                GUIFields::Text("Spinning Friction", "%.3f", o->body->getSpinningFriction());

                GUIFields::Text("Hit Fraction", "%.3f", o->body->getHitFraction());

                GUIFields::EndSection();
            }

            // ========================================
            // SHAPE INFO
            // ========================================
            if (GUIFields::BeginSection("Collision Shape", 200.0f)) {

                GUIFields::Row("Shape Type");
                int shapeType = o->body->getCollisionShape()->getShapeType();
                const char* shapeNames[] = {"Box", "Sphere", "Capsule", "Cone", "Cylinder", "Compound", "Mesh", "Convex", "Plane"};
                ImGui::Text("%d (%s)", shapeType, shapeType < 9 ? shapeNames[shapeType] : "Other");

                // Solo mostrar margin para shapes que realmente lo usan (no cápsulas ni esferas)
                if (shapeType != CAPSULE_SHAPE_PROXYTYPE && shapeType != SPHERE_SHAPE_PROXYTYPE) {
                    GUIFields::Row("Collision Margin");
                    float margin = o->body->getCollisionShape()->getMargin();
                    ImVec4 marginColor = (margin > 0.05f) ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    ImGui::TextColored(marginColor, "%.4f %s", margin, (margin > 0.05f ? "(High!)" : ""));
                }

                GUIFields::EndSection();
            }

            // ========================================
            // VELOCITY & MOTION
            // ========================================
            if (GUIFields::BeginSection("Velocity & Motion", 200.0f)) {

                auto lv = o->body->getLinearVelocity();
                GUIFields::Row("Linear Velocity");
                float speed = lv.length();
                ImGui::Text("(%.3f, %.3f, %.3f) | Speed: %.3f", lv.x(), lv.y(), lv.z(), speed);

                auto av = o->body->getAngularVelocity();
                GUIFields::Text("Angular Velocity", "(%.3f, %.3f, %.3f)", av.x(), av.y(), av.z());

                auto pv = o->body->getPushVelocity();
                GUIFields::Text("Push Velocity", "(%.3f, %.3f, %.3f)", pv.x(), pv.y(), pv.z());

                GUIFields::EndSection();
            }

            // ========================================
            // DAMPING & FACTORS
            // ========================================
            if (GUIFields::BeginSection("Damping & Factors", 200.0f)) {

                GUIFields::Text("Linear Damping", "%.3f", o->body->getLinearDamping());

                GUIFields::Text("Angular Damping", "%.3f", o->body->getAngularDamping());

                auto lf = o->body->getLinearFactor();
                GUIFields::Text("Linear Factor", "(%.3f, %.3f, %.3f)", lf.x(), lf.y(), lf.z());

                auto af = o->body->getAngularFactor();
                GUIFields::Text("Angular Factor", "(%.3f, %.3f, %.3f)", af.x(), af.y(), af.z());

                GUIFields::EndSection();
            }

            // ========================================
            // CONTINUOUS COLLISION DETECTION (CCD)
            // ========================================
            if (GUIFields::BeginSection("Continuous Collision Detection", 200.0f)) {

                GUIFields::Row("CCD Motion Threshold");
                float ccdThreshold = o->body->getCcdMotionThreshold();
                ImVec4 ccdColor = (ccdThreshold > 0) ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
                ImGui::TextColored(ccdColor, "%.3f %s", ccdThreshold, (ccdThreshold > 0 ? "(Enabled)" : "(Disabled)"));

                GUIFields::Text("CCD Swept Sphere Radius", "%.3f", o->body->getCcdSweptSphereRadius());

                GUIFields::EndSection();
            }

            // ========================================
            // PHYSICS PROPERTIES
            // ========================================
            if (GUIFields::BeginSection("Physics Properties", 200.0f)) {

                auto g = o->body->getGravity();
                GUIFields::Text("Gravity", "(%.3f, %.3f, %.3f)", g.x(), g.y(), g.z());

                auto li = o->body->getLocalInertia();
                GUIFields::Text("Local Inertia", "(%.3f, %.3f, %.3f)", li.x(), li.y(), li.z());

                GUIFields::Text("Linear Sleeping Threshold", "%.3f", o->body->getLinearSleepingThreshold());

                GUIFields::Text("Angular Sleeping Threshold", "%.3f", o->body->getAngularSleepingThreshold());

                GUIFields::EndSection();
            }

            // ========================================
            // TRANSFORM INFO
            // ========================================
            if (GUIFields::BeginSection("Transform", 200.0f)) {

                btTransform trans = o->body->getWorldTransform();
                btVector3 pos = trans.getOrigin();
                GUIFields::Text("World Position", "(%.3f, %.3f, %.3f)", pos.x(), pos.y(), pos.z());

                btQuaternion rot = trans.getRotation();
                GUIFields::Text("World Rotation (Quat)", "(%.3f, %.3f, %.3f, %.3f)", rot.x(), rot.y(), rot.z(), rot.w());

                GUIFields::EndSection();
            }

        } else if (o->getCollisionMode() == GHOST) {

            // ========================================
            // GHOST OBJECT INFO
            // ========================================
            if (GUIFields::BeginSection("Ghost Object Info", 200.0f)) {

                GUIFields::Row("Activation State");
                int state = o->ghostObject->getActivationState();
                const char* stateNames[] = {"ACTIVE", "ISLAND_SLEEPING", "WANTS_DEACTIVATION", "DISABLE_DEACTIVATION", "DISABLE_SIMULATION"};
                ImGui::Text("%d - %s", state, (state >= 1 && state <= 5) ? stateNames[state-1] : "UNKNOWN");

                GUIFields::Text("Friction", "%.3f", o->ghostObject->getFriction());

                GUIFields::Text("Restitution", "%.3f", o->ghostObject->getRestitution());

                GUIFields::Text("Rolling Friction", "%.3f", o->ghostObject->getRollingFriction());

                GUIFields::Text("Spinning Friction", "%.3f", o->ghostObject->getSpinningFriction());

                GUIFields::Text("Hit Fraction", "%.3f", o->ghostObject->getHitFraction());

                // Solo mostrar margin para shapes que realmente lo usan (no cápsulas ni esferas)
                int ghostShapeType = o->ghostObject->getCollisionShape()->getShapeType();
                if (ghostShapeType != CAPSULE_SHAPE_PROXYTYPE && ghostShapeType != SPHERE_SHAPE_PROXYTYPE) {
                    GUIFields::Row("Collision Shape Margin");
                    float margin = o->ghostObject->getCollisionShape()->getMargin();
                    ImVec4 marginColor = (margin > 0.05f) ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    ImGui::TextColored(marginColor, "%.4f %s", margin, (margin > 0.05f ? "(High!)" : ""));
                }

                GUIFields::Row("CCD Motion Threshold");
                float ccdThreshold = o->ghostObject->getCcdMotionThreshold();
                ImVec4 ccdColor = (ccdThreshold > 0) ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
                ImGui::TextColored(ccdColor, "%.3f %s", ccdThreshold, (ccdThreshold > 0 ? "(Enabled)" : "(Disabled)"));

                GUIFields::Text("CCD Swept Sphere Radius", "%.3f", o->ghostObject->getCcdSweptSphereRadius());

                GUIFields::Text("Deactivation Time", "%.3f", o->ghostObject->getDeactivationTime());

                GUIFields::Row("Num Overlapping Objects");
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%d", o->ghostObject->getNumOverlappingObjects());

                GUIFields::EndSection();
            }
        }
}

void ColliderGUI::DrawPopUpColliderSetting(Object3D *o)
{
    using namespace GUIFields;

    if (o->getCollisionMode() == BODY) {
        if (BeginSection("BODY")) {
            Check("Collider static", &o->colliderStatic, "A static collider never moves (infinite mass).");
            if (!o->colliderStatic) {
                Float("Mass", &o->mass, 0.1f, 0.0f, 5000.0f);
            }
            Float("CCD motion threshold", &o->ccdMotionThreshold, 0.001f, 0.0f, 5.0f, "%.3f", "Continuous collision detection: speed above which swept tests are used (0 = off).");
            Float("CCD swept sphere", &o->ccdSweptSphereRadius, 0.001f, 0.0f, 5.0f, "%.3f", "Radius of the sphere used by continuous collision detection.");
            EndSection();
        }
    }

    if (o->getCollisionShape() == SIMPLE_SHAPE) {
        if (BeginSection("SHAPE")) {
            float vec3f[3];
            o->simpleShapeSize.toFloat(vec3f);
            if (Float3("Shape size", vec3f, 0.01f, -1000.0f, 1000.0f)) {
                o->simpleShapeSize.x = vec3f[0];
                o->simpleShapeSize.y = vec3f[1];
                o->simpleShapeSize.z = vec3f[2];
            }
            EndSection();
        }
    }

    if (o->getCollisionMode() == KINEMATIC || o->getCollisionShape() == CAPSULE_SHAPE) {
        if (BeginSection("CAPSULE")) {
            Float("Radius", &o->kinematicCapsuleSize.x, 0.1f, 0.0f, 1000.0f, "%.2f");
            Float("Height", &o->kinematicCapsuleSize.y, 0.1f, 0.0f, 1000.0f, "%.2f");
            EndSection();
        }
    }

    if (o->getCollisionMode() == BODY) {
        if (BeginSection("PHYSICS")) {
            float vec3f[3];
            o->angularFactor.toFloat(vec3f);
            if (Float3("Angular factor", vec3f, 0.01f, 0.0f, 1.0f, "%.2f", "Per-axis rotation allowed (0 locks that axis).")) {
                o->angularFactor.x = vec3f[0];
                o->angularFactor.y = vec3f[1];
                o->angularFactor.z = vec3f[2];
            }
            o->linearFactor.toFloat(vec3f);
            if (Float3("Linear factor", vec3f, 0.01f, 0.0f, 1.0f, "%.2f", "Per-axis movement allowed (0 locks that axis).")) {
                o->linearFactor.x = vec3f[0];
                o->linearFactor.y = vec3f[1];
                o->linearFactor.z = vec3f[2];
            }
            Float("Margin", &o->shapeMargin, 0.01f, 0.0f, 1.0f);
            Float("Friction", &o->friction, 0.01f, 0.0f, 1.0f);
            Float("Linear damping", &o->linearDamping, 0.01f, 0.0f, 1.0f);
            Float("Angular damping", &o->angularDamping, 0.01f, 0.0f, 1.0f);
            Float("Restitution", &o->restitution, 0.01f, 0.0f, 1.0f, "%.2f", "Bounciness.");
            EndSection();
        }
    }

    ImGui::Separator();
    ImGui::SeparatorText("Collision Filters");

    struct GroupEntry { const char* label; int bit; };
    static const GroupEntry groups[] = {
        { "Player",         Config::collisionGroups::Player          },
        { "Enemy",          Config::collisionGroups::Enemy           },
        { "Projectile",     Config::collisionGroups::Projectile      },
        { "ProjEnemy",      Config::collisionGroups::ProjectileEnemy },
        { "Health",         Config::collisionGroups::Health          },
        { "Weapon",         Config::collisionGroups::Weapon          },
        { "StaticWorld",    Config::collisionGroups::StaticWorld     },
    };

    ImGui::Text("Group:");
    ImGui::SameLine();
    for (auto& g : groups) {
        bool active = (o->collisionGroup & g.bit) != 0;
        if (ImGui::Checkbox(("##G_" + std::string(g.label)).c_str(), &active)) {
            if (active) o->collisionGroup |= g.bit;
            else        o->collisionGroup &= ~g.bit;
        }
        ImGui::SameLine();
        ImGui::Text("%s", g.label);
        ImGui::SameLine(0, 12);
    }
    ImGui::NewLine();

    ImGui::Text("Mask: ");
    ImGui::SameLine();
    for (auto& g : groups) {
        bool active = (o->collisionMask & g.bit) != 0;
        if (ImGui::Checkbox(("##M_" + std::string(g.label)).c_str(), &active)) {
            if (active) o->collisionMask |= g.bit;
            else        o->collisionMask &= ~g.bit;
        }
        ImGui::SameLine();
        ImGui::Text("%s", g.label);
        ImGui::SameLine(0, 12);
    }
    ImGui::NewLine();
}
