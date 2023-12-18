#ifndef SDL2_3D_ENGINE_ENGINESETUP_H
#define SDL2_3D_ENGINE_ENGINESETUP_H

#include "Misc/Color.h"
#include "Objects/Vertex3D.h"
#include "Misc/Timer.h"
#include "../imgui/imgui.h"
#include <map>

class EngineSetup {

public:
    static EngineSetup *get();

    static EngineSetup *instance;

    bool LOGGING = false;

    bool IMGUI_ENABLED = true;

    std::string ENGINE_VERSION = "v.1.0.0";

    std::string ENGINE_TITLE = "Brakeza3D Demo (" + ENGINE_VERSION + ")";

    std::string cameraNameIdentifier = "Camera";
    std::string cameraTriggerNameIdentifier = "triggerCamera";

    const std::string iconApplication = "application.png";

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
    const int screenWidth = 1024;
    const int screenHeight = 768;

    float screenWidthHalf = (float) screenWidth / 2;
    float screenHeightHalf = (float) screenHeight / 2;

    bool LIMIT_FRAMERATE = true;
    int FRAMERATE = 60;

    int RESOLUTION = screenWidth * screenHeight;

    bool BASED_TILE_RENDER = false;
    bool BASED_TILE_RENDER_THREADED = false;

    float EPSILON = 0.00001f;

    bool ENABLE_FOG = false;
    float FOG_DISTANCE = 4500;
    float FOG_INTENSITY = 1.f;
    Color FOG_COLOR = Color::FOGDefault();
    float FRUSTUM_FARPLANE_DISTANCE = 9000000;
    float HORIZONTAL_FOV = 90;

    bool DRAW_TILES_GRID = false;

    // Draw axis
    bool RENDER_MAIN_AXIS = false;
    bool RENDER_OBJECTS_AXIS = false;
    float OBJECT_AXIS_SIZE = 1;

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

    bool DRAW_OBJECT3D_BILLBOARD = false;
    bool DRAW_LIGHTPOINTS_BILLBOARD = true;
    bool DRAW_PENDULUM_THREAD = false;


    bool DRAW_MESH3D_AABB = false;
    bool DRAW_MESH3D_OCTREE = false;
    bool DRAW_MESH3D_GRID = false;
    bool DRAW_MESH3D_GRID_POINTS = true;
    bool DRAW_MESH3D_GRID_CUBES = false;
    bool DRAW_MESH3D_GRID_EMPTY = true;
    bool DRAW_MESH3D_GRID_NO_EMPTY = true;

    // Z BUFFER
    bool ENABLE_LIGHTS = true;
    bool DRAW_LIGHTS_DIRECTION = false;
    float LIGHTS_DIRECTION_SIZE = 2500;

    bool FULLSCREEN = false;

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
    std::string ROOT_FOLDER = "../";
    std::string SPRITES_FOLDER = "../assets/sprites/";
    std::string MAPS_FOLDER = "../assets/maps/";
    std::string IMAGES_FOLDER = "../assets/images/";
    std::string ASSETS_FOLDER = "../assets/";
    std::string VIDEOS_FOLDER = "../assets/videos/";
    std::string SOUNDS_FOLDER = "../assets/sounds/";
    std::string MODELS_FOLDER = "../assets/models/";
    std::string HUD_FOLDER = ASSETS_FOLDER + "textures/HUD/";
    std::string FONTS_FOLDER = ASSETS_FOLDER + "fonts/";
    std::string WEAPONS_FOLDER = ASSETS_FOLDER + "weapons/";
    std::string TEXTURES_FOLDER = ASSETS_FOLDER + "textures/";
    std::string GRIDS_FOLDER = ASSETS_FOLDER + "grids/";
    std::string ICONS_FOLDER = ASSETS_FOLDER + "icons/";
    std::string SCRIPTS_FOLDER = ASSETS_FOLDER + "scripts/";
    std::string SCENES_FOLDER = ASSETS_FOLDER + "scenes/";

    std::string CL_SHADERS_FOLDER = "../kernels/";

    bool SOUND_ENABLED = true;
    float SOUND_VOLUME_MUSIC = 20;
    float SOUND_CHANNEL_GLOBAL = 50;

    // SpriteDirectional3D Default size
    float BILLBOARD_WIDTH_DEFAULT = 100.f;
    float BILLBOARD_HEIGHT_DEFAULT = 100.f;

    Uint32 CROSSHAIR_COLOR = 0x00FF00; // GREEN
    bool DRAW_CROSSHAIR = false;
    bool TEXT_ON_OBJECT3D = false;

    // KEYBOARD
    float WALKING_SPEED = 0.1f;
    float TURN_SPEED = 0.1f;
    float PITCH_SPEED = 0.1f;
    float STRAFE_SPEED = 0.1f;

    // MOUSE
    float MOUSE_SENSITIVITY = 0.250;

