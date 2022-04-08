#ifndef SDL2_3D_ENGINE_ENGINESETUP_H
#define SDL2_3D_ENGINE_ENGINESETUP_H

#include "Misc/Color.h"
#include "Objects/Vertex3D.h"
#include "Misc/Timer.h"
#include "../imgui/imgui.h"
#include <map>

class EngineSetup {

public:
    EngineSetup();

    static EngineSetup *get();

    static EngineSetup *instance;

    bool LOGGING = true;
    bool LOGGING_TO_FILE = false;

    std::string ENGINE_TITLE = "Brakeza3D v0.9";

    std::string cameraNameIdentifier = "Camera";
    std::string cameraTriggerNameIdentifier = "triggerCamera";

    Vertex3D right = Vertex3D(1, 0, 0);
    Vertex3D up = Vertex3D(0, 1, 0);
    Vertex3D forward = Vertex3D(0, 0, 1);

    Vertex3D left = right.getInverse();
    Vertex3D down = up.getInverse();
    Vertex3D backward = forward.getInverse();

    Vertex3D defaultCameraPosition = Vertex3D(1, 1, 1);
    Vertex3D defaultCameraAnglesRotation = Vertex3D(190, 0, 0);

    Vertex3D gravity = Vertex3D(0, -900.0f, 0);

    // Screen dimension constants
    const int screenWidth = 640;
    const int screenHeight = 480;

    float screenWidthHalf = (float) screenWidth / 2;
    float screenHeightHalf = (float) screenHeight / 2;

    int RESOLUTION = screenWidth * screenHeight;

    bool BASED_TILE_RENDER = true;
    bool BASED_TILE_RENDER_THREADED = true;

    float EPSILON = 0.00001f;

    bool ENABLE_FOG = false;
    float FOG_DISTANCE = 4500;
    float FOG_INTENSITY = 1.f;
    Color FOG_COLOR = Color::FOGDefault();
    float FRUSTUM_FARPLANE_DISTANCE = 9000000;
    float HORIZONTAL_FOV = 90;

    bool DRAW_TILES_GRID = false;

    // Draw axis
    bool RENDER_MAIN_AXIS = true;
    bool RENDER_OBJECTS_AXIS = false;
    float OBJECT_AXIS_SIZE = 10;

    bool EXECUTE_GAMEOBJECTS_ONUPDATE = true;

    bool DEBUG_RENDER_INFO = false;


    // Fill Triangle modes
    bool TRIANGLE_MODE_PIXELS = false;
    bool TRIANGLE_MODE_WIREFRAME = false;
    bool TRIANGLE_MODE_COLOR_SOLID = false;
    bool TRIANGLE_MODE_TEXTURIZED = true;
    bool DRAW_MAIN_DEEP_MAPPING = false;

    bool DRAW_ANIMATION_BONES = false;
    bool DRAW_DECAL_WIREFRAMES = false;

    bool TEXTURES_BILINEAR_INTERPOLATION = false;
    bool LIGHTMAPS_BILINEAR_INTERPOLATION = false;

    bool DRAW_OBJECT3D_BILLBOARD = false;
    bool DRAW_LIGHTPOINTS_BILLBOARD = true;
    bool DRAW_LIGHTPOINTS_AXIS = true;
    bool DRAW_PENDULUM_THREAD = false;


    bool DRAW_MESH3D_AABB = false;
    bool DRAW_MESH3D_OCTREE = false;
    bool DRAW_MESH3D_GRID = false;
    bool DRAW_MESH3D_GRID_POINTS = true;
    bool DRAW_MESH3D_GRID_CUBES = false;
    bool DRAW_MESH3D_GRID_EMPTY = true;
    bool DRAW_MESH3D_GRID_NO_EMPTY = true;

    // Z BUFFER
    bool CREATE_LIGHT_ZBUFFER = false;
    bool ENABLE_SHADOW_MAPPING = false;
    bool ENABLE_LIGHTS = true;
    bool ENABLE_LIGHTS_SPECULAR = true;
    bool DRAW_LIGHTS_DIRECTION = false;
    float LIGHTS_DIRECTION_SIZE = 500;

    bool DRAW_FRUSTUM = false;
    bool DRAW_FPS = true;

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
    std::string ICONS_FOLDER = ASSETS_FOLDER + "icons/";

    std::string CFG_MAPS = "maps.json";
    std::string CFG_MENU = "menu.json";
    std::string CFG_WEAPONS = "playerWeapons.json";
    std::string CFG_ENEMIES = "level01.json";
    std::string CFG_SOUNDS = "sounds.json";

