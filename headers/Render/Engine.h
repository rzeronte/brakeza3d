
#ifndef SDL2_3D_ENGINE_ENGINE_H
#define SDL2_3D_ENGINE_ENGINE_H

#include <SDL.h>
#include "../EngineSetup.h"
#include "Timer.h"
#include "../Objects/LightPoint3D.h"
#include <SDL_ttf.h>

#include "../../imgui/imgui.h"
#include "../../imgui/examples/imgui_impl_sdl.h"
#include "../../imgui/examples/imgui_impl_opengl2.h"

#include "../../headers/GUI/GUI_Menu.h"
#include "../GUI/GUI_Objects3D.h"
#include "../GUI/GUIManager.h"
#include "../../headers/GUI/GUI_Weapons.h"

#include "BSPMap.h"
#include "../Objects/BSPEntity3D.h"
#include "../Physics/Mesh3DBody.h"
#include "PhysicsDebugDraw.h"
#include "../Physics/Mesh3DGhost.h"
#include "../Physics/SpriteDirectional3DBody.h"

#include <btBulletDynamicsCommon.h>
#include <OpenCL/opencl.h>
#include "../Misc/cJSON.h"
#include "../2D/WeaponsManager.h"
#include "../2D/MenuManager.h"
#include "../Objects/Tile.h"
#include <thread>

class Engine {
public:
    // Events SDL
    SDL_Event e;

    // Scene triangles
    std::vector<Triangle*> frameTriangles;
    std::vector<Triangle*> visibleTriangles;

    // Exit
    bool finish = false;

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
    cl_kernel processAllTriangles;

    cl_mem opencl_buffer_triangles;
    cl_mem opencl_buffer_video;
    cl_mem opencl_buffer_depth;

    cl_command_queue command_queue_rasterizer;

    std::thread preUpdateThread;

    Engine();
    void Close();

    // init systems
    void initOpenCL();
    void initCollisionManager();

    // Cycle of life
    virtual void onStart();
    virtual void preUpdate();
    virtual void onUpdate();
    virtual void onEnd();

    // resolve collision
    virtual void resolveCollisions();

    // Window update
    void updateWindow();

    // Triangle recollector
    void getBSPTriangles();
    void getObjectsTriangles();

    // Object's axis
    void drawSceneObjectsAxis();

    // Triangles filter
    void hiddenSurfaceRemoval();

    // TODO: erase
    //void objects3DShadowMapping();
    //void clearLightPointsShadowMappings();

    // update GUI data
    void updateGUI();

    // OpenCL Rasterization
    void OpenCLInfo();
    void handleOpenCLTriangles();
};

#endif //SDL2_3D_ENGINE_ENGINE_H
