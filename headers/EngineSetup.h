#ifndef SDL2_3D_ENGINE_ENGINESETUP_H
#define SDL2_3D_ENGINE_ENGINESETUP_H

#include "Misc/Color.h"
#include "Objects/Vertex3D.h"
#include "Misc/Timer.h"
#include "../imgui/imgui.h"
#include <any>
#include <map>

class EngineSetup {

public:
    EngineSetup();

    static EngineSetup *getInstance();

    static EngineSetup *instance;

    static void setResolution(int, int);

    std::string LOG_FILE = "brakeza.log";
    bool LOGGING = true;
    bool LOGGING_TO_FILE = false;

    std::string ENGINE_TITLE = "Brakeza3D v0.6";

    std::string cameraNameIdentifier = "Camera";
    std::string cameraTriggerNameIdentifier = "triggerCamera";

    Vertex3D right = Vertex3D(1, 0, 0);
    Vertex3D up = Vertex3D(0, 1, 0);
    Vertex3D forward = Vertex3D(0, 0, 1);

    Vertex3D left = right.getInverse();
    Vertex3D down = up.getInverse();
    Vertex3D backward = forward.getInverse();

    Vertex3D gravity = Vertex3D(0, 10.0f, 0);

    // Screen dimension constants
    int screenWidth = 640;
    int screenHeight = 480;

    int RESOLUTION = screenWidth * screenHeight;

    bool BASED_TILE_RENDER = false;
    bool BASED_TILE_RENDER_THREADED = false;

    const int ENGINE_MAX_OCLTRIANGLES = 10240;

    float EPSILON = 0.00001f;

    bool ENABLE_FOG = true;
    float FOG_DISTANCE = 1000;
    float FOG_INTENSITY = 1;
    float FOG_COLOR = Color::FOGDefault();
    ImVec4 FOG_IMGUI_COLOR;
    float FRUSTUM_FARPLANE_DISTANCE = 1250;
    float HORIZONTAL_FOV = 90;

    bool RASTERIZER_OPENCL = false;

    bool DRAW_TILES_GRID = false;

    // Draw axis
    bool RENDER_MAIN_AXIS = true;
    bool RENDER_OBJECTS_AXIS = false;

    bool EXECUTE_GAMEOBJECTS_ONUPDATE = true;

    bool RENDER_BSP_TRIANGLES = true;

    bool DEBUG_RENDER_INFO = false;
    bool OPENCL_SHOW_TIME_KERNELS = false;

    bool MENU_ACTIVE = true;
    bool LOADING = false;
    bool SPLASHING = true;
    bool FADEIN = false;
    bool FADEOUT = false;

    // Fill Triangle modes
    bool TRIANGLE_MODE_PIXELS = false;
    bool TRIANGLE_MODE_WIREFRAME = false;
    bool TRIANGLE_MODE_COLOR_SOLID = false;
    bool TRIANGLE_MODE_TEXTURIZED = true;

    bool DRAW_ANIMATION_BONES = false;
    bool TRIANGLE_MODE_VERTEX_WEIGHT = false;
    bool DRAW_DECAL_WIREFRAMES = false;

    bool TEXTURES_BILINEAR_INTERPOLATION = false;
    bool LIGHTMAPS_BILINEAR_INTERPOLATION = false;

    bool DRAW_OBJECT3D_BILLBOARD = false;
    bool DRAW_LIGHTPOINTS_BILLBOARD = true;
    bool DRAW_LIGHTPOINTS_AXIS = true;

    bool DRAW_MESH3D_AABB = false;
    bool DRAW_MESH3D_OCTREE = false;
    bool DRAW_MESH3D_GRID = false;
    bool DRAW_MESH3D_GRID_POINTS = true;
    bool DRAW_MESH3D_GRID_CUBES = false;
    bool DRAW_MESH3D_GRID_EMPTY = true;
    bool DRAW_MESH3D_GRID_NO_EMPTY = true;