    // COLLISION SYSTEM
    bool BULLET_STEP_SIMULATION = true;
    bool BULLET_DEBUG_MODE = false;
    bool BULLET_CHECK_ALL_PAIRS = true;

    float TESTING_INT1 = 0.1f;
    float TESTING_INT2 = 0.03f;
    float TESTING_INT3 = 1;
    float TESTING_INT4 = 0.1f;

    bool ENABLE_DEPTH_OF_FIELD = true;
    float LIGHTMAPPING_BLEND_INTENSITY = 0.05f;
    float LIGHTMAPPING_INTENSITY = 0.75f;

    //LIGHTNING EFFECT
    float LIGHTNING_GENERATIONS = 5;
    float LIGHTNING_OFFSET_REDUCTION = 650.f;
    float LIGHTNING_PROBABILITY_BRANCH = 1.5f;
    float LIGHTNING_SEGMENT_SHIFT = 2.5f;

    bool LOG_COLLISION_OBJECTS = false;
    bool LOG_WEAPONS_SYSTEM = false;

    float ZOOM_FOV = 40;

    float PLAYER_CAPSULE_RADIUS = 16.00f;
    float PLAYER_CAPSULE_HEIGHT = 32.00f;

    float PROJECTILE_DEMO_IMPULSE = 4000;
    float PROJECTILE_DEMO_ACCURACY = 100;
    float PROJECTILE_DEMO_MASS = 1;

    bool CLICK_SELECT_OBJECT3D = true;
    bool GAMEPAD_CONTROLLER_ENABLED = true;

    // PARTICLES_SHADER
    float PARTICLES_SHADER_GRAVITY = 0.0f;
    float PARTICLES_SHADER_STEP_ADD_PARTICLE = 0.0025f;
    float PARTICLES_SHADER_PARTICLE_LIFESPAN = 1.5f;
    float PARTICLES_SHADER_SMOKE_ANGLE_RANGE = 25.0f;
    float PARTICLES_SHADER_MIN_VELOCITY = 0.0f;
    float PARTICLES_SHADER_MAX_VELOCITY = 0.1f;
    float PARTICLES_SHADER_ALPHA_MIN = 125.0f;
    float PARTICLES_SHADER_ALPHA_MAX = 255.0f;
    float PARTICLES_SHADER_POSITION_NOISE = 0.2f;
    float PARTICLES_SHADER_VELOCITY_NOISE = 0.91f;
    float PARTICLES_SHADER_DECELERATION_FACTOR = 0.98f;

    float SHADER_PARTICLE_EXPLOSION_TTL = 1.0f;
    float SHADER_PARTICLE_EXPLOSION_EMISSION_TIME = 0.25f;

    std::string CONFIG_FOLDER = "../config/";
    std::string CFG_MENU = "menu.json";
    std::string CFG_WEAPONS = "playerWeapons.json";
    std::string CFG_SOUNDS = "sounds.json";

    std::string DEFAULT_HELP_IMAGE = "xbox_one_controller.png";

    std::string LOGO_BRAKEZA = "logo.png";

    std::string DARKHEAZ_ROOT_FOLDER = ROOT_FOLDER + "ProjectDarkHeaZ/";
    std::string DARKHEAZ_MAIN_CONFIG = "project-darkheaz.json";

    enum collisionGroups {
        Player = 1,
        Enemy = 2,
        Projectile = 4,
        ProjectileEnemy = 8,
        Health = 16,
        Weapon = 32,
        AllFilter = -1  //all bits sets: DefaultFilter | StaticFilter | KinematicFilter | DebrisFilter | SensorTrigger
    };

    enum SoundChannels {
        SND_GLOBAL = -1,
    };

    typedef enum {
        LUA_STOP = 0,
        LUA_PLAY = 1,
    } LuaStateScripts;

    typedef enum {
        NONE = 0,
        SPLASH = 1,
        MENU = 2,
        PRESS_KEY_NEW_LEVEL = 3,
        PRESS_KEY_PREVIOUS_LEVEL = 4,
        PRESS_KEY_BY_DEAD = 5,
        COUNTDOWN = 6,
        GAMING = 7,
        HELP = 8,
        PRESS_KEY_GAMEOVER = 9,
        CREDITS = 10,
        PRESS_KEY_BY_WIN = 11,
        INTRO = 12,
        GAMING_TUTORIAL = 13,
        STORE = 14,
        SPACESHIP_SELECTOR = 16
    } GameState;

    typedef enum {
        ITEM_EXTRA_POWER = 0,
        ITEM_FAST_ENERGY_RELOAD = 1,
        ITEM_SATELLITE = 2,
        ITEM_MIRROR_SHOOT = 3,
        ITEM_EXTRA_DASH = 4,
        ITEM_FAST_SHOOT_CADENCE = 5
    } StoreItems;
};

#endif //SDL2_3D_ENGINE_ENGINESETUP_H
