//
// Created by eduardo on 3/3/22.
//

#ifndef BRAKEDA3D_GUI_SHADERS_H
#define BRAKEDA3D_GUI_SHADERS_H

#include "GUI.h"
#include "../Components/Camera3D.h"
#include "../../imgui/imgui.h"
#include "GUIManager.h"
#include "../Render/Tile.h"
#include "../Render/Shader.h"

class GUI_Shaders : public GUI {
public:
    bool show = false;

    virtual ~GUI_Shaders() {}

    virtual void draw(std::map<int, Shader*> &shaders) {
        ImGuiWindowFlags window_flags = 0;

        if (show) {

            ImGui::SetNextWindowPos(ImVec2(2, 608), ImGuiSetCond_Once);
            ImGui::SetNextWindowSize(ImVec2(250, 240), ImGuiSetCond_Once);

            std::string title = "Shaders";
            ImGui::Begin(title.c_str(), &show, window_flags);

            int i = 1;
            for (auto shaderMap : shaders) {
                std::string shader_text = "Shader " + shaderMap.second->getLabel() + "##" + std::to_string(i);
                std::string enabled_text = "Enabled##" + std::to_string(i);

                if (ImGui::CollapsingHeader(shader_text.c_str(), false)) {
                    ImGui::Checkbox(enabled_text.c_str(), &shaderMap.second->enabled);
                }
                i++;
            }
            ImGui::Columns(1);
            ImGui::Separator();
            ImGui::End();
        }
    }
};


#endif //BRAKEDA3D_GUI_SHADERS_H
