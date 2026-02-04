//
// Created by darkh on 05/02/2026.
//

#include "ColliderGUI.h"
#include "../../3D/Object3D.h"
#include <imgui.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>

void ColliderGUI::DrawColliderWorldVariables(Object3D *o)
{
        static ImGuiTableFlags flags = ImGuiTableFlags_BordersInnerV |
                                       ImGuiTableFlags_BordersOuterV |
                                       ImGuiTableFlags_RowBg |
                                       ImGuiTableFlags_SizingStretchProp;

        if (o->getCollisionMode() == BODY) {

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
                float mass = o->body->getMass();
                ImVec4 massColor = (mass <= 0) ? ImVec4(1.0f, 0.5f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                ImGui::TextColored(massColor, "%.3f %s", mass, (mass <= 0 ? "(Static)" : ""));

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Activation State");
                ImGui::TableSetColumnIndex(1);
                int state = o->body->getActivationState();
                const char* stateNames[] = {"ACTIVE", "ISLAND_SLEEPING", "WANTS_DEACTIVATION", "DISABLE_DEACTIVATION", "DISABLE_SIMULATION"};
                ImGui::Text("%d - %s", state, (state >= 1 && state <= 5) ? stateNames[state-1] : "UNKNOWN");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Collision Flags");
                ImGui::TableSetColumnIndex(1);
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

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Contact Processing Threshold");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->body->getContactProcessingThreshold());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Deactivation Time");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->body->getDeactivationTime());

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
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->body->getFriction());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Restitution (Bounciness)");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->body->getRestitution());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Rolling Friction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->body->getRollingFriction());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Spinning Friction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->body->getSpinningFriction());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Hit Fraction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->body->getHitFraction());

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
                int shapeType = o->body->getCollisionShape()->getShapeType();
                const char* shapeNames[] = {"Box", "Sphere", "Capsule", "Cone", "Cylinder", "Compound", "Mesh", "Convex", "Plane"};
                ImGui::Text("%d (%s)", shapeType, shapeType < 9 ? shapeNames[shapeType] : "Other");

                // Solo mostrar margin para shapes que realmente lo usan (no cápsulas ni esferas)
                if (shapeType != CAPSULE_SHAPE_PROXYTYPE && shapeType != SPHERE_SHAPE_PROXYTYPE) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Collision Margin");
                    ImGui::TableSetColumnIndex(1);
                    float margin = o->body->getCollisionShape()->getMargin();
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

                auto lv = o->body->getLinearVelocity();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Linear Velocity");
                ImGui::TableSetColumnIndex(1);
                float speed = lv.length();
                ImGui::Text("(%.3f, %.3f, %.3f) | Speed: %.3f", lv.x(), lv.y(), lv.z(), speed);

                auto av = o->body->getAngularVelocity();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Angular Velocity");
                ImGui::TableSetColumnIndex(1); ImGui::Text("(%.3f, %.3f, %.3f)", av.x(), av.y(), av.z());

                auto pv = o->body->getPushVelocity();
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
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->body->getLinearDamping());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Angular Damping");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->body->getAngularDamping());

                auto lf = o->body->getLinearFactor();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Linear Factor");
                ImGui::TableSetColumnIndex(1); ImGui::Text("(%.3f, %.3f, %.3f)", lf.x(), lf.y(), lf.z());

                auto af = o->body->getAngularFactor();
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
                float ccdThreshold = o->body->getCcdMotionThreshold();
                ImVec4 ccdColor = (ccdThreshold > 0) ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
                ImGui::TextColored(ccdColor, "%.3f %s", ccdThreshold, (ccdThreshold > 0 ? "(Enabled)" : "(Disabled)"));

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("CCD Swept Sphere Radius");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->body->getCcdSweptSphereRadius());

                ImGui::EndTable();
            }

            // ========================================
            // PHYSICS PROPERTIES
            // ========================================
            ImGui::SeparatorText("Physics Properties");
            if (ImGui::BeginTable("Physics", 2, flags)) {
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                auto g = o->body->getGravity();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Gravity");
                ImGui::TableSetColumnIndex(1); ImGui::Text("(%.3f, %.3f, %.3f)", g.x(), g.y(), g.z());

                auto li = o->body->getLocalInertia();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Local Inertia");
                ImGui::TableSetColumnIndex(1); ImGui::Text("(%.3f, %.3f, %.3f)", li.x(), li.y(), li.z());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Linear Sleeping Threshold");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->body->getLinearSleepingThreshold());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Angular Sleeping Threshold");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->body->getAngularSleepingThreshold());

                ImGui::EndTable();
            }

            // ========================================
            // TRANSFORM INFO
            // ========================================
            ImGui::SeparatorText("Transform");
            if (ImGui::BeginTable("Transform", 2, flags)) {
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                btTransform trans = o->body->getWorldTransform();
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

        } else if (o->getCollisionMode() == GHOST) {

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
                int state = o->ghostObject->getActivationState();
                const char* stateNames[] = {"ACTIVE", "ISLAND_SLEEPING", "WANTS_DEACTIVATION", "DISABLE_DEACTIVATION", "DISABLE_SIMULATION"};
                ImGui::Text("%d - %s", state, (state >= 1 && state <= 5) ? stateNames[state-1] : "UNKNOWN");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Friction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->ghostObject->getFriction());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Restitution");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->ghostObject->getRestitution());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Rolling Friction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->ghostObject->getRollingFriction());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Spinning Friction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->ghostObject->getSpinningFriction());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Hit Fraction");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->ghostObject->getHitFraction());

                // Solo mostrar margin para shapes que realmente lo usan (no cápsulas ni esferas)
                int ghostShapeType = o->ghostObject->getCollisionShape()->getShapeType();
                if (ghostShapeType != CAPSULE_SHAPE_PROXYTYPE && ghostShapeType != SPHERE_SHAPE_PROXYTYPE) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("Collision Shape Margin");
                    ImGui::TableSetColumnIndex(1);
                    float margin = o->ghostObject->getCollisionShape()->getMargin();
                    ImVec4 marginColor = (margin > 0.05f) ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    ImGui::TextColored(marginColor, "%.4f %s", margin, (margin > 0.05f ? "(High!)" : ""));
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("CCD Motion Threshold");
                ImGui::TableSetColumnIndex(1);
                float ccdThreshold = o->ghostObject->getCcdMotionThreshold();
                ImVec4 ccdColor = (ccdThreshold > 0) ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
                ImGui::TextColored(ccdColor, "%.3f %s", ccdThreshold, (ccdThreshold > 0 ? "(Enabled)" : "(Disabled)"));

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("CCD Swept Sphere Radius");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->ghostObject->getCcdSweptSphereRadius());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Deactivation Time");
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.3f", o->ghostObject->getDeactivationTime());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Num Overlapping Objects");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%d", o->ghostObject->getNumOverlappingObjects());

                ImGui::EndTable();
            }
        }
}

