
#ifndef BRAKEDA3D_GUI_TILES_H
#define BRAKEDA3D_GUI_TILES_H


#include "GUI.h"
#include "../Components/Camera3D.h"
#include "../../imgui/imgui.h"
#include "GUIManager.h"
#include "../Render/Tile.h"

class GUI_Tiles : public GUI {
public:
    bool show = false;

    virtual ~GUI_Tiles() {}

    virtual void draw(Camera3D *camera, std::vector<Tile> &tiles, int numColumns) {
        ImGuiWindowFlags window_flags = 0;

        if (show) {

            ImGui::SetNextWindowPos(ImVec2(2, 608), ImGuiSetCond_Once);
            ImGui::SetNextWindowSize(ImVec2(250, 240), ImGuiSetCond_Once);

            std::string title = "Tiles";
            ImGui::Begin(title.c_str(), &show, window_flags);

            ImGui::Columns(numColumns, "mycolumns3", false);

            if (EngineSetup::getInstance()->BASED_TILE_RENDER) {
                for (int i = 0; i < tiles.size(); i++) {
                    ImGui::Checkbox(std::string(
                            std::to_string(i) + " (" + std::to_string(tiles[i].triangleIds.size()) + ")").c_str(),
                                    &tiles[i].draw);
                    ImGui::NextColumn();
                }
            }

            ImGui::Columns(1);
            ImGui::Separator();
            ImGui::End();
        }
    }
};


#endif
