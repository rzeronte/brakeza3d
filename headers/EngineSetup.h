#ifndef SDL2_3D_ENGINE_ENGINESETUP_H
#define SDL2_3D_ENGINE_ENGINESETUP_H

#include "Color.h"
#include "Vertex.h"

class EngineSetup {

public:
    EngineSetup();
    ~EngineSetup();

    static EngineSetup* getInstance();

    std::string ENGINE_TITLE = "brakeZa v0.1";

    // Screen dimension constants
    int SCREEN_WIDTH = 640;
    int SCREEN_HEIGHT = 480;

    float MOUSE_SENSIBILITY = 0.15f;
    bool CAMERA_MOUSE_ROTATION = true;

    static EngineSetup* instance;
    const int ENGINE_MAX_GAMEOBJECTS = 100;

    // Draw axis
    bool RENDER_AXIS = true;
    bool RENDER_OBJECTS_AXIS = true;

    bool MESH_DEBUG_INFO = false;

    // Triangle render options
    // FaceCulling
    bool TRIANGLE_FACECULLING = true;
    bool TRIANGLE_FRUSTUM_CULLING = true;

    // Fill Triangle modes
    bool TRIANGLE_MODE_PIXELS = false;
    bool TRIANGLE_MODE_WIREFRAME = true;
    bool TRIANGLE_MODE_COLOR_SOLID = true;
    bool TRIANGLE_MODE_TEXTURIZED = true;

    // Normal in triangle
    bool TRIANGLE_RENDER_NORMAL = false;

    bool DRAW_OBJECT3D_BILLBOARD = true;
    bool DRAW_LIGHTPOINTS_BILLBOARD = true;
    bool DRAW_LIGHTPOINTS_AXIS = true;


    // Z BUFFER
    bool TRIANGLE_RENDER_DEPTH_BUFFER = true;

    bool SHOW_SHADOWMAPPING_BUFFER = false;
    bool ENABLE_SHADOW_CASTING = false;
    bool ENABLE_LIGHTS = true;

    bool TRIANGLE_RENDER_CLIPPING = true;

    bool DRAW_FRUSTUM = false;

    // FRUSTUM PLANES
    int NEAR_PLANE   = 0;

    // VIEW FRUSTUM
    int FAR_PLANE    = 1;
    int LEFT_PLANE   = 2;
    int RIGHT_PLANE  = 3;
    int TOP_PLANE    = 4;
    int BOTTOM_PLANE = 5;

    // CONFIG VALUES
    float FRUSTUM_CLIPPING_DISTANCE = 0.0001f;

    Vertex CameraPosition = Vertex(1, 1, -5);
    float CONTROLLER_SPEED_MOVEMENT = 0.5f;
    float CONTROLLER_SPEED_ROTATION = 5.f;

    float CONTROLLER_SPEED_ROTATION_CAMERA = 0.5f;
    float CONTROLLER_SPEED_MOVEMENT_CAMERA = 0.5f;

    std::string ICON_LIGHTPOINTS_DEFAULT = "../icons/lightpoint.tga";
    std::string ICON_OBJECT3D_DEFAULT    = "../icons/object3d.tga";

    std::string FOLDER_SPRITES_DEFAULT    = "../sprites/";

    // Triangle color for TRIANGLE_MODE_COLOR_SOLID
    Uint32 TRIANGLE_SOLID_COLOR = 0xFFFFFF;    // green

    // Show Extra Line Demo when triangle stripped in two when rasterizing
    bool TRIANGLE_DEMO_EXTRALINE_ENABLED = false;
    Uint32 TRIANGLE_DEMO_EXTRALINE = 0x00FF00; // blue

    // GUI Setup values
    float GUI_MIN_SPACE_COORDINATES_VALUE = -100.f;
    float GUI_MAX_SPACE_COORDINATES_VALUE = -100.f;
    float GUI_FLOAT_SPACE_COORDINATES_SENS = -0.1f;

};


#endif //SDL2_3D_ENGINE_ENGINESETUP_H