void ColliderGUI::DrawPopUpColliderSetting(Object3D *o)
{
    if (o->getCollisionMode() == BODY) {
        ImGui::Checkbox("Collider static", &o->colliderStatic);
        ImGui::Separator();
        ImGui::DragFloat("CCD Motion Treshold", &o->ccdMotionThreshold, 0.001f, 0.0f, 5.0f);
        ImGui::Separator();
        ImGui::DragFloat("CCD Swept Sphere Radius", &o->ccdSweptSphereRadius, 0.001f, 0.0f, 5.0f);
    }

    if (o->getCollisionShape() == SIMPLE_SHAPE) {
        ImGui::Separator();
        float vec3f[3];
        o->simpleShapeSize.toFloat(vec3f);
        if (ImGui::DragFloat3("Shape Size", vec3f, 0.01f, -1000.0f, 1000.0f)) {
            o->simpleShapeSize.x = vec3f[0];
            o->simpleShapeSize.y = vec3f[1];
            o->simpleShapeSize.z = vec3f[2];
        }
    }

    if (o->getCollisionMode() == KINEMATIC || o->getCollisionShape() == CAPSULE_SHAPE) {
        ImGui::Separator();
        const float range_min = 0;
        const float range_max = 1000;
        ImGui::DragScalar("Capsule radius", ImGuiDataType_Float, &o->kinematicCapsuleSize.x, 0.1f, &range_min, &range_max, "%f", 1.0f);
        ImGui::DragScalar("Capsule height", ImGuiDataType_Float, &o->kinematicCapsuleSize.y, 0.1f, &range_min, &range_max, "%f", 1.0f);
    }

    if (o->getCollisionMode() == BODY) {
        ImGui::Separator();
        if (!o->colliderStatic) {
            ImGui::DragFloat("Mass", &o->mass, 0.1f, 0.0f, 5000.0f);
            ImGui::Separator();
        }

        float vec3f[3];
        o->angularFactor.toFloat(vec3f);
        if (ImGui::DragFloat3("Angular factor", vec3f, 0.01f, 0.0f, 1.0f)) {
            o->angularFactor.x = vec3f[0];
            o->angularFactor.y = vec3f[1];
            o->angularFactor.z = vec3f[2];
        }
        ImGui::Separator();
        o->linearFactor.toFloat(vec3f);
        if (ImGui::DragFloat3("Linear Factor", vec3f, 0.01f, 0.0f, 1.0f)) {
            o->linearFactor.x = vec3f[0];
            o->linearFactor.y = vec3f[1];
            o->linearFactor.z = vec3f[2];
        }
        ImGui::Separator();
        ImGui::DragFloat("Margin", &o->shapeMargin, 0.01f, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::DragFloat("Friction", &o->friction, 0.01f, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::DragFloat("Linear Damping", &o->linearDamping, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Angular Damping", &o->angularDamping, 0.01f, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::DragFloat("Restitution", &o->restitution, 0.01f, 0.0f, 1.0f);
    }
}