    // Z BUFFER
    bool ENABLE_SHADOW_CASTING = false;
    bool ENABLE_LIGHTS = true;

    bool DRAW_FRUSTUM = false;
    bool DRAW_FPS = false;

    // FRUSTUM PLANES
    int NEAR_PLANE = 0;
    int FAR_PLANE = 1;
    int LEFT_PLANE = 2;
    int RIGHT_PLANE = 3;
    int TOP_PLANE = 4;
    int BOTTOM_PLANE = 5;

    // CONFIG VALUES
    float FRUSTUM_CLIPPING_DISTANCE = 0.00075f;
    bool ENABLE_CLIPPING = true;

    // BSP ITEM BILLBOARDS
    std::string SPRITES_FOLDER = "../assets/sprites/";
    std::string MAPS_FOLDER = "../assets/maps/";
    std::string IMAGES_FOLDER = "../assets/images/";
    std::string ASSETS_FOLDER = "../assets/";
    std::string CONFIG_FOLDER = "../config/";
    std::string SOUNDS_FOLDER = "../assets/sounds/";
    std::string MODELS_FOLDER = "../assets/models/";
    std::string HUD_FOLDER = ASSETS_FOLDER + "textures/HUD/";
    std::string FONTS_FOLDER = ASSETS_FOLDER + "fonts/";
    std::string WEAPONS_FOLDER = ASSETS_FOLDER + "weapons/";
    std::string TEXTURES_FOLDER = ASSETS_FOLDER + "textures/";
    std::string GRIDS_FOLDER = ASSETS_FOLDER + "grids/";

    std::string CFG_MAPS = "maps.json";
    std::string CFG_MENU = "menu.json";
    std::string CFG_WEAPONS = "weapons.json";
    std::string CFG_ENEMIES = "enemies.json";
    std::string CFG_SOUNDS = "sounds.json";

    bool SOUND_ENABLED = true;
    float SOUND_VOLUME_MUSIC = 128;
    float SOUND_VOLUME_PLAYER = 75;
    float SOUND_VOLUME_ENVIRONMENT = 75;
    float SOUND_VOLUME_MENU = 75;

    // GUI Setup values
    float GUI_BAR_DEFAULT_MIN_VALUE = -999999999.f;
    float GUI_BAR_DEFAULT_MAX_VALUE = -999999999.f;

    float GUI_BAR_SENSITIVITY = -0.1f;

    int GUI_MIN_SPRITE3D_FRAMERATE = 1;
    int GUI_MAX_SPRITE3D_FRAMERATE = 30;

    // SpriteDirectional3D Default size
    float BILLBOARD_WIDTH_DEFAULT = 1.f;
    float BILLBOARD_HEIGHT_DEFAULT = 1.f;

    Uint32 TEXT_3D_COLOR = 0x00FF00; // GREEN
    Uint32 CROSSHAIR_COLOR = 0x00FF00; // GREEN
    bool DRAW_CROSSHAIR = true;
    bool TEXT_ON_OBJECT3D = false;

    // KEYBOARD
    float WALKING_SPEED = 4.1f;
    float TURN_SPEED = 5.f;
    float PITCH_SPEED = 5.f;
    float STRAFE_SPEED = 4.1f;

    // MOUSE
    float MOUSE_SENSITIVITY = 0.250;

    // COLLISION SYSTEM
    bool BULLET_STEP_SIMULATION = true;
    float BULLET_STEP_SIMULATION_MULTIPLIER = 1;
    bool BULLET_DEBUG_MODE = false;
    bool BULLET_CHECK_ALL_PAIRS = true;

    float PLAYER_SPHERE_RADIUS = 30.f;

    Vertex3D JUMP_FORCE = Vertex3D(0, -150, 0);

