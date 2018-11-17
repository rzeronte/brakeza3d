//
// Created by darkhead on 29/4/18.
//

#ifndef SDL2_3D_ENGINE_ENGINE_H
#define SDL2_3D_ENGINE_ENGINE_H

#include <SDL.h>
#include "Controller.h"
#include "EngineSetup.h"
#include "Timer.h"
#include "../Objects/LightPoint3D.h"
#include <SDL_ttf.h>

#include "../../imgui/imgui.h"
#include "../../imgui/examples/imgui_impl_sdl.h"
#include "../../imgui/examples/imgui_impl_opengl2.h"

#include "../../src/GUI/GUI.h"
#include "../../src/GUI/GUI_Menu.h"
#include "../../src/GUI/GUI_ObjectsInspector.h"
#include "../../src/GUI/GUI_Engine.h"

class Engine {
public:

    // Window y Renderer principal
    SDL_Window *window;
    SDL_Renderer *renderer;

    // Eventos SDL
    SDL_Event e;

    // Camera y Controlador (Input)
    Camera3D *cam;
    Controller *cont;

    // Objetos 3D
    Object3D **gameObjects;
    int numberGameObjects;

    // Luces
    LightPoint3D **lightPoints;
    int numberLightPoints;

    // Exit
    bool finish;

    TTF_Font *font = NULL;

    // Timer
    Timer fpsTimer;

    // Fps counter
    float fps;
    int countedFrames = 0;

    // GUI
    GUI_Engine *gui_engine;

    // SDL GL_Context for GUI
    SDL_GLContext gl_context;

    // Dear ImGUI
    ImGuiContext* imgui_context;

    Engine();

    void Close();

    bool initWindow();
    void initFontsTTF();

    void onStart();
    void onUpdate();
    void onEnd();

    void drawMeshes();
    void drawLightPoints();
    void drawSprites();

    void objects3DShadowMapping();
    void clearLightPointsShadowMappings();

    void addObject3D(Object3D *obj, std::string label);
    void addLightPoint(LightPoint3D *lightPoint, std::string label);

    Controller *getController() const;

    void cameraUpdate();
    void windowUpdate();

    void drawGUI();

    void processFPS();
    Timer* getTimer();

    Object3D* getObjectByLabel(std::string label);

};


#endif //SDL2_3D_ENGINE_ENGINE_H
