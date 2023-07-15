
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include <dirent.h>
#include "GUI.h"
#include "GUI_Menu.h"
#include "GUI_Objects3D.h"
#include "GUI_Camera.h"
#include "GUI_Tiles.h"
#include "GUI_Weapons.h"
#include "GUI_Files.h"

class GUIManager : public GUI {
public:
    GUI_Menu *guiMenu;
    GUI_Objects3D *guiInspector;
    GUI_Camera *guiCamera;
    GUI_Tiles *guiTiles;
    GUI_Weapons *guiWeapons;
    GUI_Files *guiFiles;

    GUIManager() {
        guiMenu = new GUI_Menu();
        guiInspector = new GUI_Objects3D();
        guiCamera = new GUI_Camera();
        guiTiles = new GUI_Tiles();
        guiWeapons = new GUI_Weapons();
        guiFiles = new GUI_Files(EngineSetup::get()->MODELS_FOLDER);
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
        //ImGui::ShowDemoWindow(&show_demo_window);

        guiMenu->draw(
            finish,
            guiInspector->show,
            guiCamera->show,
            guiTiles->show,
            guiWeapons->show,
            guiFiles->show,
            cam
        );

        guiInspector->draw(gameObjects);
        guiCamera->draw(cam);
        guiTiles->draw(cam, tiles, numTilesColumns);
        guiWeapons->draw();
        guiFiles->draw();
    }
};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
