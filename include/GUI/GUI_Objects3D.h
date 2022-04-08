
#ifndef SDL2_3D_ENGINE_GUI_INSPECTOR_H
#define SDL2_3D_ENGINE_GUI_INSPECTOR_H


#include "GUI.h"
#include "../../imgui/imgui.h"
#include "../Objects/Object3D.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/SpriteDirectional3D.h"
#include "../Objects/Sprite3D.h"
#include "../Physics/Mesh3DBody.h"
#include "../Physics/Mesh3DGhost.h"
#include "../Render/Logging.h"
#include "../Objects/Decal.h"
#include "../Objects/Mesh3DAnimatedCollection.h"
#include "../Particles/ParticleEmissor.h"
#include "../Physics/SimplePendulum.h"
#include "../../src/Game/Player.h"

class GUI_Objects3D : public GUI {
private:
    ImVec4 imguiColor;
    ImVec4 imguiColorSpecularity;

public:
    bool show = false;
    int miscFlags = ImGuiColorEditFlags_NoOptions;

    virtual ~GUI_Objects3D() {}

    virtual void draw(std::vector<Object3D *> &gameObjects) {
        ImGuiWindowFlags window_flags = 0;

        if (show) {
            ImGui::SetNextWindowPos(ImVec2(2, 22), ImGuiSetCond_Once);
            ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiSetCond_Once);
            //window_flags |= ImGuiWindowFlags_NoMove;

            std::string title = "Object Inspector (" + std::to_string(gameObjects.size()) + " objects)";
            ImGui::Begin(title.c_str(), &show, window_flags);


            for(int i = 0; i < gameObjects.size(); i++) {
                auto object = gameObjects[i];

                if (object->isRemoved()) {
                    continue;
                }

                std::string headerText = object->getLabel() + "##" + std::to_string(i);

                if (ImGui::CollapsingHeader(headerText.c_str(), false)) {
                    GuiObjects3D(object, i);
                    GuiMesh3D(object, i);
                    GuiDecal(object, i);
                    GuiSprite3D(object, i);
                    GuiMesh3DAnimatedCollection(object, i);
                    GuiLightPoints3D(object, i);
                    GuiParticleEmissor(object, i);
                    GuiSimplePendulum(object, i);
                    GuiPlayer(object, i);
                }
            }
            ImGui::End();
        }
    }

    void GuiObjects3D(Object3D *object, int i) {
        std::string enabled_text = "Enabled##" + std::to_string(i);
        std::string stencilBuffer_text = "StencilBuffer##" + std::to_string(i);
        std::string position_text = "Position##" + std::to_string(i);
        std::string rotation_text = "Rotation##" + std::to_string(i);
        std::string follow_camera_text = "FollowCamera##" + std::to_string(i);
        std::string draw_offset_text = "DrawOffset##" + std::to_string(i);
        std::string alpha_text = "Alpha Channel##" + std::to_string(i);
        std::string alpha_value_text = "Alpha Value##" + std::to_string(i);
        std::string alpha_value = "Alpha##" + std::to_string(i);

        const float range_min = -10000;
        const float range_max = 10000;
        const float range_sensibility = 0.5;

        const float range_angle_min = -360;
        const float range_angle_max = 360;
        const float range_angle_sensibility = 0.1;

        const float range_alpha_min = 0;
        const float range_alpha_max = 255;
        const float range_alpha_sensibility = 1.0;

        // enabled
        ImGui::Checkbox(enabled_text.c_str(), &object->enabled);
        ImGui::Checkbox(stencilBuffer_text.c_str(), &object->stencilBufferEnabled);
        ImGui::Checkbox(follow_camera_text.c_str(), &object->followCamera);
        ImGui::Checkbox(alpha_text.c_str(), &object->alphaEnabled);

        if (ImGui::IsItemEdited()) {
            object->setStencilBufferEnabled(object->stencilBufferEnabled);
        }

        // position
        if (ImGui::TreeNode(position_text.c_str())) {
            ImGui::DragScalar("X", ImGuiDataType_Float, &object->getPosition().x, range_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Y", ImGuiDataType_Float, &object->getPosition().y, range_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Z", ImGuiDataType_Float, &object->getPosition().z, range_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::TreePop();
        }

        // rotation
        if (ImGui::TreeNode(rotation_text.c_str())) {
            bool needUpdateRotation = false;
            ImGui::DragScalar("X", ImGuiDataType_Float, &object->rotX, range_angle_sensibility, &range_angle_min,&range_angle_max, "%f", 1.0f);
            if (ImGui::IsItemEdited()) {
                needUpdateRotation = true;
            }
            ImGui::DragScalar("Y", ImGuiDataType_Float, &object->rotY, range_angle_sensibility, &range_angle_min,&range_angle_max, "%f", 1.0f);
            if (ImGui::IsItemEdited()) {
                needUpdateRotation = true;
            }
            ImGui::DragScalar("Z", ImGuiDataType_Float, &object->rotZ, range_angle_sensibility, &range_angle_min,&range_angle_max, "%f", 1.0f);
            if (ImGui::IsItemEdited()) {
                needUpdateRotation = true;
            }
            if (needUpdateRotation) {
                object->setRotation(M3::getMatrixRotationForEulerAngles(object->rotX, object->rotY, object->rotZ));
            }
            ImGui::TreePop();
        }

        if (object->alphaEnabled) {
            if (ImGui::TreeNode(alpha_value_text.c_str())) {
                ImGui::DragScalar("Alpha", ImGuiDataType_Float, &object->alpha, range_alpha_sensibility,&range_alpha_min, &range_alpha_max, "%f", 1.0f);
                ImGui::TreePop();
            }
        }

        // drawOffset
        if (ImGui::TreeNode(draw_offset_text.c_str())) {
            ImGui::DragScalar("X", ImGuiDataType_Float, &object->drawOffset.x, range_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Y", ImGuiDataType_Float, &object->drawOffset.y, range_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Z", ImGuiDataType_Float, &object->drawOffset.z, range_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::TreePop();
        }

        ImGui::Button("Remove");
        if (ImGui::IsItemClicked()) {
            object->removed = true;
        }
    }

    void GuiMesh3D(Object3D *object, int i) {
        auto *mesh = dynamic_cast<Mesh3D *>(object);
        if (mesh != nullptr) {
            std::string mesh3d_text = "Mesh3D##" + std::to_string(i);
            std::string numTriangles = "Triangles: " + std::to_string(mesh->modelTriangles.size());

            if (ImGui::TreeNode(mesh3d_text.c_str())) {
                ImGui::Checkbox(std::string("Enable light").c_str(), &mesh->enableLights);
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), numTriangles.c_str());
                ImGui::TreePop();
            }
        }
    }

    void GuiDecal(Object3D *object, int i) {
        auto *decal = dynamic_cast<Decal *>(object);
        if (decal != nullptr) {
            ImGui::Checkbox(std::string("Draw Decal Planes").c_str(), &decal->drawWireframe);
            std::string decalNumTriangles = "Decal Triangles: " + std::to_string(decal->modelTriangles.size());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), decalNumTriangles.c_str());
        }
    }

    void GuiSprite3D(Object3D *object, int i) {
        auto *sprite = dynamic_cast<Sprite3D *>(object);
        if (sprite != nullptr) {
            const int range_framerate_min = 0;
            const int range_framerate_max = 100;

            ImGui::DragScalar("Framerate", ImGuiDataType_S32, &sprite->getCurrentTextureAnimation()->fps,1.f, &range_framerate_min, &range_framerate_max, "%d fps", 1);
            if (ImGui::IsItemEdited()) {
                sprite->updateStep();
            }
        }
    }

    void GuiMesh3DAnimatedCollection(Object3D *&object, int i) {
        auto *meshAnimatedCollection = dynamic_cast<Mesh3DAnimatedCollection *>(object);
        if (meshAnimatedCollection != nullptr) {
            const float range_speed_min = 0;
            const float range_speed_max = 1;
            const float range_speed_sensibility = 0.1;

            std::string animation_text = "Animation##" + std::to_string(i);

            ImGui::DragScalar("Speed", ImGuiDataType_Float,&meshAnimatedCollection->getCurrentMesh3DAnimated()->animation_speed,range_speed_sensibility, &range_speed_min, &range_speed_max, "%f", 1.0f);

            std::string animationEndsText = "AnimationEnds: " + std::to_string(meshAnimatedCollection->getCurrentMesh3DAnimated()->isAnimationEnds());
            ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), animationEndsText.c_str());

            static const char *item_current; // Here our selection is a single pointer stored outside the object.
            static ImGuiComboFlags flags = 0;
            if (meshAnimatedCollection->currentAnimation >= 0) {
                item_current = meshAnimatedCollection->getCurrentMesh3DAnimated()->getLabel().c_str();
            }

            // The second parameter is the label previewed before opening the combo.
            if (ImGui::BeginCombo(animation_text.c_str(), item_current,flags)) {
                for (int n = 0; n < meshAnimatedCollection->mesh3Danimated.size(); n++) {
                    bool is_selected = (item_current == meshAnimatedCollection->mesh3Danimated[n]->getLabel());
                    if (ImGui::Selectable(meshAnimatedCollection->mesh3Danimated[n]->getLabel().c_str(),is_selected)) {
                        item_current = meshAnimatedCollection->mesh3Danimated[n]->getLabel().c_str();
                    }

                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
                    }

                    if (ImGui::IsItemDeactivatedAfterEdit()) {
                        meshAnimatedCollection->currentAnimation = n;
                    }
                }
                ImGui::EndCombo();
            }
        }
    }

    void GuiLightPoints3D(Object3D *&object, int i) {
        auto *light = dynamic_cast<LightPoint3D *>(object);
        if (light != nullptr) {

            bool changed_color_specular = false;
            std::string attenuation_text = "Attenuation##" + std::to_string(i);
            std::string colorpicker_text = "RGB##" + std::to_string(i);
            std::string colorpicker_text_specularity = "Specularity##" + std::to_string(i);
            std::string show_deep_map_text = "Show ZBuffer##" + std::to_string(i);
            std::string show_frustum_map_text = "Show Frustum##" + std::to_string(i);
            std::string specularity_RGB_text = "RGB##" + std::to_string(i);
            std::string color_text = "Color##" + std::to_string(i);

            const float range_potence_min = 0;
            const float range_potence_max = 1000;
            const float range_potence_sensibility = 0.1f;

            const float range_min = -90000;
            const float range_max = 90000;

            if (EngineSetup::get()->CREATE_LIGHT_ZBUFFER) {
                ImGui::Checkbox(show_deep_map_text.c_str(), &light->showDeepMapping);
                ImGui::Checkbox(show_frustum_map_text.c_str(), &light->showFrustum);
            }

            // Color
            if (ImGui::TreeNode(color_text.c_str())) {
                bool changed_color = ImGui::ColorEdit4(colorpicker_text.c_str(), (float *) &imguiColor, miscFlags);
                if (changed_color) {
                    light->setColor(
                            imguiColor.x * 256,
                            imguiColor.y * 256,
                            imguiColor.z * 256
                    );
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode(attenuation_text.c_str())) {
                ImGui::DragScalar("P", ImGuiDataType_Float, &light->p, range_potence_sensibility,&range_potence_min, &range_potence_max, "%f", 1.0f);
                ImGui::DragScalar("Constant", ImGuiDataType_Float, &light->kc, range_potence_sensibility,&range_min, &range_max, "%f", 1.0f);
                ImGui::DragScalar("Linear", ImGuiDataType_Float, &light->kl, range_potence_sensibility,&range_min, &range_max, "%f", 1.0f);
                ImGui::DragScalar("Quadratic", ImGuiDataType_Float, &light->kq, range_potence_sensibility,&range_min, &range_max, "%f", 1.0f);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode(colorpicker_text_specularity.c_str())) {
                ImGui::DragScalar("m", ImGuiDataType_Float, &light->specularComponent, range_potence_sensibility,&range_potence_min, &range_potence_max, "%f", 1.0f);
                changed_color_specular = ImGui::ColorEdit4(specularity_RGB_text.c_str(), (float *) &imguiColorSpecularity, miscFlags);
                if (changed_color_specular) {
                    light->setColorSpecularity(
                        imguiColorSpecularity.x * 256,
                        imguiColorSpecularity.y * 256,
                        imguiColorSpecularity.z * 256
                    );
                }
                ImGui::TreePop();
            }
        }
    }

    void GuiParticleEmissor(Object3D *object, int i) {
        auto *emissor = dynamic_cast<ParticleEmissor *>(object);
        if (emissor != nullptr) {
            const float step_range_min = 0;
            const float step_range_max = 500;
            const float  step_range_sensibility = 0.001f;

            const float range_angle_min = -360;
            const float range_angle_max = 360;
            const float range_angle_sensibility = 0.1;

            std::string rotation_particle_text = "Rotation Frame##" + std::to_string(i);

            // rotation frame
            if (ImGui::TreeNode(rotation_particle_text.c_str())) {
                ImGui::DragScalar("X", ImGuiDataType_Float, &emissor->rotFrameX, range_angle_sensibility, &range_angle_min,&range_angle_max, "%f", 1.0f);
                ImGui::DragScalar("Y", ImGuiDataType_Float, &emissor->rotFrameY, range_angle_sensibility, &range_angle_min,&range_angle_max, "%f", 1.0f);
                ImGui::DragScalar("Z", ImGuiDataType_Float, &emissor->rotFrameZ, range_angle_sensibility, &range_angle_min,&range_angle_max, "%f", 1.0f);
                ImGui::TreePop();
            }

            ImGui::DragScalar("Force", ImGuiDataType_Float, &emissor->force, step_range_sensibility,&step_range_min, &step_range_max, "%f", 1.0f);
            ImGui::DragScalar("TTL", ImGuiDataType_Float, &emissor->ttl, step_range_sensibility,&step_range_min, &step_range_max, "%f", 1.0f);
            ImGui::DragScalar("Step", ImGuiDataType_Float, &emissor->step, step_range_sensibility,&step_range_min, &step_range_max, "%f", 1.0f);
            if (ImGui::IsItemEdited()) {
                emissor->counter.setStep(emissor->step);
            }
        }
    }

    void GuiSimplePendulum(Object3D *&object, int i) {
        auto *pendulum = dynamic_cast<SimplePendulum *>(object);
        if (pendulum != nullptr) {
            std::string activeFriction = "Active friction##" + std::to_string(i);
            const float step_range_min = 0;
            const float step_range_max = 500;
            ImGui::DragScalar("AngleX", ImGuiDataType_Float, &pendulum->angleX, 1,&step_range_min, &step_range_max, "%f", 1.0f);
            ImGui::DragScalar("AngleZ", ImGuiDataType_Float, &pendulum->angleZ, 1,&step_range_min, &step_range_max, "%f", 1.0f);
            ImGui::DragScalar("Length", ImGuiDataType_Float, &pendulum->length, 1,&step_range_min, &step_range_max, "%f", 1.0f);

            ImGui::Checkbox(activeFriction.c_str(), &pendulum->activeFriction);
            if (pendulum->activeFriction) {
                ImGui::DragScalar("Friction", ImGuiDataType_Float, &pendulum->friction, 0.001f,&step_range_min, &step_range_max, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    pendulum->t = 0;
                }
            }
        }
    }

    void GuiPlayer(Object3D *object, int i) {
        auto *player = dynamic_cast<Player *>(object);
        if (player != nullptr) {
            std::string player_movement_text = "Movement##" + std::to_string(i);

            if (ImGui::TreeNode(player_movement_text.c_str())) {
                std::string activeFriction = "Active friction##" + std::to_string(i);
                const float step_range_min = 0;
                const float step_range_max = 50000;
                ImGui::DragScalar("Power", ImGuiDataType_Float, &player->power, 1,&step_range_min, &step_range_max, "%f", 1.0f);
                ImGui::DragScalar("Friction", ImGuiDataType_Float, &player->friction, 1,&step_range_min, &step_range_max, "%f", 1.0f);
                ImGui::DragScalar("Max Vel.", ImGuiDataType_Float, &player->maxVelocity, 1,&step_range_min, &step_range_max, "%f", 1.0f);
                ImGui::DragScalar("Auto Rotation Speed", ImGuiDataType_Float, &player->autoRotationSelectedObjectSpeed, 1,&step_range_min, &step_range_max, "%f", 1.0f);

                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),std::to_string(player->getVelocity().x).c_str());
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),std::to_string(player->getVelocity().y).c_str());
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f),std::to_string(player->getVelocity().z).c_str());

                ImGui::TreePop();
            }
        }
    }

};


#endif //SDL2_3D_ENGINE_GUI_INSPECTOR_H
