#ifndef SDL2_3D_ENGINE_ENGINESETUP_H
#define SDL2_3D_ENGINE_ENGINESETUP_H

#include "Color.h"
#include "../Objects/Vertex3D.h"
#include "Timer.h"

class EngineSetup {

public:
    EngineSetup();
    ~EngineSetup();

    static EngineSetup* getInstance();
    static EngineSetup* instance;

    static void setResolution(int, int);

    std::string ENGINE_TITLE = "Brakeza3D v0.1";

    Vertex3D CameraPosition = Vertex3D(1, 1, -5);

    Vertex3D right   = Vertex3D(1, 0, 0);
    Vertex3D up      = Vertex3D(0, 1, 0);
    Vertex3D forward = Vertex3D(0, 0, 1);

    Vertex3D left     = right.getInverse();
    Vertex3D down     = up.getInverse();
    Vertex3D backward = forward.getInverse();

    Vertex3D gravity = Vertex3D(0, 300.f, 0);

    // Screen dimension constants
    int screenWidth = 320;
    int screenHeight = 240;

    const int RESOLUTION = screenWidth * screenHeight;

    bool BASED_TILE_RENDER = false;

    bool CAMERA_MOUSE_ROTATION = true;

    const int ENGINE_MAX_GAMEOBJECTS  = 1500;
    const int ENGINE_MAX_FRAGMENTS    = 256000;
    const int ENGINE_MAX_OCLTRIANGLES = 10240;

    const float EPSILON = 0.00001f;

    bool RASTERIZER_OPENCL = false;
    bool TRANSFORMS_OPENCL = false;

    bool DRAW_TILES_GRID = false;

    // Draw axis
    bool RENDER_MAIN_AXIS = true;
    bool RENDER_OBJECTS_AXIS = false;

    bool DEBUG_RENDER_INFO = false;
    bool OPENCL_SHOW_TIME_KERNELS = false;

    // FaceCulling
    bool TRIANGLE_BACK_FACECULLING = true;

    bool MENU_ACTIVE = true;

    // Fill Triangle modes
    bool TRIANGLE_MODE_PIXELS = false;
    bool TRIANGLE_MODE_WIREFRAME = false;
    bool TRIANGLE_MODE_COLOR_SOLID = false;
    bool TRIANGLE_MODE_TEXTURIZED = true;

    bool TEXTURES_BILINEAR_INTERPOLATION = false;
    bool LIGHTMAPS_BILINEAR_INTERPOLATION = true;

    bool DRAW_OBJECT3D_BILLBOARD = false;
    bool DRAW_LIGHTPOINTS_BILLBOARD = true;
    bool DRAW_LIGHTPOINTS_AXIS = true;

    bool DEBUG_BSP_MODE = false;
    bool DEBUG_MESH_MODE = false;

    // Z BUFFER
    bool TRIANGLE_RENDER_DEPTH_BUFFER = true;

    bool SHOW_SHADOWMAPPING_BUFFER = false;
    bool ENABLE_SHADOW_CASTING = false;
    bool ENABLE_LIGHTS = true;

    bool DRAW_FRUSTUM = false;
    bool DRAW_FPS = false;

    // FRUSTUM PLANES
    int NEAR_PLANE   = 0;
    int FAR_PLANE    = 1;
    int LEFT_PLANE   = 2;
    int RIGHT_PLANE  = 3;
    int TOP_PLANE    = 4;
    int BOTTOM_PLANE = 5;

    // CONFIG VALUES
    float FRUSTUM_CLIPPING_DISTANCE = 0.001f;

    // BSP ITEM BILLBOARDS
    std::string ICON_LIGHTPOINTS_DEFAULT = "../assets/icons/light.tga";
    std::string ICON_OBJECT3D_DEFAULT = "../assets/icons/object3d.tga";
    std::string ICON_WEAPON_SHOTGUN = "../assets/icons/weapon_shotgun.tga";
    std::string ICON_ITEM_HEALTH = "../assets/icons/item_health.tga";
    std::string ICON_FUNC_BUTTON = "../assets/icons/func_button.tga";
    std::string ICON_MONSTER_GENERIC = "../assets/icons/monster.tga";
    std::string ICON_SHIELD_GENERIC = "../assets/icons/shield.tga";
    std::string ICON_INFO_PLAYER_START = "../assets/icons/info_player_start.tga";
    std::string ICON_INFO_TELEPORT_DESTINATION = "../assets/icons/info_teleport_destination.tga";
    std::string ICON_LIGHT_FLAME = "../assets/icons/light_flame.tga";
    std::string SPRITES_FOLDER = "../assets/sprites/";
    std::string MAPS_FOLDER = "../assets/maps/";
    std::string IMAGES_FOLDER = "../assets/images/";
    std::string ASSETS_FOLDER = "../assets/";
    std::string CONFIG_FOLDER = "../config/";
    std::string SOUNDS_FOLDER = "../assets/sounds/";