    std::string CONCHARS_SPRITE_FILE = "conchars.png";
    std::string DEFAULT_SHADER_BACKGROUND_IMAGE = "background_scrollable.png";
    std::string DEFAULT_HELP_IMAGE = "xbox_one_controller.png";

    std::string LOGO_BRAKEZA = "brakeza.png";

    bool SOUND_ENABLED = true;
    float SOUND_VOLUME_MUSIC = 128;
    float SOUND_VOLUME_PLAYER = 75;
    float SOUND_VOLUME_ENVIRONMENT = 75;
    float SOUND_VOLUME_ENEMIES = 75;
    float SOUND_VOLUME_MENU = 75;

    // SpriteDirectional3D Default size
    float BILLBOARD_WIDTH_DEFAULT = 100.f;
    float BILLBOARD_HEIGHT_DEFAULT = 100.f;

    Uint32 CROSSHAIR_COLOR = 0x00FF00; // GREEN
    bool DRAW_CROSSHAIR = false;
    bool TEXT_ON_OBJECT3D = false;

    // KEYBOARD
    float WALKING_SPEED = 45.0f;
    float TURN_SPEED = 5.f;
    float PITCH_SPEED = 5.f;
    float STRAFE_SPEED = 45.0f;

    // MOUSE
    float MOUSE_SENSITIVITY = 0.250;

    // COLLISION SYSTEM
    bool BULLET_STEP_SIMULATION = true;
    bool BULLET_DEBUG_MODE = false;
    bool BULLET_CHECK_ALL_PAIRS = true;

    float TESTING = 64.0;
    float TESTING_INT1 = 1;
    float TESTING_INT2 = 2;
    float TESTING_INT3 = 3;
    float TESTING_INT4 = 4;

    bool TESTING_BOOL = false;

    int LOAD_OF_DETAIL = 1;
    bool ENABLE_MIPMAPPING = true;

    bool ENABLE_LIGHTMAPPING = true;
    bool SHOW_LIGHTMAPPING = false;
    float LIGHTMAPPING_BLEND_INTENSITY = 0.05f;
    float LIGHTMAPPING_INTENSITY = 0.75f;

    bool DRAW_HUD = true;

    // BSP TEXTURES ANIMATED
    bool TRIANGLE_TEXTURES_ANIMATED = true;

    //BSP LAVA EFFECT
    float LAVA_CLOSENESS = 1.0f;
    float LAVA_INTENSITY = 0.5f;
    float LAVA_SPEED = 0.5f;
    float LAVA_SCALE = 1.f;

    //LIGHTNING EFFECT
    float LIGHTNING_GENERATIONS = 5;
    float LIGHTNING_OFFSET_REDUCTION = 500.f;
    float LIGHTNING_PROBABILITY_BRANCH = 1.5f;
    float LIGHTNING_SEGMENT_SHIFT = 2.5f;

    //LIGHTNING  EFFECT
    float FIRE_DIVISOR1 = 32;
    float FIRE_DIVISOR2 = 129;

    bool LOG_COLLISION_OBJECTS = false;
    bool LOG_WEAPONS_SYSTEM = false;

    bool ENABLE_IA = true;
    float ZOOM_FOV = 40;

    float PLAYER_CAPSULE_RADIUS = 16.00f;
    float PLAYER_CAPSULE_HEIGHT = 32.00f;

    float PROJECTILE_DEMO_IMPULSE = 4000;
    float PROJECTILE_DEMO_ACCURACY = 100;
    float PROJECTILE_DEMO_MASS = 1;
    float PROJECTILE_DEMO_TTL = 60;

    bool CLICK_SELECT_OBJECT3D = true;
    bool GAMEPAD_CONTROLLER_ENABLED = true;

    enum collisionGroups {
        Player = 1,
        Enemy = 2,
        Projectile = 4,
        Health = 8,
        Weapon = 16,
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

    enum SoundChannels {
        SND_GLOBAL = -1,
    };

    enum ShaderTypes {
        BACKGROUND = 1,
        WATER = 2,
        FIRE = 3,
        TINT_SCREEN = 4,
        SILHOUETTE = 5,
        SMOKE = 6,
        BLINK = 7,
    };

    enum ShadersPhaseRender {
        PREUPDATE = 0,
        POSTUPDATE = 1,
    };

    typedef enum {
        NONE = 0,
        SPLASH = 1,
        MENU = 2,
        PRESSKEY_NEWLEVEL = 3,
        PRESSKEY_BY_DEAD = 4,
        COUNTDOWN = 5,
        GAMING = 6,
        HELP = 7,
        PRESSKEY_GAMEOVER = 8,
        CREDITS = 9
    } GameState;
};

#endif //SDL2_3D_ENGINE_ENGINESETUP_H
