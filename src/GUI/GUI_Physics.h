
#ifndef BRAKEDA3D_GUI_PHYSICS_H
#define BRAKEDA3D_GUI_PHYSICS_H


#include "GUI.h"
#include "../../headers/Objects/Camera3D.h"
#include "../../imgui/imgui.h"

class GUI_Physics : public GUI  {
public:
    bool show = true;

    virtual ~GUI_Physics() {}

    virtual void draw(Camera3D *camera)
    {
        ImGuiWindowFlags window_flags = 0;

        if (show) {

            ImGui::SetNextWindowPos(ImVec2(2, 608), ImGuiSetCond_Once);
            ImGui::SetNextWindowSize(ImVec2(250, 240), ImGuiSetCond_Once);
            window_flags |= ImGuiWindowFlags_NoMove;

            std::string title = "Physics";
            ImGui::Begin(title.c_str(), &show, window_flags);


            ImGui::TextColored(ImVec4(255.0f, 255.0f, 255.0f,1.0f), "Velocity");
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f,1.0f), std::to_string( camera->velocity.vertex2.x).c_str() );
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f,1.0f), std::to_string( camera->velocity.vertex2.y).c_str() );
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f,1.0f), std::to_string( camera->velocity.vertex2.z).c_str() );

            ImGui::Separator();
            ImGui::End();
        }
    }
};


#endif
