//
// Created by eduardo on 3/05/23.
//

#ifndef BRAKEZA3D_GUI_FILES_H
#define BRAKEZA3D_GUI_FILES_H


#include <dirent.h>
#include <string>
#include <utility>
#include "GUI.h"
#include "imgui.h"
#include "../Misc/Tools.h"

class GUI_Files : public GUI {
private:

public:
    bool show = true;
    int miscFlags = ImGuiColorEditFlags_NoOptions;
    ImGuiWindowFlags window_flags = 0;

    std::string directory_path;
    std::string selected_file;

    explicit GUI_Files(std::string directoryPath) : directory_path(std::move(directoryPath)) {}

    virtual void draw()
    {
        ImGuiWindowFlags window_flags = 0;

        if (show) {
            ImGui::Begin("File Browser", &show, window_flags);

            DIR *dir;
            struct dirent *ent;
            std::vector<std::string> result;
            if ((dir = opendir (directory_path.c_str())) != NULL) {
                int index = 1;
                while ((ent = readdir (dir)) != NULL) {
                    if (ent->d_type == DT_DIR) continue;
                    result.push_back(ent->d_name);
                }
                std::sort( result.begin(), result.end() );

                closedir (dir);

                for (const auto& file: result) {
                    if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                        if(ImGui::Selectable(file.c_str(), selected_file == file.c_str())){
                            selected_file = file.c_str();
                        }
                    }
                }
            } else {
                ImGui::Text("Could not open directory");
            }

            if (ImGui::Button("Accept")) {
                if (!selected_file.empty()) {
                    Tools::addSceneObject(selected_file, "added_item");

                    selected_file = "";
                } else {
                    ImGui::Text("No file selected");
                }
            }

            ImGui::End();
        }

    }

};

#endif //BRAKEZA3D_GUI_FILES_H
