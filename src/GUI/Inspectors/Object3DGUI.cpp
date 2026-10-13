//
// Created by Eduardo on 26/11/2025.
//

#include "../../../include/GUI/Inspectors/Object3DGUI.h"
#include "../../../include/Components/Components.h"
#include "../../../include/GUI/Windows/FileSystemGUI.h"
#include "../../../include/GUI/Widgets/GUIFields.h"
#include <functional>
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
            using namespace GUIFields;

            // Botón de reset (icono) a la izquierda del control; el control llena el resto de la columna
            auto resetButton = [](const char* id, const char* tooltip, const std::function<void()>& action) {
                ImGui::PushID(id);
                GUI::DrawButton(tooltip, IconGUI::RESET, GUIType::Sizes::ICON_SIZE_SMALL, true, action);
                ImGui::PopID();
                ImGui::SameLine();
                ImGui::SetNextItemWidth(-FLT_MIN);
            };

            if (BeginSection("TRANSFORM")) {
                // position
                if (o->featuresGUI.position) {
                    Row("Position");
                    resetButton("reset_position", "Reset position", [&] {
                        LOG_MESSAGE("Reset Position");
                        o->setPosition(Vertex3D(0, 0, 0));
                    });
                    float vec3f[3];
                    o->getPosition().toFloat(vec3f);
                    if (ImGui::DragFloat3("##v", vec3f, 0.01f, -999999.0f, 999999.0f)) {
                        o->position.x = vec3f[0];
                        o->position.y = vec3f[1];
                        o->position.z = vec3f[2];
                    }
                }

                // rotation
                if (o->featuresGUI.rotation) {
                    // Edit-buffer pattern: avoid re-decomposing the matrix to Euler on every
                    // frame while the user is dragging — that roundtrip is unstable near
                    // gimbal-lock (pitch ~ ±90°) and causes the sliders to jump around.
                    static const Object3D* editedObj = nullptr;
                    static float           editBuf[3] = {0, 0, 0};
                    static bool            wasActive  = false;

                    Row("Rotation");
                    resetButton("reset_rotation", "Reset rotation", [&] {
                        LOG_MESSAGE("Reset Rotation");
                        o->setRotation(M3::getMatrixIdentity());
                        editBuf[0] = editBuf[1] = editBuf[2] = 0.0f;
                        editedObj = o;
                    });

                    // Resync from the matrix only when NOT actively dragging (or switched object)
                    if (!wasActive || editedObj != o) {
                        editBuf[0] = o->getRotation().getPitchDegree();
                        editBuf[1] = o->getRotation().getYawDegree();
                        editBuf[2] = o->getRotation().getRollDegree();
                    }

                    bool changed = ImGui::DragFloat3("##v", editBuf, 0.5f, -360.0f, 360.0f, "%.2f");
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

                    Row("Draw offset", "Visual offset of the model without moving the object (and its collider).");
                    resetButton("reset_drawoffset", "Reset draw offset", [&] {
                        LOG_MESSAGE("Reset DrawOffset");
                        o->setDrawOffset(Vertex3D(0, 0, 0));
                    });
                    float vec3f[3];
                    o->drawOffset.toFloat(vec3f);
                    if (ImGui::DragFloat3("##v", vec3f, 0.01f, -999999.0f, 999999.0f)) {
                        o->drawOffset.x = vec3f[0];
                        o->drawOffset.y = vec3f[1];
                        o->drawOffset.z = vec3f[2];
                    }
                }

                // scale
                if (o->featuresGUI.scale) {
                    Row("Scale");
                    resetButton("reset_scale", "Reset scale", [&] {
                        LOG_MESSAGE("Reset Scale");
                        o->setScale(1.f);
                    });
                    float sv[3] = {o->scale3D.x, o->scale3D.y, o->scale3D.z};
                    if (ImGui::DragFloat3("##v", sv, 0.01f, -360.f, 360.f, "%.3f")) {
                        o->scale3D = Vertex3D(sv[0], sv[1], sv[2]);
                    }

                    // Escala general: el valor es la media de los tres ejes y al cambiarlo se multiplican los
                    // tres por el mismo factor, así se conservan las proporciones de un objeto escalado por ejes
                    float uniform = o->getScale();
                    if (Float("Scale (all)", &uniform, 0.01f, 0.001f, 1000.f, "%.3f", "Scales the three axes together, keeping their proportions")) {
                        const float current = o->getScale();
                        if (std::fabs(current) > 1e-6f) {
                            const float k = uniform / current;
                            o->scale3D = Vertex3D(o->scale3D.x * k, o->scale3D.y * k, o->scale3D.z * k);
                        } else {
                            o->scale3D = Vertex3D(uniform, uniform, uniform);
                        }
                    }
                }
                EndSection();
            }
        }
    }
    // alpha
    if (o->featuresGUI.alpha) {
        if (ImGui::CollapsingHeader("Render settings")) {
            using namespace GUIFields;
            auto& rs = o->getRenderSettings();

            if (BeginSection("APPEARANCE")) {
                Float("Alpha", &o->getAlpha(), 0.01f, 0.0f, 1.0f, "%.2f", "Opacity of the whole object (0 = invisible, 1 = opaque).");
                Check("Selectable", &o->selectable, "Can be picked with the mouse in the viewport.");
                Check("Highlighted", &o->highlighted, "Draw the object with a highlight color.");
                if (o->highlighted) {
                    float col[4] = {o->highlightColor.r, o->highlightColor.g, o->highlightColor.b, o->highlightColor.a};
                    Row("Highlight color");
                    if (ImGui::ColorEdit4("##v", col, ImGuiColorEditFlags_NoInputs)) {
                        o->highlightColor = Color(col[0], col[1], col[2], col[3]);
                    }
                    EndRow();
                }
                bool avatarGlobal = Config::get()->SHOW_AVATARS;
                ImGui::BeginDisabled(!avatarGlobal);
                Check("Show avatar", &o->showAvatar, "Draw the object's icon in the viewport (needs avatars enabled globally).");
                ImGui::EndDisabled();
                EndSection();
            }

            if (BeginSection("LIGHTING")) {
                Check("Lit", &o->enableLights, "Off = ignore scene lights (drawn at full albedo).");
                if (!o->isTransparent() && o->isEnableLights()) {
                    Check("Shadow mapping", &rs.shadowMap, "Receive and cast shadows.");
                }
                EndSection();
            }

            if (BeginSection("RASTER STATE")) {
                Check("Back face culling", &rs.culling);
                Check("Frustum culling", &rs.frustumCulling, "Skip drawing when outside the camera view.");
                Check("Depth test", &rs.depthTest);
                Check("Write depth", &rs.writeDepth);
                Check("Blend", &rs.blend);
                EndSection();
            }
            if (rs.blend) {
                SelectorOGLBlendMode(rs.mode_src, rs.mode_dst);
            }
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
                if (GUIFields::BeginSection("OBJECT")) {
                    GUIFields::Text("Picking color", "(%f, %f, %f)", o->pickingColor.r, o->pickingColor.g, o->pickingColor.b);
                    GUIFields::Text("ID", "%d", o->id);
                    GUIFields::Text("Belongs to scene", "%s", o->belongToScene ? "Yes" : "No");
                    GUIFields::Text("Distance to camera", "%f", o->distanceToCamera);
                    GUIFields::EndSection();
                }
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