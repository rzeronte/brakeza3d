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
    ~EngineSetup();

    static EngineSetup* getInstance();
    static EngineSetup* instance;

    static void setResolution(int, int);

    std::map<std::string, std::any> setup;

    std::string ENGINE_TITLE = "Brakeza3D v0.6";

    std::string cameraNameIdentifier        = "Camera";
    std::string cameraTriggerNameIdentifier = "triggerCamera";

    Vertex3D CameraPosition = Vertex3D(1, 1, -5);

    Vertex3D right   = Vertex3D(1, 0, 0);
    Vertex3D up      = Vertex3D(0, 1, 0);
    Vertex3D forward = Vertex3D(0, 0, 1);

    Vertex3D left     = right.getInverse();
    Vertex3D down     = up.getInverse();
    Vertex3D backward = forward.getInverse();

    Vertex3D gravity = Vertex3D(0, 300.f, 0);
    float    BULLET_FALL_SPEED = 256;

    // Screen dimension constants
    int screenWidth  = 320;
    int screenHeight = 240;

    int RESOLUTION = screenWidth * screenHeight;

    bool BASED_TILE_RENDER = true;
    bool BASED_TILE_RENDER_THREADED = true;

    const int ENGINE_MAX_OCLTRIANGLES = 10240;

    const float EPSILON = 0.00001f;

    bool  ENABLE_FOG    = true;
    float FOG_DISTANCE  = 75;
    float FOG_INTENSITY = 1;
    float FOG_COLOR = Color::FOGDefault();
    ImVec4 FOG_IMGUI_COLOR;
    float FRUSTUM_FARPLANE_DISTANCE = 75;
    float HORIZONTAL_FOV = 90;
    float MIN_TRIANGLE_AREA = 2.5;

    bool RASTERIZER_OPENCL = false;

    bool DRAW_TILES_GRID = false;

    // Draw axis
    bool RENDER_MAIN_AXIS = true;
    bool RENDER_OBJECTS_AXIS = false;

    bool DEBUG_RENDER_INFO = false;
    bool OPENCL_SHOW_TIME_KERNELS = false;

    bool MENU_ACTIVE = false;
    bool LOADING = false;
    bool SPLASHING = true;
    bool FADEIN = false;
    bool FADEOUT = false;

    // Fill Triangle modes
    bool TRIANGLE_MODE_PIXELS = false;
    bool TRIANGLE_MODE_WIREFRAME = false;
    bool TRIANGLE_MODE_COLOR_SOLID = false;
    bool TRIANGLE_MODE_TEXTURIZED = true;

    bool DRAW_DECAL_WIREFRAMES = false;

    bool TEXTURES_BILINEAR_INTERPOLATION = false;
    bool LIGHTMAPS_BILINEAR_INTERPOLATION = false;

    bool DRAW_OBJECT3D_BILLBOARD = false;
    bool DRAW_LIGHTPOINTS_BILLBOARD = true;
    bool DRAW_LIGHTPOINTS_AXIS = true;

    // Z BUFFER
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
    float FRUSTUM_CLIPPING_DISTANCE = 0.000001f;

    // BSP ITEM BILLBOARDS
    std::string SPRITES_FOLDER = "../assets/sprites/";
    std::string MAPS_FOLDER = "../assets/maps/";
    std::string IMAGES_FOLDER = "../assets/images/";
    std::string ASSETS_FOLDER = "../assets/";
    std::string CONFIG_FOLDER = "../config/";
    std::string SOUNDS_FOLDER = "../assets/sounds/";
    std::string HUD_FOLDER = ASSETS_FOLDER + "textures/HUD/";
    std::string FONTS_FOLDER = ASSETS_FOLDER + "fonts/";
    std::string WEAPONS_FOLDER = ASSETS_FOLDER + "weapons/";
    std::string TEXTURES_FOLDER = ASSETS_FOLDER + "textures/";

    std::string ITEM_FIRSTAID_ICON = "first_aid.png";

    std::string CFG_MAPS    = "maps.json";
    std::string CFG_MENU    = "menu.json";
    std::string CFG_WEAPONS = "weapons.json";
    std::string CFG_ENEMIES = "enemies.json";
    std::string CFG_SOUNDS  = "sounds.json";

    bool  SOUND_ENABLED = true;
    float SOUND_VOLUME_MUSIC = 25;
    float SOUND_VOLUME_PLAYER = 128;
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
    float WALKING_SPEED     = 0.6f;
    float TURN_SPEED        = 0.099f;
    float PITCH_SPEED       = 0.099f;
    float STRAFE_SPEED      = 0.75f;

    float WALKING_SPEED_LIQUID_DIVISOR = 1.5;

    // MOUSE
    float MOUSE_SENSITIVITY	= 0.250;

    // COLLISION SYSTEM
    bool BULLET_STEP_SIMULATION = true;
    bool BULLET_DEBUG_MODE = false;
    bool BULLET_CHECK_ALL_PAIRS = true;

    float PLAYER_SPHERE_RADIUS = 30.f;

    Vertex3D JUMP_FORCE = Vertex3D(0, -80, 0);

    const float AIR_RESISTANCE = 1.1f;

    float TESTING = 1.0;
    float TESTING_INT = 0;
    bool  TESTING_BOOL = false;

    int  LOAD_OF_DETAIL = 1;
    bool ENABLE_MIPMAPPING = true;

    bool  ENABLE_LIGHTMAPPING = true;
    bool  SHOW_LIGHTMAPPING = false;
    float LIGHTMAPPING_INTENSITY = 0.08f;
    float TEXTURE_INTENSITY = 0.1f;

    bool DRAW_BSP_HULLS = false;
    bool DRAW_SPRITES = true;
    bool DRAW_WEAPON = false;
    bool DRAW_HUD = true;

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

    bool LOG_COLLISION_OBJECTS = false;
    bool LOG_WEAPONS_SYSTEM = false;
    bool LOG_LEAF_TYPE = false;

    char *LIGHT_PATTERNS[12];

    int FIRE_WIDTH = 320;
    int FIRE_HEIGHT = 240;

    bool  ENABLE_IA = true;
    float TIME_STEP_IA_ENEMIES = 0.50f;
    float ZOOM_FOV = 40;

    bool WEAPON_AUTOSWICH = true;

    int GAME_PLAYER_STAMINA_INITIAL = 100;

    float PLAYER_CAPSULE_RADIUS = 1.50f;
    float PLAYER_CAPSULE_HEIGHT = 4.5f;

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
    };

    enum SpriteSoldierAnimations {
        SOLDIER_WALK = 0,
        SOLDIER_FIRE = 1,
        SOLDIER_INJURIED = 2,
        SOLDIER_DEAD = 3,
        SOLDIER_EXPLODE = 4
    };

    enum SpriteDoom2CacodemonAnimations {
        FLY = 0,
        DEAD = 1
    };

    enum WeaponsTypes {
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
        WALKING = 0,
    };

    enum CollisionResolverTypes {
        COLLISION_RESOLVER_PROJECTILE_AND_NPCENEMY = 1,
        COLLISION_RESOLVER_CAMERA_AND_FUNCDOOR = 2,
        COLLISION_RESOLVER_CAMERA_AND_FUNCBUTTON = 3,
        COLLISION_RESOLVER_PROJECTILE_AND_BSPMAP = 4,
        COLLISION_RESOLVER_NPCENEMYPART_AND_BSPMAP = 5,
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
    
    enum ComponentID {
        COMPONENT_WINDOW = 0,
        COMPONENT_CAMERA = 1,
        COMPONENT_COLLISIONS = 2,
        COMPONENT_INPUT = 3,
        COMPONENT_BSP = 4,
        COMPONENT_SOUND = 5,
        COMPONENT_RENDER = 6,
        COMPONENT_GUI = 7,
        COMPONENT_MENU = 8,
        COMPONENT_WEAPONS = 9,
        COMPONENT_HUD = 10,
        COMPONENT_GAME = 11,
        COMPONENT_GAME_INPUT = 12
    };

};

#endif //SDL2_3D_ENGINE_ENGINESETUP_H
