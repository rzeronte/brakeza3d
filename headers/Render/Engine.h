
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
#include "BSPMap.h"
#include "../Objects/BSPEntity3D.h"

class Engine {
public:

    // Window y Renderer principal
    SDL_Window *window;
    SDL_Renderer *renderer;

    // Eventos SDL
    SDL_Event e;

    // Camera y Controlador (Input)
    Camera3D *camera;
    Controller *controller;

    // Objetos 3D
    Object3D **gameObjects;

    int numberGameObjects;

    BSPMap *bsp_map;

    // Luces
    LightPoint3D **lightPoints;
    int numberLightPoints;

    // Exit
    bool finish;

    TTF_Font *font = NULL;

    // Timer
    Timer engineTimer;
    float deltaTime;
    float last_ticks;
    float current_ticks;
    float timerCurrent;

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
    void preUpdate();
    void postUpdate();
    void onEnd();

    void cameraUpdate();

    void drawMeshes();
    void drawBSP();
    void drawLightPoints();
    void drawSprites();
    void drawObjectsBillboard();

    void checkCollisionsBSP();
    void checkCollisionsMesh();

    void objects3DShadowMapping();
    void clearLightPointsShadowMappings();

    void addObject3D(Object3D *obj, std::string label);
    void addLightPoint(LightPoint3D *lightPoint, std::string label);

    void windowUpdate();

    void drawGUI();

    void loadBSP(const char *bspFilename, const char *paletteFilename);

    void processFPS();

    Timer* getTimer();

    Object3D* getObjectByLabel(std::string label);

    Vertex3D collideWithWorld( Vertex3D eSpacePosition, Vertex3D eSpaceVelocity, int &collisionRecursionDepth, Collider *collider);
    void collideAndSlide(Vertex3D vel);

    Vertex3D updatePhysics();

    void updateTimer();
    float getDeltaTime();

};

#endif //SDL2_3D_ENGINE_ENGINE_H
