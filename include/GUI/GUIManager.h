
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include <dirent.h>
#include "GUI.h"
#include "GUI_Menu.h"
#include "GUI_Objects3D.h"
#include "GUI_Camera.h"
#include "GUI_Tiles.h"
#include "GUI_Weapons.h"

class GUIManager : public GUI {
public:
    GUI_Menu *guiMenu;
    GUI_Objects3D *guiInspector;
    GUI_Camera *guiCamera;
    GUI_Tiles *guiTiles;
    GUI_Weapons *guiWeapons;

    GUIManager() {
        guiMenu = new GUI_Menu();
        guiInspector = new GUI_Objects3D();
        guiCamera = new GUI_Camera();
        guiTiles = new GUI_Tiles();
        guiWeapons = new GUI_Weapons();
    }

    virtual void draw(
            float timedelta,
            bool &finish,
            Camera3D *cam,
            const std::vector<Object3D *> &gameObjects,
            std::vector<LightPoint3D *> &lightPoints,
            std::vector<Tile> &tiles,
            int numTilesColumns
    ) {
        bool show_demo_window = true;
        ImGui::ShowDemoWindow(&show_demo_window);

        guiMenu->draw(
            finish,
            guiInspector->show,
            guiCamera->show,
            guiTiles->show,
            guiWeapons->show,
            cam
        );

        ShowFileBrowser(EngineSetup::get()->MODELS_FOLDER);

        guiInspector->draw(gameObjects);
        guiCamera->draw(cam);
        guiTiles->draw(cam, tiles, numTilesColumns);
        guiWeapons->draw();
    }

    std::string selected_file;

    void ShowFileBrowser(const std::string& directory_path)
    {
        ImGui::Begin("File Browser");

        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir (directory_path.c_str())) != NULL) {
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

        if(ImGui::Button("Accept")){
            if(!selected_file.empty()){
                ImGui::Text("You selected: %s", selected_file.c_str());
                Tools::addSceneObject(selected_file, "added_item");
                selected_file = "";
            } else {
                ImGui::Text("No file selected");
            }
        }

        ImGui::End();
    }
};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
