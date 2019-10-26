
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include "GUI.h"
#include "GUI_Menu.h"
#include "GUI_Objects3D.h"
#include "GUI_Lights.h"
#include "GUI_Log.h"
#include "GUI_Camera.h"
#include "GUI_Tiles.h"
#include "GUI_Weapons.h"

class GUIManager : GUI {
public:
    GUI_Menu      *guiMenu;
    GUI_Objects3D *guiInspector;
    GUI_Lights    *guiLightpoints;
    GUI_Log       *guiLog;
    GUI_Camera    *guiCamera;
    GUI_Tiles     *guiTiles;
    GUI_Weapons   *guiWeapons;

    GUIManager() {
        guiMenu        = new GUI_Menu();
        guiInspector   = new GUI_Objects3D();
        guiLightpoints = new GUI_Lights();
        guiLog         = new GUI_Log();
        guiCamera      = new GUI_Camera();
        guiTiles       = new GUI_Tiles();
        guiWeapons     = new GUI_Weapons();
    }

    virtual void draw(float timedelta, bool &finish, std::vector<Object3D*> &gameObjects, std::vector<LightPoint3D*> &lightPoints, Camera3D *cam, std::vector<Tile> &tiles, int numTilesColumns, int numVisibleTriangles, cJSON *maps, WeaponsManager *weapon)
    {
        bool show_demo_window = true;
        //ImGui::ShowDemoWindow(&show_demo_window);

        guiMenu->draw(finish, guiInspector->show, guiLightpoints->show, guiLog->show, guiCamera->show, guiTiles->show, guiWeapons->show, maps);
        guiInspector->draw(gameObjects);
        guiLightpoints->draw(lightPoints);
        guiLog->draw(timedelta, cam, numVisibleTriangles);
        guiCamera->draw(cam);
        guiTiles->draw(cam, tiles, numTilesColumns);
        guiWeapons->draw(weapon);
    }
};


#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
