
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include "GUI.h"
#include "GUI_Menu.h"
#include "GUI_Objects3D.h"
#include "GUI_Lights.h"
#include "GUI_Camera.h"
#include "GUI_Tiles.h"
#include "GUI_Weapons.h"

class GUIManager : public GUI {
public:
    GUI_Menu      *guiMenu;
    GUI_Objects3D *guiInspector;
    GUI_Lights    *guiLightpoints;
    GUI_Camera    *guiCamera;
    GUI_Tiles     *guiTiles;
    GUI_Weapons   *guiWeapons;

    GUIManager() {
        guiMenu        = new GUI_Menu();
        guiInspector   = new GUI_Objects3D();
        guiLightpoints = new GUI_Lights();
        guiCamera      = new GUI_Camera();
        guiTiles       = new GUI_Tiles();
        guiWeapons     = new GUI_Weapons();
    }

    virtual void draw(float timedelta, bool &finish, std::vector<Object3D*> &gameObjects, Camera3D *cam, std::vector<Tile> &tiles, int numTilesColumns)
    {
        bool show_demo_window = true;
        //ImGui::ShowDemoWindow(&show_demo_window);

        guiMenu->draw(finish, guiInspector->show, guiLightpoints->show, guiCamera->show, guiTiles->show, guiWeapons->show, cam);
        guiInspector->draw(gameObjects);
        guiCamera->draw(cam);
        guiTiles->draw(cam, tiles, numTilesColumns);
        guiWeapons->draw();
    }
};


#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
