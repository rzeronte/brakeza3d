#ifndef SDL2_3D_ENGINE_ENGINESETUP_H
#define SDL2_3D_ENGINE_ENGINESETUP_H

#include "Color.h"
#include "../Objects/Vertex3D.h"

class EngineSetup {

public:
    EngineSetup();
    ~EngineSetup();

    static EngineSetup* getInstance();

    std::string ENGINE_TITLE = "Brakeza3D v0.1";

    Vertex3D CameraPosition = Vertex3D(1, 1, -5);

    Vertex3D right   = Vertex3D(1, 0, 0);
    Vertex3D up      = Vertex3D(0, 1, 0);
    Vertex3D forward = Vertex3D(0, 0, 1);

    Vertex3D left     = right.getInverse();
    Vertex3D down     = up.getInverse();
    Vertex3D backward = forward.getInverse();

    Vertex3D gravity = Vertex3D(0, 9.8f, 0);

    // Screen dimension constants
    int SCREEN_WIDTH = 320;
    int SCREEN_HEIGHT = 240;

    bool CAMERA_MOUSE_ROTATION = true;

    static EngineSetup* instance;
    const int ENGINE_MAX_GAMEOBJECTS = 1000;

    const float EPSILON = 0.001;

    // Draw axis
    bool RENDER_OBJECTS_AXIS = true;

    bool MESH_DEBUG_INFO = false;

    // FaceCulling
    bool TRIANGLE_BACK_FACECULLING = true;
    bool SHOW_WIREFRAME_FOR_BFC_HIDDEN_TRIANGLES = false;

    bool TRIANGLE_FRUSTUM_CULLING = true;

    // Fill Triangle modes
    bool TRIANGLE_MODE_PIXELS = false;
    bool TRIANGLE_MODE_WIREFRAME = false;
    bool TRIANGLE_MODE_COLOR_SOLID = false;
    bool TRIANGLE_MODE_TEXTURIZED = true;

    bool TEXTURES_BILINEAR_INTERPOLATION = false;

    // Normal in triangle
    bool TRIANGLE_RENDER_NORMAL = false;

    bool DRAW_OBJECT3D_BILLBOARD = true;
    bool DRAW_LIGHTPOINTS_BILLBOARD = true;
    bool DRAW_LIGHTPOINTS_AXIS = true;

    bool DEBUG_BSP_MODE = false;
    bool DEBUG_MESH_MODE = false;

    // Z BUFFER
    bool TRIANGLE_RENDER_DEPTH_BUFFER = true;

    bool SHOW_SHADOWMAPPING_BUFFER = false;
    bool ENABLE_SHADOW_CASTING = false;
    bool ENABLE_LIGHTS = true;

    bool TRIANGLE_RENDER_CLIPPING = true;

    bool DRAW_FRUSTUM = false;

    // FRUSTUM PLANES
    int NEAR_PLANE   = 0;
    int FAR_PLANE    = 1;
    int LEFT_PLANE   = 2;
    int RIGHT_PLANE  = 3;
    int TOP_PLANE    = 4;
    int BOTTOM_PLANE = 5;

    // CONFIG VALUES
    float FRUSTUM_CLIPPING_DISTANCE = 0.0001f;

    std::string ICON_LIGHTPOINTS_DEFAULT = "../assets/icons/lightpoint.tga";
    std::string ICON_OBJECT3D_DEFAULT = "../assets/icons/object3d.tga";

    std::string SPRITES_FOLDER = "../assets/sprites/";
    std::string ASSETS_FOLDER = "../assets/";

    // Show Extra Line Demo when triangle stripped in two when rasterizing
    bool TRIANGLE_DEMO_EXTRALINE_ENABLED = false;
    Uint32 TRIANGLE_DEMO_EXTRALINE_COLOR = 0x00FF00; // GREEN

    // GUI Setup values
    float GUI_BAR_DEFAULT_MIN_VALUE = -999999999.f;
    float GUI_BAR_DEFAULT_MAX_VALUE = -999999999.f;

    float GUI_BAR_SENSITIVITY = -0.1f;

    int GUI_MIN_SPRITE3D_FRAMERATE = 1;
    int GUI_MAX_SPRITE3D_FRAMERATE = 30;

    // SpriteDirectional3D Default size
    float SPRITE3D_DEFAULT_WIDTH = 25.f;
    float SPRITE3D_DEFAULT_HEIGHT = 25.f;

    int TEXT_3D_SIZE = 25;
    Uint32 TEXT_3D_COLOR = 0x00FF00; // GREEN
    bool TEXT_ON_OBJECT3D = false;

    // KEYBOARD & MOUSE
    float WALKING_SPEED     = 30.0;
    float TURN_SPEED        = 10.0;
    float PITCH_SPEED       = 2.0;
    float STRAFE_SPEED      = 30.0;
    float MOUSE_SENSITIVITY	= 0.5;

    // COLLISION SYSTEM
    bool BSP_COLLISIONS_ENABLED = false;
    bool ENABLE_GRAVITY = false;
    bool ENABLE_FRICTION = true;
    bool ENABLE_AIR_FRICTION = true;
    bool ENABLE_FLYING = true;

    float PLAYER_SPHERE_RADIUS = 1;

    Vertex3D JUMP_FORCE = Vertex3D(0, -150, 0);

    const float FRICTION_COEFICIENT = 0.25f;
    const float AIR_RESISTANCE = 0.0005f;

    float TESTING = 0.0000f;

    int LOAD_OF_DETAIL = 1;
    bool ENABLE_MIPMAPPING = true;

    bool ENABLE_LIGHTMAPPING = true;
    bool SHOW_LIGHTMAPPING = false;
    float LIGHTMAPPING_INTENSITY = 0.015;
};


#endif //SDL2_3D_ENGINE_ENGINESETUP_H
