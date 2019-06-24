
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
#include "../Objects/Mesh3DBody.h"
#include "PhysicsDebugDraw.h"
#include "../Objects/Mesh3DGhost.h"
#include "../Objects/Weapon3D.h"

#include <btBulletDynamicsCommon.h>
#include <OpenCL/opencl.h>

enum collisionGroups
{
    DefaultFilter = 1,
    StaticFilter = 2,
    KinematicFilter = 4,
    DebrisFilter = 8,
    SensorTrigger = 16,
    CharacterFilter = 32,
    CameraTrigger = 64,
    BSPHullTrigger = 128,
    AllFilter = -1  //all bits sets: DefaultFilter | StaticFilter | KinematicFilter | DebrisFilter | SensorTrigger
} ;

class Engine {
public:
    // Window y Renderer principal
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Surface  *screenSurface;
    SDL_Texture *screenTexture;

    // Eventos SDL
    SDL_Event e;

    // Camera y Controlador (Input)
    Camera3D *camera;
    Controller *controller;

    // Objetos 3D
    Object3D **gameObjects;

    std::vector<Mesh3DBody *> meshPhysics;

    int numberGameObjects;

    BSPMap *bsp_map;

    // Luces
    LightPoint3D **lightPoints;
    int numberLightPoints;

    // Exit
    bool finish;

    TTF_Font *font = NULL;

    // Fps counter
    float fps;
    int countedFrames = 0;

    Triangle *frameTriangles;
    int numFrameTriangles = 0;

    Triangle *visibleTriangles;
    int numVisibleTriangles = 0;

    // GUI
    GUI_Engine *gui_engine;

    // SDL GL_Context for GUI
    SDL_GLContext gl_context;

    // Dear ImGUI
    ImGuiContext* imgui_context;

    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    btDefaultCollisionConfiguration* collisionConfiguration;

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    btCollisionDispatcher* dispatcher;

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    btBroadphaseInterface* overlappingPairCache;

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver* solver;

    btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

    PhysicsDebugDraw* debugDraw;

    Weapon3D *weapon;
    Mesh3DGhost *triggerCamera;

    // Timer
    Timer engineTimer;
    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float timerCurrent = 0;

    cl_platform_id platform_id;
    cl_device_id cpu_device_id;
    cl_device_id gpu_device_id;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;
    cl_context context;
    cl_program program;
    cl_kernel kernel;
    cl_kernel makeFragments;

    cl_mem opencl_buffer_triangles;
    cl_mem opencl_buffer_depth;
    cl_mem opencl_buffer_video;

    cl_command_queue command_queue;
    cl_uint* uiInput = NULL; // Mapped Pointer to pinned Host input buffer for host processing

    int sizeTileWidth = 64;
    int sizeTileHeight = 48;
    int tilesWidth;
    int tilesHeight;

    std::vector<Tile> tiles;

    Engine();

    void Close();

    bool initWindow();
    void initFontsTTF();
    void initPhysics();
    void initOpenCL();
    void initTiles();

    void OpenCLInfo();

    void onStart();
    void onUpdate();
    void preUpdate();
    void postUpdate();
    void onEnd();

    void cameraUpdate();

    void getMesh3DTriangles();
    void getQuakeMapTriangles();
    void getLightPointsTriangles();
    void getSpritesTriangles();
    void getObjectsBillboardTriangles();

    void hiddenSurfaceRemoval();
    void handleTrianglesToTiles();

    void drawTilesGrid();

    void drawTilesTriangles();
    void drawFrameTriangles();

    void updatePhysicObjects();

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

    Weapon3D *getWeapon() const;

    void setWeapon(Weapon3D *weapon);

    void processPairsCollisions();
    bool needsCollision(const btCollisionObject* body0, const btCollisionObject* body1);

    void updateTimer();
    float getDeltaTime();

    void testOpenCL();
    void processOpenCL(int numTriangles);


};

#endif //SDL2_3D_ENGINE_ENGINE_H
