
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

#include "../../src/GUI/GUI_Menu.h"
#include "../../src/GUI/GUI_ObjectsInspector.h"
#include "../../src/GUI/GUI_Engine.h"
#include "../../src/GUI/GUI_Weapons.h"

#include "BSPMap.h"
#include "../Objects/BSPEntity3D.h"
#include "../Physics/Mesh3DBody.h"
#include "PhysicsDebugDraw.h"
#include "../Physics/Mesh3DGhost.h"
#include "../Physics/SpriteDirectional3DBody.h"

#include <btBulletDynamicsCommon.h>
#include <OpenCL/opencl.h>
#include "../cJSON.h"
#include "../2D/Weapon.h"
#include "../2D/Menu.h"

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
    BillboardsTrigger = 256,
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
    std::vector<Object3D*> gameObjects;
    std::vector<Mesh3DBody *> meshPhysics;
    std::vector<SpriteDirectional3DBody *> projectilePhysics;

    BSPMap *bsp_map;

    // Luces
    std::vector<LightPoint3D *> lightPoints;

    // Exit
    bool finish;

    TTF_Font *font = NULL;

    // Fps counter
    int fps;
    int fpsFrameCounter = 0;

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
    Mesh3DGhost *triggerCamera;

    // Timer
    Timer engineTimer;
    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;
    float frameTime = 0;
    float decimalTime = 0;

    // OpenCL Rasterizer
    cl_platform_id platform_id;
    cl_device_id device_cpu_id;
    cl_device_id device_gpu_id;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;
    cl_context contextCPU;
    cl_context contextGPU;
    cl_program programCPU;
    cl_program programGPU;
    cl_kernel processTileTriangles;
    cl_kernel processAllTriangles;
    cl_kernel processTransformTriangles;

    cl_mem opencl_buffer_triangles;
    cl_mem opencl_buffer_tiles;
    cl_mem opencl_buffer_video;
    cl_mem opencl_buffer_depth;
    cl_mem opencl_buffer_frustum;

    cl_command_queue command_queue_rasterizer;
    cl_command_queue command_queue_transforms;

    // Tiled rasterizer
    std::vector<Tile> tiles;
    OCLTriangle *trianglesTile;

    int sizeTileWidth = 80;
    int sizeTileHeight = 80;
    int tilesWidth;
    int tilesHeight;
    int numTiles;
    int tilePixelsBufferSize;

    // optionsJSON.json
    cJSON *mapsJSONList;
    cJSON *weaponsJSONList;

    Menu *menu;
    Weapon *weapon;

    Engine();

    void Close();

    bool initWindow();
    bool initSound();
    void initFontsTTF();
    void initBulletPhysics();
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
    void removeObject3D(Object3D *obj);
    void addLightPoint(LightPoint3D *lightPoint, std::string label);

    void windowUpdate();

    void drawGUI();

    void loadBSP(const char *bspFilename, const char *paletteFilename);

    void processFPS();

    Timer* getTimer();

    Object3D* getObjectByLabel(std::string label);

    void processPairsCollisions();
    void moveMesh3DBody(Mesh3DBody *oRemoteBody, int targetEntityId);
    bool needsCollision(const btCollisionObject* body0, const btCollisionObject* body1);

    void updateTimer();
    float getDeltaTime();

    void handleOpenCLTriangles();
    void handleOpenCLTrianglesForTiles();
    void dumpTileToFrameBuffer(Tile *t);

    void handleOpenCLTransform();

    void waterShader();

    void getMapsJSON();
    void getWeaponsJSON();

    void drawMenu();

    };

#endif //SDL2_3D_ENGINE_ENGINE_H
