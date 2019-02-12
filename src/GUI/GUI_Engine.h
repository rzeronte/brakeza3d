
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include "GUI.h"
#include "GUI_Menu.h"
#include "GUI_ObjectsInspector.h"
#include "GUI_LightsInspector.h"
#include "GUI_Log.h"
#include "GUI_Camera.h"
#include "GUI_Physics.h"

class GUI_Engine : GUI {
public:
    GUI_Menu *gui_menu;
    GUI_ObjectsInspector *gui_inspector;
    GUI_LightsInspector *gui_lightpoints;
    GUI_Log *gui_log;
    GUI_Camera *gui_camera;
    GUI_Physics *gui_physics;

    float fps;

    GUI_Engine()
    {
        gui_menu = new GUI_Menu();
        gui_inspector = new GUI_ObjectsInspector();
        gui_lightpoints = new GUI_LightsInspector();
        gui_log = new GUI_Log();
        gui_camera = new GUI_Camera();
        gui_physics = new GUI_Physics();
    }

    virtual void draw(bool &finish, Object3D **gameObjects, int numberGameObjects, LightPoint3D **lightPoints, int numberLightPoints, Camera3D *cam, float timeDelta)
    {
        bool show_demo_window = true;
        //ImGui::ShowDemoWindow(&show_demo_window);

        gui_menu->draw(finish, gui_inspector->show, gui_lightpoints->show, gui_log->show, gui_camera->show, gui_physics->show);
        gui_inspector->draw(gameObjects, numberGameObjects);
        gui_lightpoints->draw(lightPoints, numberLightPoints);
        gui_log->draw(getFps(), timeDelta, cam);
        gui_camera->draw(cam);
        gui_physics->draw(cam);
    }

    float getFps() const {
        return fps;
    }

    void setFps(float fps) {
        GUI_Engine::fps = fps;
    }

};


#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
