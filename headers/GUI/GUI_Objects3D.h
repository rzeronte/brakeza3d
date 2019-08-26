
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
#include "../../src/Decal.h"

class GUI_Objects3D : public GUI  {
public:
    bool show = false;

    virtual ~GUI_Objects3D() {}

    virtual void draw(std::vector<Object3D*> &gameObjects)
    {
        ImGuiWindowFlags window_flags = 0;

        if (show) {

            ImGui::SetNextWindowPos(ImVec2(2, 22), ImGuiSetCond_Once);
            ImGui::SetNextWindowSize(ImVec2(250, 825), ImGuiSetCond_Once);
            //window_flags |= ImGuiWindowFlags_NoMove;

            std::string title = "Object Inspector (" + std::to_string(gameObjects.size()) + " objects)";
            ImGui::Begin(title.c_str(), &show, window_flags);

            const float range_min = EngineSetup::getInstance()->GUI_BAR_DEFAULT_MIN_VALUE;
            const float range_max = EngineSetup::getInstance()->GUI_BAR_DEFAULT_MAX_VALUE;
            const float range_sensibility = EngineSetup::getInstance()->GUI_BAR_SENSITIVITY;

            const int range_framerate_min = EngineSetup::getInstance()->GUI_MIN_SPRITE3D_FRAMERATE;
            const int range_framerate_max = EngineSetup::getInstance()->GUI_MAX_SPRITE3D_FRAMERATE;

            for (int i = 0; i < gameObjects.size(); i++) {
                std::string header_text;
                Mesh3D *pMesh = dynamic_cast<Mesh3D *>(gameObjects[i]);
                if (pMesh != NULL && !gameObjects[i]->isDecal()) {
                    header_text = gameObjects[i]->getLabel() + " (numTriangles: " + std::to_string(pMesh->numTriangles)+")"+ "##" + std::to_string(i);
                } else {
                    header_text = gameObjects[i]->getLabel() + "##" + std::to_string(i);
                }

                std::string enabled_text = "Enabled##" + std::to_string(i);
                std::string position_text = "Position##" + std::to_string(i);
                std::string rotation_text = "Rotation##" + std::to_string(i);
                std::string shadow_text = "Shadow##" + std::to_string(i);
                std::string animation_text = "Animation##" + std::to_string(i);
                std::string removed_text = "Removed##" + std::to_string(i);

                if (ImGui::CollapsingHeader(header_text.c_str(), false)) {
                    // position
                    if (ImGui::TreeNode(position_text.c_str())) {
                        ImGui::DragScalar("X", ImGuiDataType_Float, &gameObjects[i]->getPosition()->x, range_sensibility,
                                          &range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_Float, &gameObjects[i]->getPosition()->y, range_sensibility,
                                          &range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Z", ImGuiDataType_Float, &gameObjects[i]->getPosition()->z, range_sensibility,
                                          &range_min, &range_max, "%f", 1.0f);
                        ImGui::TreePop();
                    }

                    // rotation
                    if (ImGui::TreeNode(rotation_text.c_str())) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                                           std::to_string(gameObjects[i]->getRotation().getPitchDegree()).c_str());
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
                                           std::to_string(gameObjects[i]->getRotation().getYawDegree()).c_str());
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f),
                                           std::to_string(gameObjects[i]->getRotation().getRollDegree()).c_str());
                        ImGui::TreePop();
                    }

                    ImGui::Checkbox(removed_text.c_str(), &dynamic_cast<Object3D *>(gameObjects[i])->removed);

                    // Only for meshes
                    Mesh3D *pMesh = dynamic_cast<Mesh3D *>(gameObjects[i]);
                    if (pMesh != NULL) {
                        ImGui::Checkbox(shadow_text.c_str(), &dynamic_cast<Mesh3D *>(gameObjects[i])->shadowCaster);
                    }

                    // Only for meshes
                    Decal *pDecal = dynamic_cast<Decal *>(gameObjects[i]);
                    if (pDecal != NULL) {
                        std::string decalNumTriangles = "Decal Triangles: " + std::to_string(pDecal->numTriangles);
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), decalNumTriangles.c_str());
                    }

                    // Only for SPRITES DIRECTIONALS
                    SpriteDirectional3D *pSprite3D = dynamic_cast<SpriteDirectional3D *>(gameObjects[i]);
                    if (pSprite3D != NULL) {
                        static ImGuiComboFlags flags = 0;
                        ImGui::DragScalar("Framerate", ImGuiDataType_S32,  &pSprite3D->fps, 1.f,  &range_framerate_min, &range_framerate_max, "%d fps", 1);
                        const char* items[] = { "walk", "fire", "injuried", "dead", "explosion" };
                        static const char* item_current; // Here our selection is a single pointer stored outside the object.

                        switch (pSprite3D->current_animation) {
                            case 0:
                                item_current = items[0];
                                break;
                            case 1:
                                item_current = items[1];
                                break;
                            case 2:
                                item_current = items[2];
                                break;
                            case 3:
                                item_current = items[3];
                                break;
                            case 4:
                                item_current = items[4];
                                break;
                        }

                        if ( ImGui::BeginCombo(animation_text.c_str(), item_current, flags)) { // The second parameter is the label previewed before opening the combo.
                            for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                                bool is_selected = (item_current == items[n]);
                                if (ImGui::Selectable(items[n], is_selected)) {
                                    item_current = items[n];
                                }

                                if (is_selected) {
                                    ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
                                }

                                if (ImGui::IsItemDeactivatedAfterEdit()) {
                                    pSprite3D->current_animation = n;
                                    Logging::getInstance()->Log(("Changing animation frame to: " + std::to_string(n)));
                                }
                            }
                            ImGui::EndCombo();
                        }
                    }

                    // Only for SPRITES
                    Sprite3D *pSprite = dynamic_cast<Sprite3D *>(gameObjects[i]);
                    if (pSprite != NULL) {
                        ImGui::DragScalar("Framerate", ImGuiDataType_S32,  &pSprite->fps, 1.f,  &range_framerate_min, &range_framerate_max, "%d fps", 1);
                    }

                    // All Objects setup
                    ImGui::Checkbox(enabled_text.c_str(), &gameObjects[i]->enabled);

                    // Only for Mesh3DGhost
                    Mesh3DGhost *pMeshGhost = dynamic_cast<Mesh3DGhost *>(gameObjects[i]);
                    if (pMeshGhost != NULL) {
                        std::string counterText = "currentTriggerCounter: " + std::to_string(pMeshGhost->currentTriggerCounter);
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), counterText.c_str());
                    }

                    // Only for Mesh3DBody
                    Mesh3DBody *pMeshBody = dynamic_cast<Mesh3DBody *>(gameObjects[i]);
                    if (pMeshBody != NULL) {
                        ImGui::Checkbox("Moving", &dynamic_cast<Mesh3DBody *>(gameObjects[i])->moving);
                        ImGui::Checkbox("Reverse moving", &dynamic_cast<Mesh3DBody *>(gameObjects[i])->reverseMoving);
                        ImGui::Checkbox("Waiting", &dynamic_cast<Mesh3DBody *>(gameObjects[i])->waiting);
                        if (pMeshBody->moving || pMeshBody->reverseMoving) {
                            std::string offsetText = "OffsetMoving :" + std::to_string(pMeshBody->offsetMoving);
                            ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), offsetText.c_str());
                        }
                    }

                    // Only for SPRITES
                    Enemy *oEnemy = dynamic_cast<Enemy *>(gameObjects[i]);
                    if (oEnemy != NULL) {
                        std::string staminaText = "Stamina :" + std::to_string(oEnemy->stamina);
                        ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), staminaText.c_str());
                        Logging::getInstance()->Log(staminaText);
                    }
                }
            }

            ImGui::End();
        }
    }
};


#endif //SDL2_3D_ENGINE_GUI_INSPECTOR_H
