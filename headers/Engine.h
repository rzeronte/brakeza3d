//
// Created by darkhead on 29/4/18.
//

#ifndef SDL2_3D_ENGINE_ENGINE_H
#define SDL2_3D_ENGINE_ENGINE_H

#include <SDL.h>
#include "Controller.h"
#include "EngineSetup.h"
#include "LTimer.h"
#include "LightPoint.h"
#include <SDL_ttf.h>

#include "../imgui/imgui.h"
#include "../imgui/examples/imgui_impl_sdl.h"
#include "../imgui/examples/imgui_impl_opengl2.h"

#include "GUI/GUI.h"
#include "GUI/GUI_Menu.h"
#include "GUI/GUI_ObjectsInspector.h"
#include "GUI/GUI_Engine.h"

class Engine {
public:

    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Event e;

    Camera *cam;
    Controller *cont;

    Object3D **gameObjects;
    int numberGameObjects;

    LightPoint **lightPoints;
    int numberLightPoints;

    bool finish;

    TTF_Font *font = NULL;

    LTimer fpsTimer;
    float fps;

    GUI_Engine *gui_engine;

    SDL_GLContext gl_context;

    ImGuiContext* imgui_context;

    int countedFrames = 0;

    Engine();

    void Close();

    bool initWindow();

    void initFontsTTF();

    void onStart();

    void onUpdate();
    void onUpdateEvent();
    void onEnd();

    void drawMeshes();
    void drawLightPoints();
    void drawSprites();

    void objects3DShadowMapping();
    void clearLightPointsShadowMappings();

    void addObject3D(Object3D *obj, std::string label);
    void addLightPoint(LightPoint *lightPoint, std::string label);

    Object3D* getObjectByLabel(std::string label);

    Controller *getController() const;

    void cameraUpdate();
    void windowUpdate();

    void drawGUI();

    void processFPS();
};


#endif //SDL2_3D_ENGINE_ENGINE_H
