//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_PARTICLEEMITTERGUI_H
#define BRAKEZA3D_PARTICLEEMITTERGUI_H

#include "../../3D/ParticleEmitter.h"
#include "../../Misc/Logging.h"

class ParticleEmitterGUI
{
    // ── Helpers de maquetación ────────────────────────────────────────────────────────────────
    static constexpr float kLabelWidth = 130.0f;
    static constexpr int   kAngleMin = 0, kAngleMax = 360;

    // Título de sección + tabla de 2 columnas (etiqueta fija | control que llena el resto)
    static bool BeginSection(const char* title, const char* tableId)
    {
        ImGui::SeparatorText(title);
        if (!ImGui::BeginTable(tableId, 2, ImGuiTableFlags_SizingFixedFit)) return false;
        ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, kLabelWidth);
        ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);
        return true;
    }
    static void EndSection() { ImGui::EndTable(); }

    // Nueva fila: etiqueta (con ayuda al pasar el ratón) y deja el cursor en la columna del control
    static void Row(const char* label, const char* help)
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        if (help != nullptr && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", help);
        ImGui::TableSetColumnIndex(1);
    }

    static bool DragF(const char* id, float* v, float speed, float vmin, float vmax, const char* fmt)
    {
        ImGui::SetNextItemWidth(-FLT_MIN);
        return ImGui::DragFloat(id, v, speed, vmin, vmax, fmt);
    }

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
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM)) {
                        auto selection = (char*) payload->Data;
                        std::string fullPath = selection;
                        if (o->texture == nullptr) {
                            o->texture = new Image(fullPath);
                        } else {
                            delete o->texture;
                            o->texture = new Image(fullPath);
                        }
                        LOG_MESSAGE("File %s", selection);
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::TreePop();
            }
            ImGui::Separator();

            // Propiedades en tablas de 2 columnas (etiqueta de ancho fijo + control que llena el resto),
            // agrupadas por secciones; pares mín/máx en una sola fila; ayuda al pasar sobre la etiqueta.
            auto& c = o->context;

            if (BeginSection("EMISSION", "##pe_emission")) {
                bool emitting = !o->stopAdd;
                Row("Emitting", "Unchecked = stops spawning new particles (the alive ones finish their life).");
                if (ImGui::Checkbox("##pe_emitting", &emitting)) o->stopAdd = !emitting;
                Row("Rate", "Particles spawned per second (decimals allowed: 0.5 = one every 2 s).");
                DragF("##pe_rate", &c.PARTICLES_BY_SECOND, 1.0f, 0.0f, 5000.0f, "%.1f /s");
                Row("Lifespan", "Seconds each particle lives. Color, opacity and size interpolate over this time.");
                DragF("##pe_life", &c.PARTICLE_LIFESPAN, 0.01f, 0.0f, 100.0f, "%.2f s");
                EndSection();
            }

            if (BeginSection("MOVEMENT", "##pe_move")) {
                Row("Spread angle", "Random deviation from the emitter's UP direction (0 = straight line, 360 = all directions).");
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::DragScalar("##pe_angle", ImGuiDataType_S32, &c.SMOKE_ANGLE_RANGE, 1.0f, &kAngleMin, &kAngleMax, "%d deg");
                Row("Speed", "Launch speed, units/second. Each particle picks a value between min and max.");
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::DragFloatRange2("##pe_speed", &c.MIN_VELOCITY, &c.MAX_VELOCITY, 0.05f, 0.0f, 200.0f, "min %.2f u/s", "max %.2f u/s");
                Row("Gravity", "Vertical acceleration, u/s2. Positive = rises (smoke, fire), negative = falls (sparks, debris).");
                DragF("##pe_gravity", &c.GRAVITY, 0.01f, -100.0f, 100.0f, "%.2f u/s2");
                Row("Deceleration", "Speed kept per frame at 60 fps (1 = no braking, 0.9 = brakes fast). Frame-rate independent.");
                DragF("##pe_decel", &c.DECELERATION_FACTOR, 0.001f, 0.0f, 1.0f, "%.3f");
                Row("Direction noise", "Random turn of the direction each frame (degrees at 60 fps). Makes paths wobble; keeps the speed.");
                DragF("##pe_vnoise", &c.VELOCITY_NOISE, 0.02f, 0.0f, 90.0f, "%.2f deg");
                Row("Spawn area", "Particles are born scattered in a cube of this size (units) around the emitter.");
                DragF("##pe_pnoise", &c.POSITION_NOISE, 0.02f, 0.0f, 100.0f, "%.2f u");
                EndSection();
            }

            if (BeginSection("LOOK", "##pe_look")) {
                Row("Opacity at birth", "Opacity when born (0-255), random between min and max. Fades to 0 at the end of its life.");
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::DragIntRange2("##pe_alpha", &c.MIN_ALPHA, &c.MAX_ALPHA, 1.0f, 0, 255, "min %d", "max %d");
                Row("Size", "Base size multiplier of every particle (base ~0.1-0.6 u).");
                DragF("##pe_size", &c.PARTICLE_SIZE_SCALE, 0.05f, 0.01f, 100.0f, "x %.2f");
                Row("Size over life", "Size multiplier at birth -> at death (1 / 1 = constant). Start < end = grows (smoke), start > end = shrinks (sparks).");
                float sz[2] = { c.SIZE_START, c.SIZE_END };
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::DragFloat2("##pe_sizelife", sz, 0.01f, 0.0f, 20.0f, "%.2f")) { c.SIZE_START = sz[0]; c.SIZE_END = sz[1]; }
                EndSection();
            }

            if (BeginSection("ROTATION", "##pe_rot")) {
                Row("Random start", "Each particle is born rotated at a random angle (breaks the 'stamp' look of smoke).");
                ImGui::Checkbox("##pe_rotstart", &c.ROTATION_RANDOM_START);
                Row("Spin speed", "Rotation speed in degrees/second, random between min and max (0 / 0 = no spin).");
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::DragFloatRange2("##pe_spin", &c.ROTATION_SPEED_MIN, &c.ROTATION_SPEED_MAX, 0.25f, -1440.0f, 1440.0f, "min %.0f deg/s", "max %.0f deg/s");
                Row("Random direction", "Each particle spins clockwise or counter-clockwise at random.");
                ImGui::Checkbox("##pe_rotdir", &c.ROTATION_RANDOM_DIR);
                EndSection();
            }

            if (BeginSection("COLORS", "##pe_colors")) {
                Row("At birth", "Color when the particle is born.");
                ImVec4 color = {o->colorFrom.r, o->colorFrom.g, o->colorFrom.b, 1};
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::ColorEdit4("##pe_colfrom", (float *) &color, ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoAlpha)) {
                    o->setColorFrom(Color(color.x, color.y, color.z));
                }
                Row("At death", "Color it fades to at the end of its life.");
                color = {o->colorTo.r, o->colorTo.g, o->colorTo.b, 1};
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::ColorEdit4("##pe_colto", (float *) &color, ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoAlpha)) {
                    o->setColorTo(Color(color.x, color.y, color.z));
                }
                EndSection();
            }
            ImGui::Separator();
            if (ImGui::TreeNode("Attached Light")) {
                if (o->attachedLight != nullptr) {
                    ImGui::Text("Light: %s", o->attachedLight->getName().c_str());

                    ImVec4 color = {o->attachedLight->diffuse.r, o->attachedLight->diffuse.g, o->attachedLight->diffuse.b, 1};
                    bool changed = ImGui::ColorEdit4("Diffuse##", (float*)&color, ImGuiColorEditFlags_NoOptions);
                    if (changed) {
                        o->attachedLight->diffuse = glm::vec4(color.x, color.y, color.z, 0);
                    }

                    color = {o->attachedLight->ambient.r, o->attachedLight->ambient.g, o->attachedLight->ambient.b, 1};
                    changed = ImGui::ColorEdit4("Ambient##", (float*)&color, ImGuiColorEditFlags_NoOptions);
                    if (changed) {
                        o->attachedLight->ambient = glm::vec4(color.x, color.y, color.z, 0);
                    }

                    color = {o->attachedLight->specular.r, o->attachedLight->specular.g, o->attachedLight->specular.b, 1};
                    changed = ImGui::ColorEdit4("Specular##", (float*)&color, ImGuiColorEditFlags_NoOptions);
                    if (changed) {
                        o->attachedLight->specular = glm::vec4(color.x, color.y, color.z, 0);
                    }

                    ImGui::DragFloat("Constant", &o->attachedLight->constant, 0.01f, 0.0f, 10.0f, "%.4f");
                    ImGui::DragFloat("Linear", &o->attachedLight->linear, 0.001f, 0.0f, 10.0f, "%.4f");
                    ImGui::DragFloat("Quadratic", &o->attachedLight->quadratic, 0.001f, 0.0f, 10.0f, "%.4f");

                    if (ImGui::Button("Remove Light")) {
                        o->removeAttachedLight();
                    }
                } else {
                    ImGui::Text("No attached light.");

                    static float range = 15.0f;
                    static ImVec4 color = {1, 0, 0, 1};
                    ImGui::ColorEdit4("Color##new", (float*)&color, ImGuiColorEditFlags_NoOptions);
                    ImGui::DragFloat("Range", &range, 0.5f, 1.0f, 100.0f, "%.1f");

                    if (ImGui::Button("Create Light")) {
                        o->createAttachedLight(
                            Color(color.x, color.y, color.z),
                            Color(color.x * 0.05f, color.y * 0.05f, color.z * 0.05f),
                            Color(color.x, color.y, color.z),
                            range
                        );
                    }
                }
                ImGui::TreePop();
            }
        }
    }
};

#endif //BRAKEZA3D_PARTICLEEMITTERGUI_H