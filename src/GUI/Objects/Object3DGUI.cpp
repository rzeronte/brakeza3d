//
// Created by Eduardo on 26/11/2025.
//

#include "../../../include/GUI/Objects/Object3DGUI.h"
#include "../../../include/Components/Components.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/Misc/Logging.h"
#include "../../../include/Misc/ToolsMaths.h"
#include "../include/Render/Drawable.h"

void Object3DGUI::DrawPropertiesGUI(Object3D *o)
{
    static int cont = 0;
    static char name[256];
    strncpy(name, o->name.c_str(), sizeof(name));
    ImGui::PushID(++cont);
    ImGui::Image(FileSystemGUI::Icon(o->getIcon()), ImVec2(22, 24));
    ImGui::PopID();
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
                ImGui::PushID("reset_position");
                GUI::DrawButton("Reset position", IconGUI::RESET, GUIType::Sizes::ICON_SIZE_SMALL, true, [&] {
                    LOG_MESSAGE("Reset Position");
                    o->setPosition(Vertex3D(0, 0, 0));
                });
                ImGui::PopID();
                ImGui::SameLine();

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
                // Edit-buffer pattern: avoid re-decomposing the matrix to Euler on every
                // frame while the user is dragging — that roundtrip is unstable near
                // gimbal-lock (pitch ~ ±90°) and causes the sliders to jump around.
                static const Object3D* editedObj = nullptr;
                static float           editBuf[3] = {0, 0, 0};
                static bool            wasActive  = false;

                ImGui::PushID("reset_rotation");
                GUI::DrawButton("Reset rotation", IconGUI::RESET, GUIType::Sizes::ICON_SIZE_SMALL, true, [&] {
                    LOG_MESSAGE("Reset Rotation");
                    o->setRotation(M3::getMatrixIdentity());
                    editBuf[0] = editBuf[1] = editBuf[2] = 0.0f;
                    editedObj = o;
                });
                ImGui::PopID();
                ImGui::SameLine();

                // Resync from the matrix only when NOT actively dragging (or switched object)
                if (!wasActive || editedObj != o) {
                    editBuf[0] = o->getRotation().getPitchDegree();
                    editBuf[1] = o->getRotation().getYawDegree();
                    editBuf[2] = o->getRotation().getRollDegree();
                }

                bool changed = ImGui::DragFloat3("Rotation", editBuf, 0.5f, -360.0f, 360.0f, "%.2f");
                bool nowActive = ImGui::IsItemActive();

                if (changed) {
                    // Rebuild the full rotation from the buffer's Euler angles — no drift
                    o->setRotation(M3::getMatrixRotationForEulerAngles(
                        ToolsMaths::DegreesToRadians(editBuf[0]),
                        ToolsMaths::DegreesToRadians(editBuf[1]),
                        ToolsMaths::DegreesToRadians(editBuf[2])
                    ));
                }

                if (nowActive) editedObj = o;
                wasActive = nowActive;

                ImGui::Separator();

                float vec3f[3];

                ImGui::PushID("reset_drawoffset");
                GUI::DrawButton("Reset rotation", IconGUI::RESET, GUIType::Sizes::ICON_SIZE_SMALL, true, [&] {
                    LOG_MESSAGE("Reset DrawOffset");
                    o->setDrawOffset(Vertex3D(0, 0, 0));
                });
                ImGui::PopID();
                ImGui::SameLine();

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
                ImGui::PushID("reset_scale");
                GUI::DrawButton("Reset Scale", IconGUI::RESET, GUIType::Sizes::ICON_SIZE_SMALL, true, [&] {
                    LOG_MESSAGE("Reset Scale");
                    o->setScale(1.f);
                });
                ImGui::PopID();
                ImGui::SameLine();

                float sv[3] = {o->scale3D.x, o->scale3D.y, o->scale3D.z};
                if (ImGui::DragFloat3("Scale", sv, 0.01f, -360.f, 360.f, "%.3f")) {
                    o->scale3D = Vertex3D(sv[0], sv[1], sv[2]);
                }
            }
        }
    }
    // alpha
    if (o->featuresGUI.alpha) {
        if (ImGui::CollapsingHeader("Render settings")) {
            const float range_alpha_min = 0;
            const float range_alpha_max = 1;

            ImGui::DragScalar("Alpha##001", ImGuiDataType_Float, &o->getAlpha(), 0.01, &range_alpha_min, &range_alpha_max, "%f", 1.0f);
            ImGui::Spacing();
            ImGui::Checkbox("Selectable", &o->selectable);
            ImGui::SameLine();
            ImGui::Checkbox("Highlighted", &o->highlighted);
            if (o->highlighted) {
                ImGui::SameLine();
                float col[4] = {o->highlightColor.r, o->highlightColor.g, o->highlightColor.b, o->highlightColor.a};
                if (ImGui::ColorEdit4("Highlight Color", col, ImGuiColorEditFlags_NoInputs)) {
                    o->highlightColor = Color(col[0], col[1], col[2], col[3]);
                }
            }
            ImGui::SameLine();
            ImGui::Checkbox(std::string("Lit/Unlit").c_str(), &o->enableLights);
            if (!o->isTransparent() && o->isEnableLights()) {
                ImGui::Separator();
                ImGui::Checkbox(std::string("Shadow Mapping").c_str(), &o->getRenderSettings().shadowMap);
            }
            ImGui::Separator();
            ImGui::Checkbox(std::string("Back Face Culling").c_str(), &o->getRenderSettings().culling);
            ImGui::SameLine();
            ImGui::Checkbox(std::string("Frustum Culling").c_str(), &o->getRenderSettings().frustumCulling);
            ImGui::Separator();
            ImGui::Checkbox(std::string("Depth Test").c_str(), &o->getRenderSettings().depthTest);
            ImGui::SameLine();
            ImGui::Checkbox(std::string("Write Depth").c_str(), &o->getRenderSettings().writeDepth);
            ImGui::Separator();
            ImGui::Checkbox(std::string("Blend").c_str(), &o->getRenderSettings().blend);
            if (o->getRenderSettings().blend) {
                SelectorOGLBlendMode(o->getRenderSettings().mode_src, o->getRenderSettings().mode_dst);
            }
            ImGui::Spacing();

            bool avatarGlobal = Config::get()->SHOW_AVATARS;
            ImGui::BeginDisabled(!avatarGlobal);
            ImGui::Checkbox("Show Avatar", &o->showAvatar);
            ImGui::EndDisabled();
        }
    }

    if (o->featuresGUI.attached) {
        if (ImGui::CollapsingHeader("Attached Objects")) {
            if (o->attachedObjects.empty() <= 0) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Not objects found");
            }

            for (auto a: o->attachedObjects) {
                if (ImGui::TreeNodeEx(a->getName().c_str())) {
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

                o->drawImGuiCollisionModeSelector();
                if (o->getCollisionMode() != KINEMATIC) {
                    o->DrawImGuiCollisionShapeSelector();
                }
                ImGui::Separator();
                GUI::ImageButtonNormal(IconGUI::SCRIPT_RELOAD, "Update Collider", [&] {
                    o->UpdateShapeCollider();
                });
                ImGui::SameLine();

                static bool showWorldStatus = false;
                static bool showWorldProperties = false;

                GUI::ImageButtonNormal(IconGUI::COLLIDER_INFO, "Debug", [&] {
                    showWorldStatus = true;
                });
                ImGui::SameLine();
                GUI::ImageButtonNormal(IconGUI::COLLIDER_EDIT, "Properties ", [&] {
                    showWorldProperties = true;
                });

                if (showWorldStatus) {
                    ImGui::SetNextWindowSize(ImVec2(450, 500), ImGuiCond_FirstUseEver);
                    if (ImGui::Begin("Collider Debug Info", &showWorldStatus, ImGuiWindowFlags_NoDocking)) {
                        ColliderGUI::DrawColliderWorldVariables(o);
                    }
                    ImGui::End();
                }

                if (showWorldProperties) {
                    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
                    if (ImGui::Begin("Collider Properties", &showWorldProperties, ImGuiWindowFlags_NoDocking)) {
                        ColliderGUI::DrawPopUpColliderSetting(o);
                    }
                    ImGui::End();
                }
            }
        }
        // alpha
        if (o->featuresGUI.misc) {
            if (ImGui::CollapsingHeader("Misc")) {
                ImGui::Text( " %s: (%f, %f, %f)", "Picking Color", o->pickingColor.r, o->pickingColor.g, o->pickingColor.b);
                ImGui::Text( " %s: %d", "ID", o->id);
                ImGui::Text( " %s: %s", "BelongToScene", o->belongToScene ? "Yes" : "No");
                ImGui::Text( " %s: %f", "Distance to camera", o->distanceToCamera);
            }
        }
    }
}


void Object3DGUI::DrawSelectedObjectGuizmo()
{
    if (Components::get()->Scripting()->isExecuting()) return;

    auto render = Components::get()->Render();

    if (render->getSelectedObject() != nullptr) {
        auto o = render->getSelectedObject();

        auto camera = Components::get()->Camera();
        Drawable::drawObject3DGizmo(
            o,
            o->getModelMatrix(),
            camera->getGLMMat4ViewMatrix(),
            camera->getGLMMat4ProjectionMatrix()
        );
    }
}

void Object3DGUI::SelectorOGLBlendMode(GLenum &mode_src, GLenum &mode_dst)
{
    // Selector para Source
    int current_src = Config::FindBlendModeIndex(mode_src);
    if (ImGui::Combo("Source Blend Mode##", &current_src, Config::blend_modes, IM_ARRAYSIZE(Config::blend_modes))) {
        mode_src = Config::blend_values[current_src];
    }

    // Selector para Destination
    int current_dst = Config::FindBlendModeIndex(mode_dst);
    if (ImGui::Combo("Destination Blend Mode##", &current_dst, Config::blend_modes, IM_ARRAYSIZE(Config::blend_modes))) {
        mode_dst = Config::blend_values[current_dst];
    }
}