
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include "GUI.h"
#include "GUI_Menu.h"
#include "GUI_ObjectsInspector.h"
#include "GUI_LightsInspector.h"
#include "GUI_Log.h"
#include "GUI_Camera.h"
#include "GUI_Tiles.h"
#include "GUI_Weapons.h"

class GUI_Engine : GUI {
public:
    GUI_Menu *gui_menu;
    GUI_ObjectsInspector *gui_inspector;
    GUI_LightsInspector *gui_lightpoints;
    GUI_Log *gui_log;
    GUI_Camera *gui_camera;
    GUI_Tiles *gui_tiles;
    GUI_Weapons *gui_weapons;

    GUI_Engine() {
        gui_menu = new GUI_Menu();
        gui_inspector = new GUI_ObjectsInspector();
        gui_lightpoints = new GUI_LightsInspector();
        gui_log = new GUI_Log();
        gui_camera = new GUI_Camera();
        gui_tiles = new GUI_Tiles();
        gui_weapons = new GUI_Weapons();
    }

    virtual void draw(float timedelta, bool &finish, std::vector<Object3D*> &gameObjects, std::vector<LightPoint3D*> &lightPoints, Camera3D *cam, std::vector<Tile> &tiles, int numTilesColumns, int numVisibleTriangles, cJSON *maps, Weapon *weapon)
    {
        bool show_demo_window = true;
        //ImGui::ShowDemoWindow(&show_demo_window);

        gui_menu->draw(finish, gui_inspector->show, gui_lightpoints->show, gui_log->show, gui_camera->show, gui_tiles->show, gui_weapons->show, maps);
        gui_inspector->draw(gameObjects);
        gui_lightpoints->draw(lightPoints);
        gui_log->draw(timedelta, cam, numVisibleTriangles);
        gui_camera->draw(cam);
        gui_tiles->draw(cam, tiles, numTilesColumns);
        gui_weapons->draw(weapon);
    }
};


#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