    std::string SOUND_MAINMENU = "tartani.wav";
    std::string SOUND_WEAPON_1 = "gun_shot_1.wav";
    std::string SOUND_BASE_LEVEL_0 = "base_level_0.wav";

    std::string CFG_MAPS = "maps.json";
    std::string CFG_MENU = "menu.json";
    bool RENDER_BSP_MAP = true;

    // Show Extra Line Demo when triangle stripped in two when rasterizing
    bool TRIANGLE_DEMO_EXTRALINE_ENABLED = false;

    // GUI Setup values
    float GUI_BAR_DEFAULT_MIN_VALUE = -999999999.f;
    float GUI_BAR_DEFAULT_MAX_VALUE = -999999999.f;

    float GUI_BAR_SENSITIVITY = -0.1f;

    int GUI_MIN_SPRITE3D_FRAMERATE = 1;
    int GUI_MAX_SPRITE3D_FRAMERATE = 30;

    // SpriteDirectional3D Default size
    float BILLBOARD_WIDTH_DEFAULT = 3.f;
    float BILLBOARD_HEIGHT_DEFAULT = 3.f;

    int TEXT_3D_SIZE = 40;
    Uint32 TEXT_3D_COLOR = 0x00FF00; // GREEN
    Uint32 CROSSHAIR_COLOR = 0x00FF00; // GREEN
    bool TEXT_ON_OBJECT3D = false;

    // KEYBOARD
    float WALKING_SPEED     = 0.9f;
    float TURN_SPEED        = 0.099f;
    float PITCH_SPEED       = 0.099f;
    float STRAFE_SPEED      = 0.9f;

    float WALKING_SPEED_LIQUID_DIVISOR = 2.0;

    // MOUSE
    float MOUSE_SENSITIVITY	= 0.50;

    bool HEAD_BOB = false;

    // COLLISION SYSTEM
    bool BULLET_STEP_SIMULATION = false;
    bool BULLET_DEBUG_MODE = false;
    bool BULLET_CHECK_ALL_PAIRS = true;

    float PLAYER_SPHERE_RADIUS = 30.f;

    Vertex3D JUMP_FORCE = Vertex3D(0, -60, 0);

    const float AIR_RESISTANCE = 1.1f;

    float TESTING = 0;
    float TESTING_INT = 0;
    bool TESTING_BOOL = false;

    int LOAD_OF_DETAIL = 1;
    bool ENABLE_MIPMAPPING = true;

    bool ENABLE_LIGHTMAPPING = true;
    bool SHOW_LIGHTMAPPING = false;
    float LIGHTMAPPING_INTENSITY = 0.08f;
    float TEXTURE_INTENSITY = 0.1f;

    bool DRAW_BSP_HULLS = false;

    // BSP TEXTURES ANIMATED
    bool TRIANGLE_TEXTURES_ANIMATED = true;

    //BSP LAVA EFFECT
    float LAVA_CLOSENESS = 1.0f;
    float LAVA_INTENSITY = 0.5f;
    float LAVA_SPEED = 0.5f;
    float LAVA_SCALE = 1.f;

    //LIGHTNING  EFFECT
    float LIGHTNING_GENERATIONS = 5;
    float LIGHTNING_OFFSET_REDUCTION = 5.f;
    float LIGHTNING_PROBABILITY_BRANCH = 1.5;
    float LIGHTNING_SEGMENT_SHIFT = 0.7;

    //WEAPON
    bool SHOW_WEAPON = false;

    bool LOG_COLLISION_OBJECTS = false;
    bool LOG_LEAF_TYPE = false;

    char *LIGHT_PATTERNS[12];

    bool CFG_AUTOLOAD_MAP = true;

    bool EVENT_GUI = false;
    int EVENT_LAUNCH = 0;
    int EVENT_GUI_CHANGE_MAP = 1000;
    std::string EVENT_DATA = "";

    int FIRE_WIDTH = 320;
    int FIRE_HEIGHT = 240;

    enum SpriteDoom2SoldierAnimations {
        SOLDIER_WALK = 0,
        SOLDIER_FIRE = 1,
        SOLDIER_INJURIED = 2,
        SOLDIER_DEAD = 3
    };

    enum SpriteDoom2CacodemonAnimations {
        FLY = 0,
        DEAD = 1
    };
    enum SpriteShotgunAnimations {
        READY,
        RELOAD,
        SHOT
    };

    enum SpriteGuyAnimations {
        NORMAL,
    };

    enum WeaponsTypes {
        WEAPON_TYPE_MELEE = 0,
        WEAPON_TYPE_GUN = 1,
        WEAPON_TYPE_MACHINEGUN = 2,
        WEAPON_TYPE_ROCKETLAUNCHER = 3,
    };

    enum WeaponsActions {
        WEAPON_ACTION_WALK = 0,
        WEAPON_ACTION_FIRE = 1,
        WEAPON_ACTION_RELOAD = 2,
    };

};

#endif //SDL2_3D_ENGINE_ENGINESETUP_H
