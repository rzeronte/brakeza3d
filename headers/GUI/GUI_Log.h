
#ifndef SDL2_3D_ENGINE_GUI_LOG_H
#define SDL2_3D_ENGINE_GUI_LOG_H

#include "GUI.h"
#include "../../imgui/imgui.h"
#include "../Misc/Tools.h"

struct ExampleAppLog
{
    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets;        // Index to lines offset
    bool                ScrollToBottom;

    void    Clear()     { Buf.clear(); LineOffsets.clear(); }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size);
        ScrollToBottom = true;
    }

    void Draw(const char* title, bool* p_open = NULL)
    {
        ImGui::SetNextWindowPos(ImVec2(260, 650), ImGuiSetCond_Once);
        ImGui::SetNextWindowSize(ImVec2(1170, 200), ImGuiSetCond_Once);
        if (!ImGui::Begin(title, p_open)) {
            ImGui::End();
            return;
        }
        if (ImGui::Button("Clear")) Clear();
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        Filter.Draw("Filter", -100.0f);
        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (copy) ImGui::LogToClipboard();

        if (Filter.IsActive()) {
            const char* buf_begin = Buf.begin();
            const char* line = buf_begin;
            for (int line_no = 0; line != NULL; line_no++)
            {
                const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
                if (Filter.PassFilter(line, line_end))
                    ImGui::TextUnformatted(line, line_end);
                line = line_end && line_end[1] ? line_end + 1 : NULL;
            }
        } else {
            ImGui::TextUnformatted(Buf.begin());
        }

        if (ScrollToBottom)
            ImGui::SetScrollHere(1.0f);
        ScrollToBottom = false;
        ImGui::EndChild();
        ImGui::End();
    }
};

class GUI_Log : public GUI{
public:

    virtual ~GUI_Log() {
    }

    ExampleAppLog log;
    bool show = false;

    virtual void draw(float deltaTime, Camera3D *cam, int numVisibleTriangles)
    {
        if (show) {
            std::string title = "Log | Camera (x: " +
                    std::to_string(cam->getPosition()->x) + ", y: " +
                    std::to_string(cam->getPosition()->y) + ", z:  " +
                    std::to_string(cam->getPosition()->z) + " | Yaw: " +
                    std::to_string(cam->getRotation().getYawDegree()) + ", Pitch: " +
                    std::to_string(cam->getRotation().getPitchDegree()) + ", deltaTime: " +
                    std::to_string(deltaTime) + ", numTriangles: " +
                    std::to_string(numVisibleTriangles) + "###AnimatedTitle"
            ;
            log.Draw(title.c_str(), &show);
        }
    }
};


#endif //SDL2_3D_ENGINE_GUI_LOG_H
