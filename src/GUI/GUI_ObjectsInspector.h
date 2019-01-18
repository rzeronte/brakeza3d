
#ifndef SDL2_3D_ENGINE_GUI_INSPECTOR_H
#define SDL2_3D_ENGINE_GUI_INSPECTOR_H


#include "GUI.h"
#include "../../imgui/imgui.h"
#include "../../headers/Objects/Object3D.h"
#include "../../headers/Objects/Mesh3D.h"
#include "../../headers/Objects/SpriteDirectional3D.h"
#include "../../headers/Objects/Sprite3D.h"

class GUI_ObjectsInspector : public GUI  {
public:
    bool show = true;

    virtual ~GUI_ObjectsInspector() {}

    virtual void draw(Object3D **objects, int number)
    {
        ImGuiWindowFlags window_flags = 0;

        if (show) {

            ImGui::SetNextWindowPos(ImVec2(2, 22), ImGuiSetCond_Once);
            ImGui::SetNextWindowSize(ImVec2(250, 420), ImGuiSetCond_Once);
            window_flags |= ImGuiWindowFlags_NoMove;

            std::string title = "Object Inspector (" + std::to_string(number) + " objects)";
            ImGui::Begin(title.c_str(), &show, window_flags);

            const float range_min = EngineSetup::getInstance()->GUI_BAR_DEFAULT_MIN_VALUE;
            const float range_max = EngineSetup::getInstance()->GUI_BAR_DEFAULT_MAX_VALUE;
            const float range_sensibility = EngineSetup::getInstance()->GUI_BAR_SENSITIVITY;

            const int range_framerate_min = EngineSetup::getInstance()->GUI_MIN_SPRITE3D_FRAMERATE;
            const int range_framerate_max = EngineSetup::getInstance()->GUI_MAX_SPRITE3D_FRAMERATE;

            for (int i = 0; i < number; i++) {
                std::string header_text = objects[i]->label + "##" + std::to_string(i);
                std::string enabled_text = "Enabled##" + std::to_string(i);
                std::string position_text = "Position##" + std::to_string(i);
                std::string rotation_text = "Rotation##" + std::to_string(i);
                std::string shadow_text = "Shadow##" + std::to_string(i);

                if (ImGui::CollapsingHeader(header_text.c_str(), i)) {
                    // position
                    if (ImGui::TreeNode( position_text.c_str() )) {
                        ImGui::DragScalar("X",     ImGuiDataType_Float,  &objects[i]->getPosition()->x, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Y",     ImGuiDataType_Float,  &objects[i]->getPosition()->y, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Z",     ImGuiDataType_Float,  &objects[i]->getPosition()->z, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::TreePop();
                    }

                    // rotation
                    if (ImGui::TreeNode( rotation_text.c_str() )) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f,1.0f), std::to_string( objects[i]->getRotation().getPitchDegree()).c_str() );
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f,1.0f), std::to_string( objects[i]->getRotation().getYawDegree()).c_str() );
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f,1.0f), std::to_string( objects[i]->getRotation().getRollDegree()).c_str() );
                        ImGui::TreePop();
                    }

                    // Only for meshes
                    Mesh3D *pMesh = dynamic_cast<Mesh3D *>(objects[i]);
                    if (pMesh != NULL) {
                        ImGui::Checkbox(shadow_text.c_str(), &dynamic_cast<Mesh3D *>(objects[i])->shadowCaster);
                    }

                    // Only for SPRITES
                    SpriteDirectional3D *pSprite3D = dynamic_cast<SpriteDirectional3D *>(objects[i]);
                    if (pSprite3D != NULL) {
                        ImGui::DragScalar("Framerate", ImGuiDataType_S32,  &pSprite3D->fps, 1.f,  &range_framerate_min, &range_framerate_max, "%d fps", 1);
                    }
                    // Only for SPRITES
                    Sprite3D *pSprite = dynamic_cast<Sprite3D *>(objects[i]);
                    if (pSprite != NULL) {
                        ImGui::DragScalar("Framerate", ImGuiDataType_S32,  &pSprite->fps, 1.f,  &range_framerate_min, &range_framerate_max, "%d fps", 1);
                    }

                    // All Objects setup
                    ImGui::Checkbox(enabled_text.c_str(), &objects[i]->enabled);
                }
            }

            ImGui::End();
        }
    }
};


#endif //SDL2_3D_ENGINE_GUI_INSPECTOR_H