    float TESTING = 64.0;
    float TESTING_INT1 = 1;
    float TESTING_INT2 = 2;
    float TESTING_INT3 = 3;
    float TESTING_INT4 = 4;
    float TESTING_INT5 = 5;

    bool TESTING_BOOL = false;

    int LOAD_OF_DETAIL = 1;
    bool ENABLE_MIPMAPPING = true;

    bool ENABLE_LIGHTMAPPING = true;
    bool SHOW_LIGHTMAPPING = false;
    float LIGHTMAPPING_BLEND_INTENSITY = 0.05f;
    float LIGHTMAPPING_INTENSITY = 0.75f;

    bool DRAW_BSP_HULLS = false;
    bool DRAW_SPRITES = true;
    bool DRAW_WEAPON = false;
    bool DRAW_HUD = true;
    bool DRAW_BSP_CAMERA_HULL_CONTENTS = false;

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

    //LIGHTNING  EFFECT
    float FIRE_DIVISOR1 = 32;
    float FIRE_DIVISOR2 = 129;

    bool LOG_COLLISION_OBJECTS = false;
    bool LOG_WEAPONS_SYSTEM = false;
    bool LOG_LEAF_TYPE = false;

    char *LIGHT_PATTERNS[12];

    int FIRE_WIDTH = 320;
    int FIRE_HEIGHT = 240;

    bool ENABLE_IA = true;
    float ZOOM_FOV = 40;

    bool WEAPON_AUTOSWICH = false;

    int GAME_PLAYER_STAMINA_INITIAL = 100;

    float PLAYER_CAPSULE_RADIUS = 16.00f;
    float PLAYER_CAPSULE_HEIGHT = 32.00f;

    enum collisionGroups {
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
    };

    enum SpriteSoldierAnimations {
        SOLDIER_IDLE = 0,
        SOLDIER_WALK = 1,
        SOLDIER_FIRE = 2,
        SOLDIER_INJURIED = 3,
        SOLDIER_DEAD = 4
    };

    enum SpriteDoom2CacodemonAnimations {
        FLY = 0,
        DEAD = 1
    };

    enum WeaponsTypes {
        EMPTY = -1,
        PISTOL = 0,
        REPEATER = 1,
        STATIC_RIFLE = 2,
        HAR = 3,
        CHAINGUN = 4,
        GAUSS_CANNON = 5,
        RAILGUN = 6,
        ROCKETLAUNCHER = 7,
    };

    enum WeaponsActions {
        NONE = -1,
        IDLE = 0,
        FIRE = 1,
        RELOAD = 2,
    };

    enum CollisionResolverTypes {
        COLLISION_RESOLVER_PROJECTILE_AND_NPCENEMY = 1,
        COLLISION_RESOLVER_CAMERA_AND_FUNCDOOR = 2,
        COLLISION_RESOLVER_CAMERA_AND_FUNCBUTTON = 3,
        COLLISION_RESOLVER_CAMERA_AND_TRIGGER_MULTIPLE = 4,
        COLLISION_RESOLVER_CAMERA_AND_TRIGGER_TELEPORT = 5,
        COLLISION_RESOLVER_PROJECTILE_AND_CAMERA = 6,
        COLLISION_RESOLVER_ITEMWEAPON_AND_CAMERA = 7,
        COLLISION_RESOLVER_ITEMHEALTH_AND_CAMERA = 8,
        COLLISION_RESOLVER_ITEMAMMO_AND_CAMERA = 9
    };

    enum SoundChannels {
        SND_GLOBAL = -1,
        SND_MENU = 0,
        SND_PLAYER = 1,
        SND_PLAYER_STEPS = 2,
        SND_ENVIRONMENT = 3,
        SND_EXPLODE_ENEMY = 4,
        SND_WEAPON = 5,
        SND_WEAPON_LOOP = 6,
        SND_BLOOD_HIT = 7,
        SND_CASINGS = 8
    };
};

#endif //SDL2_3D_ENGINE_ENGINESETUP_H
