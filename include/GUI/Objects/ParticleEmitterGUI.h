//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_PARTICLEEMITTERGUI_H
#define BRAKEZA3D_PARTICLEEMITTERGUI_H

#include "../../3D/ParticleEmitter.h"
#include "../../Misc/Logging.h"

class ParticleEmitterGUI
{
public:
    static void DrawPropertiesGUI(ParticleEmitter *o)
    {
        if (ImGui::CollapsingHeader("ParticleEmitter")) {

            if (ImGui::TreeNode("Image")) {
                if (o->texture != nullptr) {
                    ImGui::Image(o->texture->getOGLImTexture(),ImVec2(32, 32));
                } else {
                    ImGui::Text("Empty texture. Drag one here!");
                }

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMAGE_ITEM")) {
                        Logging::Message("Dropping image (%s) in emitter %s", payload->Data, o->getName().c_str());
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        auto selection = (char*) payload->Data;
                        auto fullPath = Config::get()->IMAGES_FOLDER + selection;
                        if (o->texture == nullptr) {
                            o->texture = new Image(fullPath);
                        } else {
                            delete o->texture;
                            o->texture = new Image(fullPath);
                        }
                        Logging::Message("File %s", selection);
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::TreePop();
            }
            ImGui::Separator();

            if (ImGui::TreeNode("Colors")) {
                ImVec4 color = {o->colorFrom.r, o->colorFrom.g, o->colorFrom.b, 1};
                bool changed_color = ImGui::ColorEdit4("Color From##", (float *) &color, ImGuiColorEditFlags_NoOptions);
                if (changed_color) {
                    o->setColorFrom(Color(color.x,color.y,color.z));
                }
                color = {o->colorTo.r, o->colorTo.g, o->colorTo.b, 1};
                changed_color = ImGui::ColorEdit4("Color To##", (float *) &color, ImGuiColorEditFlags_NoOptions);
                if (changed_color) {
                    o->setColorTo(Color(color.x,color.y,color.z));
                }
                ImGui::TreePop();
            }
            ImGui::Separator();
            if (ImGui::TreeNode("ParticlesContext")) {

                ImGui::Checkbox(std::string("stopAdd").c_str(), &o->stopAdd);

                const float range_sensibility = 0.01f;
                const float range_min = -100;
                const float range_max = 100;

                const int range_min_int = 5;
                const int range_max_int = 255;

                ImGui::DragScalar("Gravity", ImGuiDataType_Float, &o->context.GRAVITY, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
                ImGui::DragScalar("Particle lifespan", ImGuiDataType_Float, &o->context.PARTICLE_LIFESPAN, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
                ImGui::DragScalar("Particles/sec", ImGuiDataType_S32, &o->context.PARTICLES_BY_SECOND, 1.f , &range_min_int, &range_max_int, "%d", 1.0f);
                ImGui::DragScalar("Angle range", ImGuiDataType_S32, &o->context.SMOKE_ANGLE_RANGE, 1.f ,&range_min_int, &range_max_int, "%d", 1.0f);
                ImGui::DragScalar("Velocity min", ImGuiDataType_S32, &o->context.MIN_VELOCITY,1.f, &range_min_int, &range_max_int, "%d", 1.0f);
                ImGui::DragScalar("Velocity max", ImGuiDataType_S32, &o->context.MAX_VELOCITY, 1.f ,&range_min_int, &range_max_int, "%d", 1.0f);
                ImGui::DragScalar("Alpha min", ImGuiDataType_S32, &o->context.MIN_ALPHA,1.f, &range_min_int, &range_max_int, "%d", 1.0f);
                ImGui::DragScalar("Alpha max", ImGuiDataType_S32, &o->context.MAX_ALPHA, 1.f ,&range_min_int, &range_max_int, "%d", 1.0f);
                ImGui::DragScalar("Position noise", ImGuiDataType_S32, &o->context.POSITION_NOISE, 1.f ,&range_min_int, &range_max_int, "%d", 1.0f);
                ImGui::DragScalar("Velocity noise", ImGuiDataType_S32, &o->context.VELOCITY_NOISE, 1.f ,&range_min_int, &range_max_int, "%d", 1.0f);
                ImGui::DragScalar("Deceleration", ImGuiDataType_Float, &o->context.DECELERATION_FACTOR, range_sensibility ,&range_min, &range_max, "%f", 1.0f);

                ImGui::TreePop();
            }
        }
    }
};

#endif //BRAKEZA3D_PARTICLEEMITTERGUI_H