//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_SWARMGUI_H
#define BRAKEZA3D_SWARMGUI_H

#include "../../Brakeza.h"
#include "../../3D/Swarm.h"
#include "../../Misc/ToolsMaths.h"

class SwarmGUI
{
public:
    static void DrawPropertiesGUI(Swarm *o)
    {

        static char name[256];
        strncpy(name, o->name.c_str(), sizeof(name));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(125.0f);
        ImGui::InputText("Name##nameObject", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AlwaysOverwrite);
        if (ImGui::IsItemEdited()) {
            o->setName(name);
        }
        ImGui::Separator();

        if (o->featuresGUI.position || o->featuresGUI.rotation || o->featuresGUI.scale) {
            if (ImGui::CollapsingHeader("Transformations")) {
                // position
                if (o->featuresGUI.position) {
                    float vec3f[3];
                    o->getPosition().toFloat(vec3f);
                    if (ImGui::DragFloat3("Position", vec3f, 0.01f, -999999.0f, 999999.0f)) {
                        o->position.x = vec3f[0];
                        o->position.y = vec3f[1];
                        o->position.z = vec3f[2];
                    }
                    ImGui::Separator();
                }

                // rotation
                if (o->featuresGUI.rotation) {
                    float oldPitch = o->getRotation().getPitchDegree();
                    float oldYaw = o->getRotation().getYawDegree();
                    float oldRoll = o->getRotation().getRollDegree();
                    float pitch = oldPitch;
                    float yaw = oldYaw;
                    float roll = oldRoll;

                    float vec3f[3];
                    vec3f[0] = pitch;
                    vec3f[1] = yaw;
                    vec3f[2] = roll;
                    if (ImGui::DragFloat3("Rotation", vec3f, 0.01f, -999999.0f, 999999.0f)) {
                        const float factor = 0.0025f;
                        pitch = vec3f[0];
                        yaw = vec3f[1];
                        roll = vec3f[2];
                        if (abs(pitch - oldPitch) > 0) {
                            auto partialRotX = M3::arbitraryAxis(o->getRotation().X(), ToolsMaths::RadiansToDegrees(pitch - oldPitch) * factor);
                            o->setRotation(o->getRotation() * partialRotX);
                            M3::normalize(o->rotation);
                        }

                        if (abs(yaw - oldYaw) > 0) {
                            auto partialRotY = M3::arbitraryAxis(o->getRotation().Y(), ToolsMaths::RadiansToDegrees(yaw - oldYaw) * factor);
                            o->setRotation(o->getRotation() * partialRotY);
                            M3::normalize(o->rotation);
                        }

                        if (abs(roll - oldRoll) > 0) {
                            auto partialRotZ = M3::arbitraryAxis(o->getRotation().Z(), ToolsMaths::RadiansToDegrees(roll - oldRoll) * factor);
                            o->setRotation(o->getRotation() * partialRotZ);
                            M3::normalize(o->rotation);
                        }
                    }
                    ImGui::Separator();

                    o->drawOffset.toFloat(vec3f);
                    if (ImGui::DragFloat3("DrawOffset", vec3f, 0.01f, -999999.0f, 999999.0f)) {
                        o->drawOffset.x = vec3f[0];
                        o->drawOffset.y = vec3f[1];
                        o->drawOffset.z = vec3f[2];
                    }
                    ImGui::Separator();
                }
                // scale
                if (o->featuresGUI.scale) {
                    const float range_scale_min = -360;
                    const float range_scale_max = 360;
                    ImGui::DragScalar("Scale", ImGuiDataType_Float, &o->scale, 0.01, &range_scale_min, &range_scale_max, "%f", 1.0f);
                }
            }
        }
        // alpha
        if (o->featuresGUI.alpha) {
            if (ImGui::CollapsingHeader("Alpha")) {
                const float range_alpha_min = 0;
                const float range_alpha_max = 1;

                ImGui::DragScalar("Alpha##001", ImGuiDataType_Float, &o->getAlpha(), 0.01, &range_alpha_min, &range_alpha_max, "%f", 1.0f);
            }
        }

        if (o->featuresGUI.attached) {
            if (ImGui::CollapsingHeader("Attached Objects")) {
                if (o->attachedObjects.empty() <= 0) {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Not objects found");
                }

                for (auto a: o->attachedObjects) {
                    if (ImGui::TreeNode(a->getName().c_str())) {
                        a->DrawPropertiesGUI();
                        ImGui::TreePop();
                    }
                }
            }
        }

        if (o->featuresGUI.attached) {
            if (ImGui::CollapsingHeader("Collider")) {
                if (ImGui::Checkbox("Enable collider", &o->collisionsEnabled)) {
                    if (!o->collisionsEnabled) {
                        o->RemoveCollisionObject();
                    } else {
                        o->SetupGhostCollider(SIMPLE_SHAPE);
                    }
                }

                if (o->collisionsEnabled) {
                    if (ImGui::Button(std::string("Update Collider").c_str())) {
                        o->UpdateShapeCollider();
                    }
                    ImGui::Separator();

                    o->drawImGuiCollisionModeSelector();
                    if (o->getCollisionMode() != KINEMATIC) {
                        o->DrawImGuiCollisionShapeSelector();
                    }

                    if (ImGui::TreeNode("Collider settings")) {
                        if (o->getCollisionMode() == CollisionMode::BODY) {
                            ImGui::Separator();
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

                            ImGui::DragScalar("Capsule radius", ImGuiDataType_Float, &o->kinematicCapsuleSize.x, 0.1 ,&range_min, &range_max, "%f", 1.0f);
                            ImGui::DragScalar("Capsule height", ImGuiDataType_Float, &o->kinematicCapsuleSize.y, 0.1 ,&range_min, &range_max, "%f", 1.0f);
                        }
                        ImGui::Separator();

                        if (o->getCollisionMode() == BODY) {
                            if (!o->colliderStatic) {
                                ImGui::DragFloat("Mass", &o->mass, 0.1f, 0.0f, 5000.0f);
                                ImGui::Separator();
                            }

                            if (o->getCollisionMode() == GHOST || o->getCollisionMode() == BODY) {
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
                        ImGui::TreePop();
                    }

                    o->drawWorldPhysicVariables();
                }
            }
            // alpha
            if (o->featuresGUI.misc) {
                if (ImGui::CollapsingHeader("Misc")) {
                    ImGui::Text( " %s: (%f, %f, %f)", "Picking Color", o->pickingColor.r, o->pickingColor.g, o->pickingColor.b);
                    ImGui::Text( " %s: %d", "ID", o->id);
                    ImGui::Text( " %s: %d", "BelongToScene", o->belongToScene ? 1 : 0);
                    ImGui::Text( " %s: %f", "Distance to camera", o->distanceToCamera);
                }
            }
        }
    }

};

#endif //BRAKEZA3D_SWARMGUI_H