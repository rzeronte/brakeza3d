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
    bool show = false;
    int miscFlags = ImGuiColorEditFlags_NoOptions;
    ImGuiWindowFlags window_flags = 0;

    std::string directory_path;
    std::string selected_file;

    explicit GUI_Files(std::string directoryPath) : directory_path(std::move(directoryPath)) {}

    virtual void draw() {
        ImGuiWindowFlags window_flags = 0;

        if (show) {
            ImGui::Begin("File Browser", &show, window_flags);

            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir (directory_path.c_str())) != NULL) {
                int index = 1;
                while ((ent = readdir (dir)) != NULL) {
                    if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                        if(ImGui::Selectable(ent->d_name, selected_file == ent->d_name)){
                            selected_file = ent->d_name;
                        }
                    }
                }
                closedir (dir);
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
