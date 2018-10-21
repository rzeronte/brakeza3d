
#ifndef SDL2_3D_ENGINE_GUI_INSPECTOR_H
#define SDL2_3D_ENGINE_GUI_INSPECTOR_H


#include "GUI.h"
#include "../../imgui/imgui.h"
#include "../../headers/Object3D.h"
#include "../../headers/Mesh.h"
#include "../../headers/SpriteDirectionalObject3D.h"
#include "../../headers/SpriteObject3D.h"

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

            const float range_min = EngineSetup::getInstance()->GUI_MIN_SPACE_COORDINATES_VALUE;
            const float range_max = EngineSetup::getInstance()->GUI_MAX_SPACE_COORDINATES_VALUE;
            const float range_sensibility = EngineSetup::getInstance()->GUI_FLOAT_SPACE_COORDINATES_SENS;

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
                        ImGui::DragScalar("X",     ImGuiDataType_Float,  &objects[i]->position.x, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Y",     ImGuiDataType_Float,  &objects[i]->position.y, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Z",     ImGuiDataType_Float,  &objects[i]->position.z, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::TreePop();
                    }

                    // rotation
                    if (ImGui::TreeNode( rotation_text.c_str() )) {
                        ImGui::DragScalar("X",     ImGuiDataType_Float,  &objects[i]->rotation.x, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Y",     ImGuiDataType_Float,  &objects[i]->rotation.y, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Z",     ImGuiDataType_Float,  &objects[i]->rotation.z, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::TreePop();
                    }

                    // rotation limits
                    if (objects[i]->rotation.x > 360) { objects[i]->rotation.x = 0; }
                    if (objects[i]->rotation.y > 360) { objects[i]->rotation.y = 0; }
                    if (objects[i]->rotation.z > 360) { objects[i]->rotation.z = 0; }

                    if (objects[i]->rotation.x < 0) { objects[i]->rotation.x = 360; }
                    if (objects[i]->rotation.y < 0) { objects[i]->rotation.y = 360; }
                    if (objects[i]->rotation.z < 0) { objects[i]->rotation.z = 360; }

                    // Only for meshes
                    Mesh *pMesh = dynamic_cast<Mesh *>(objects[i]);
                    if (pMesh != NULL) {
                        ImGui::Checkbox(shadow_text.c_str(), &dynamic_cast<Mesh *>(objects[i])->shadowCaster);
                    }

                    // Only for SPRITES
                    SpriteDirectionalObject3D *pSprite3D = dynamic_cast<SpriteDirectionalObject3D *>(objects[i]);
                    if (pSprite3D != NULL) {
                        ImGui::DragScalar("Framerate", ImGuiDataType_S32,  &pSprite3D->fps, 1.f,  &range_framerate_min, &range_framerate_max, "%d fps", 1);
                    }
                    // Only for SPRITES
                    SpriteObject3D *pSprite = dynamic_cast<SpriteObject3D *>(objects[i]);
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
