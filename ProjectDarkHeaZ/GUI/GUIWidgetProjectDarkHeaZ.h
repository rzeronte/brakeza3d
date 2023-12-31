//
// Created by edu on 31/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETPROJECTDARKHEAZ_H
#define BRAKEZA3D_GUIWIDGETPROJECTDARKHEAZ_H

#include "imgui.h"
#include "../../include/ComponentsManager.h"

struct GUIWidgetProjectDarkHeaZ
{
    static void draw() {
        if (ImGui::Begin("Project DarkHeaZ")) {

            auto levels = ComponentsManager::get()->getComponentGame()->getLevelLoader()->getLevels();
            for (int n = 0; n < levels.size(); n++) {
                auto levelName = std::to_string(n + 1) + ") " + levels[n];
                ImGui::Button(levelName.c_str());
                if (ImGui::IsItemClicked()) {
                    ComponentsManager::get()->getComponentGame()->reloadLevel(n);
                }
            }

            ImGui::EndMenu();
        }
    }
};
#endif //BRAKEZA3D_GUIWIDGETPROJECTDARKHEAZ_H
